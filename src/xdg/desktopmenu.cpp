/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2010-2011 Razor team
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QDir>
#include <QHash>
#include <QLocale>
#include <QTranslator>
#include <QCoreApplication>
#include <QCryptographicHash>

#include "desktopmenu_p.h"
#include "xdgdirs_p_p.h"
#include "xdgmenuapplinkprocessor_p_p.h"
#include "xdgmenulayoutprocessor_p_p.h"
#include "xdgmenureader_p_p.h"
#include "xdgmenurules_p_p.h"
#include "xmlhelper_p_p.h"
#include "logging_p.h"

namespace Liri {

// Helper functions prototypes
void installTranslation(const QString &name);
bool isParent(const QDomElement &parent, const QDomElement &child);

DesktopMenu::DesktopMenu(QObject *parent)
    : QObject(parent)
    , d_ptr(new DesktopMenuPrivate(this))
{
}

DesktopMenu::~DesktopMenu()
{
    Q_D(DesktopMenu);
    delete d;
}

DesktopMenuPrivate::DesktopMenuPrivate(DesktopMenu *parent)
    : mOutDated(true)
    , q_ptr(parent)
{
    mRebuildDelayTimer.setSingleShot(true);
    mRebuildDelayTimer.setInterval(REBUILD_DELAY);

    connect(&mRebuildDelayTimer, SIGNAL(timeout()), this, SLOT(rebuild()));
    connect(&mWatcher, SIGNAL(fileChanged(QString)), &mRebuildDelayTimer, SLOT(start()));
    connect(&mWatcher, SIGNAL(directoryChanged(QString)), &mRebuildDelayTimer, SLOT(start()));

    connect(this, SIGNAL(changed()), q_ptr, SIGNAL(changed()));
}

const QString DesktopMenu::logDir() const
{
    Q_D(const DesktopMenu);
    return d->mLogDir;
}

void DesktopMenu::setLogDir(const QString &directory)
{
    Q_D(DesktopMenu);
    d->mLogDir = directory;
}

const QDomDocument DesktopMenu::xml() const
{
    Q_D(const DesktopMenu);
    return d->mXml;
}

QString DesktopMenu::menuFileName() const
{
    Q_D(const DesktopMenu);
    return d->mMenuFileName;
}

QStringList DesktopMenu::environments()
{
    Q_D(DesktopMenu);
    return d->mEnvironments;
}

void DesktopMenu::setEnvironments(const QStringList &envs)
{
    Q_D(DesktopMenu);
    d->mEnvironments = envs;
}

void DesktopMenu::setEnvironments(const QString &env)
{
    setEnvironments(QStringList() << env);
}

const QString DesktopMenu::errorString() const
{
    Q_D(const DesktopMenu);
    return d->mErrorString;
}

bool DesktopMenu::read(const QString &menuFileName)
{
    Q_D(DesktopMenu);

    d->mMenuFileName = menuFileName;

    d->clearWatcher();

    XdgMenuReader reader(this);
    if (!reader.load(d->mMenuFileName)) {
        qCWarning(lcXdg) << reader.errorString();
        d->mErrorString = reader.errorString();
        return false;
    }

    d->mXml = reader.xml();
    QDomElement root = d->mXml.documentElement();
    d->saveLog(QLatin1String("00-reader.xml"));

    d->simplify(root);
    d->saveLog(QLatin1String("01-simplify.xml"));

    d->mergeMenus(root);
    d->saveLog(QLatin1String("02-mergeMenus.xml"));

    d->moveMenus(root);
    d->saveLog(QLatin1String("03-moveMenus.xml"));

    d->mergeMenus(root);
    d->saveLog(QLatin1String("04-mergeMenus.xml"));

    d->deleteDeletedMenus(root);
    d->saveLog(QLatin1String("05-deleteDeletedMenus.xml"));

    d->processDirectoryEntries(root, QStringList());
    d->saveLog(QLatin1String("06-processDirectoryEntries.xml"));

    d->processApps(root);
    d->saveLog(QLatin1String("07-processApps.xml"));

    d->processLayouts(root);
    d->saveLog(QLatin1String("08-processLayouts.xml"));

    d->deleteEmpty(root);
    d->saveLog(QLatin1String("09-deleteEmpty.xml"));

    d->fixSeparators(root);
    d->saveLog(QLatin1String("10-fixSeparators.xml"));

    d->mOutDated = false;
    d->mHash = QCryptographicHash::hash(d->mXml.toByteArray(), QCryptographicHash::Md5);

    return true;
}

void DesktopMenu::save(const QString &fileName)
{
    Q_D(const DesktopMenu);

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        qCWarning(lcXdg, "Cannot write file \"%s\": %s",
                  qPrintable(fileName), qPrintable(file.errorString()));
        return;
    }

    QTextStream ts(&file);
    d->mXml.save(ts, 2);

    file.close();
}

/************************************************
 For debug only
 ************************************************/
void DesktopMenuPrivate::load(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qCWarning(lcXdg, "Failed to load \"%s\": %s",
                  qPrintable(fileName), qPrintable(file.errorString()));
        return;
    }
    mXml.setContent(&file, true);
}

void DesktopMenuPrivate::saveLog(const QString &logFileName)
{
    Q_Q(DesktopMenu);
    if (!mLogDir.isEmpty())
        q->save(mLogDir + QLatin1Char('/') + logFileName);
}

void DesktopMenuPrivate::mergeMenus(QDomElement &element)
{
    QHash<QString, QDomElement> menus;

    MutableDomElementIterator it(element, QLatin1String("Menu"));

    it.toFront();
    while (it.hasNext()) {
        it.next();
        menus[it.current().attribute(QLatin1String("name"))] = it.current();
    }

    it.toBack();
    while (it.hasPrevious()) {
        QDomElement src = it.previous();
        QDomElement dest = menus[src.attribute(QLatin1String("name"))];
        if (dest != src) {
            prependChilds(src, dest);
            element.removeChild(src);
        }
    }

    QDomElement n = element.firstChildElement(QLatin1String("Menu"));
    while (!n.isNull()) {
        mergeMenus(n);
        n = n.nextSiblingElement(QLatin1String("Menu"));
    }

    it.toFront();
    while (it.hasNext())
        mergeMenus(it.next());
}

void DesktopMenuPrivate::simplify(QDomElement &element)
{
    MutableDomElementIterator it(element);
    // it.toFront();
    while (it.hasNext()) {
        QDomElement n = it.next();

        if (n.tagName() == QLatin1String("Name")) {
            // The <Name> field must not contain the slash character ("/");
            // implementations should discard any name containing a slash.
            element.setAttribute(QLatin1String("name"), n.text().remove(QLatin1Char('/')));
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if (n.tagName() == QLatin1String("Deleted")) {
            element.setAttribute(QLatin1String("deleted"), true);
            n.parentNode().removeChild(n);
        } else if (n.tagName() == QLatin1String("NotDeleted")) {
            element.setAttribute(QLatin1String("deleted"), false);
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if (n.tagName() == QLatin1String("OnlyUnallocated")) {
            element.setAttribute(QLatin1String("onlyUnallocated"), true);
            n.parentNode().removeChild(n);
        } else if (n.tagName() == QLatin1String(QLatin1String("NotOnlyUnallocated"))) {
            element.setAttribute(QLatin1String("onlyUnallocated"), false);
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if (n.tagName() == QLatin1String("FileInfo")) {
            n.parentNode().removeChild(n);
        }

        // ......................................
        else if (n.tagName() == QLatin1String("Menu")) {
            simplify(n);
        }
    }
}

void DesktopMenuPrivate::prependChilds(QDomElement &srcElement, QDomElement &destElement)
{
    MutableDomElementIterator it(srcElement);

    it.toBack();
    while (it.hasPrevious()) {
        QDomElement n = it.previous();
        destElement.insertBefore(n, destElement.firstChild());
    }

    if (srcElement.attributes().contains(QLatin1String("deleted"))
        && !destElement.attributes().contains(QLatin1String("deleted")))
        destElement.setAttribute(QLatin1String("deleted"),
                                 srcElement.attribute(QLatin1String("deleted")));

    if (srcElement.attributes().contains(QLatin1String("onlyUnallocated"))
        && !destElement.attributes().contains(QLatin1String("onlyUnallocated")))
        destElement.setAttribute(QLatin1String("onlyUnallocated"),
                                 srcElement.attribute(QLatin1String("onlyUnallocated")));
}

void DesktopMenuPrivate::appendChilds(QDomElement &srcElement, QDomElement &destElement)
{
    MutableDomElementIterator it(srcElement);

    while (it.hasNext())
        destElement.appendChild(it.next());

    if (srcElement.attributes().contains(QLatin1String("deleted")))
        destElement.setAttribute(QLatin1String("deleted"),
                                 srcElement.attribute(QLatin1String("deleted")));

    if (srcElement.attributes().contains(QLatin1String("onlyUnallocated")))
        destElement.setAttribute(QLatin1String("onlyUnallocated"),
                                 srcElement.attribute(QLatin1String("onlyUnallocated")));
}

/************************************************
 Search item by path. The path can be absolute or relative. If the element not
 found, the behavior depends on a parameter "createNonExisting." If it's true, then
 the missing items will be created, otherwise the function returns 0.
 ************************************************/
QDomElement DesktopMenu::findMenu(QDomElement &baseElement, const QString &path,
                                  bool createNonExisting)
{
    Q_D(DesktopMenu);
    // Absolute path ..................
    if (path.startsWith(QLatin1Char('/'))) {
        QDomElement root = d->mXml.documentElement();
        return findMenu(root, path.section(QLatin1Char('/'), 2), createNonExisting);
    }

    // Relative path ..................
    if (path.isEmpty())
        return baseElement;

    QString name = path.section(QLatin1Char('/'), 0, 0);
    MutableDomElementIterator it(baseElement);
    while (it.hasNext()) {
        QDomElement n = it.next();
        if (n.attribute(QLatin1String("name")) == name)
            return findMenu(n, path.section(QLatin1Char('/'), 1), createNonExisting);
    }

    // Not found ......................
    if (!createNonExisting)
        return QDomElement();

    const QStringList names = path.split(QLatin1Char('/'), QString::SkipEmptyParts);
    QDomElement el = baseElement;
    for (const QString &name : names) {
        QDomElement p = el;
        el = d->mXml.createElement(QLatin1String("Menu"));
        p.appendChild(el);
        el.setAttribute(QLatin1String("name"), name);
    }
    return el;
}

bool isParent(const QDomElement &parent, const QDomElement &child)
{
    QDomNode n = child;
    while (!n.isNull()) {
        if (n == parent)
            return true;
        n = n.parentNode();
    }
    return false;
}

/************************************************
 Recurse to resolve <Move> elements for each menu starting with any child menu before
 handling the more top level menus. So the deepest menus have their <Move> operations
 performed first. Within each <Menu>, execute <Move> operations in the order that
 they appear.
 If the destination path does not exist, simply relocate the origin <Menu> element,
 and change its <Name> field to match the destination path.
 If the origin path does not exist, do nothing.
 If both paths exist, take the origin <Menu> element, delete its <Name> element, and
 prepend its remaining child elements to the destination <Menu> element.
 ************************************************/
void DesktopMenuPrivate::moveMenus(QDomElement &element)
{
    Q_Q(DesktopMenu);

    {
        MutableDomElementIterator i(element, QLatin1String("Menu"));
        while (i.hasNext())
            moveMenus(i.next());
    }

    MutableDomElementIterator i(element, QLatin1String("Move"));
    while (i.hasNext()) {
        i.next();
        QString oldPath = i.current().lastChildElement(QLatin1String("Old")).text();
        QString newPath = i.current().lastChildElement(QLatin1String("New")).text();

        element.removeChild(i.current());

        if (oldPath.isEmpty() || newPath.isEmpty())
            continue;

        QDomElement oldMenu = q->findMenu(element, oldPath, false);
        if (oldMenu.isNull())
            continue;

        QDomElement newMenu = q->findMenu(element, newPath, true);

        if (isParent(oldMenu, newMenu))
            continue;

        appendChilds(oldMenu, newMenu);
        oldMenu.parentNode().removeChild(oldMenu);
    }
}

/************************************************
 For each <Menu> containing a <Deleted> element which is not followed by a
 <NotDeleted> element, remove that menu and all its child menus.

 Kmenuedit create .hidden menu entry, delete it too.
 ************************************************/
void DesktopMenuPrivate::deleteDeletedMenus(QDomElement &element)
{
    MutableDomElementIterator i(element, QLatin1String("Menu"));
    while (i.hasNext()) {
        QDomElement e = i.next();
        if (e.attribute(QLatin1String("deleted")) == QLatin1String("1")
            || e.attribute(QLatin1String("name")) == QLatin1String(".hidden"))
            element.removeChild(e);
        else
            deleteDeletedMenus(e);
    }
}

void DesktopMenuPrivate::processDirectoryEntries(QDomElement &element,
                                                 const QStringList &parentDirs)
{
    QStringList dirs;
    QStringList files;

    element.setAttribute(QLatin1String("title"), element.attribute(QLatin1String("name")));

    MutableDomElementIterator i(element, QString());
    i.toBack();
    while (i.hasPrevious()) {
        QDomElement e = i.previous();

        if (e.tagName() == QLatin1String("Directory")) {
            files << e.text();
            element.removeChild(e);
        }

        else if (e.tagName() == QLatin1String("DirectoryDir")) {
            dirs << e.text();
            element.removeChild(e);
        }
    }

    dirs << parentDirs;

    bool found = false;
    for (const QString &file : const_cast<const QStringList &>(files)) {
        if (file.startsWith(QLatin1Char('/')))
            found = loadDirectoryFile(file, element);
        else {
            for (const QString &dir : const_cast<const QStringList &>(dirs)) {
                found = loadDirectoryFile(dir + QLatin1Char('/') + file, element);
                if (found)
                    break;
            }
        }
        if (found)
            break;
    }

    MutableDomElementIterator it(element, QLatin1String("Menu"));
    while (it.hasNext()) {
        QDomElement e = it.next();
        processDirectoryEntries(e, dirs);
    }
}

bool DesktopMenuPrivate::loadDirectoryFile(const QString &fileName, QDomElement &element)
{
    DesktopFile file;
    file.load(fileName);

    if (!file.isValid())
        return false;

    element.setAttribute(QLatin1String("title"), file.name());
    element.setAttribute(QLatin1String("comment"), file.comment());
    element.setAttribute(QLatin1String("icon"), file.iconName());

    Q_Q(DesktopMenu);
    q->addWatchPath(QFileInfo(file.fileName()).absolutePath());
    return true;
}

void DesktopMenuPrivate::processApps(QDomElement &element)
{
    Q_Q(DesktopMenu);
    XdgMenuApplinkProcessor processor(element, q);
    processor.run();
}

void DesktopMenuPrivate::deleteEmpty(QDomElement &element)
{
    MutableDomElementIterator it(element, QLatin1String("Menu"));
    while (it.hasNext())
        deleteEmpty(it.next());

    if (element.attribute(QLatin1String("keep")) == QLatin1String("true"))
        return;

    QDomElement childMenu = element.firstChildElement(QLatin1String("Menu"));
    QDomElement childApps = element.firstChildElement(QLatin1String("AppLink"));

    if (childMenu.isNull() && childApps.isNull()) {
        element.parentNode().removeChild(element);
    }
}

void DesktopMenuPrivate::processLayouts(QDomElement &element)
{
    XdgMenuLayoutProcessor proc(element);
    proc.run();
}

void DesktopMenuPrivate::fixSeparators(QDomElement &element)
{
    MutableDomElementIterator it(element, QLatin1String("Separator"));
    while (it.hasNext()) {
        QDomElement s = it.next();
        if (s.previousSiblingElement().tagName() == QLatin1String("Separator"))
            element.removeChild(s);
    }

    QDomElement first = element.firstChild().toElement();
    if (first.tagName() == QLatin1String("Separator"))
        element.removeChild(first);

    QDomElement last = element.lastChild().toElement();
    if (last.tagName() == QLatin1String("Separator"))
        element.removeChild(last);

    MutableDomElementIterator mi(element, QLatin1String("Menu"));
    while (mi.hasNext())
        fixSeparators(mi.next());
}

/************************************************
 $XDG_CONFIG_DIRS/menus/${XDG_MENU_PREFIX}applications.menu
 The first file found in the search path should be used; other files are ignored.
 ************************************************/
QString DesktopMenu::getMenuFileName(const QString &baseName)
{
    const QStringList configDirs = XdgDirs::configDirs();
    QString menuPrefix = QString::fromLocal8Bit(qgetenv("XDG_MENU_PREFIX"));

    for (const QString &configDir : configDirs) {
        QFileInfo file(QString::fromLatin1("%1/menus/%2%3").arg(configDir, menuPrefix, baseName));
        if (file.exists())
            return file.filePath();
    }

    QStringList wellKnownFiles;
    // liri- is a priority for us
    wellKnownFiles << QLatin1String("liri-applications.menu");
    // the "global" menu file name on suse and fedora
    wellKnownFiles << QLatin1String("applications.menu");
    // rest files ordered by priority (descending)
    wellKnownFiles << QLatin1String("razor-applications.menu");
    wellKnownFiles << QLatin1String("kde4-applications.menu");
    wellKnownFiles << QLatin1String("kde-applications.menu");
    wellKnownFiles << QLatin1String("gnome-applications.menu");
    wellKnownFiles << QLatin1String("lxde-applications.menu");

    for (const QString &configDir : configDirs) {
        for (const QString &f : const_cast<const QStringList &>(wellKnownFiles)) {
            QFileInfo file(QString::fromLatin1("%1/menus/%2").arg(configDir, f));
            if (file.exists())
                return file.filePath();
        }
    }

    return QString();
}

void DesktopMenu::addWatchPath(const QString &path)
{
    Q_D(DesktopMenu);

    if (d->mWatcher.files().contains(path))
        return;

    if (d->mWatcher.directories().contains(path))
        return;

    d->mWatcher.addPath(path);
}

bool DesktopMenu::isOutDated() const
{
    Q_D(const DesktopMenu);
    return d->mOutDated;
}

void DesktopMenuPrivate::rebuild()
{
    Q_Q(DesktopMenu);
    QByteArray prevHash = mHash;
    q->read(mMenuFileName);

    if (prevHash != mHash) {
        mOutDated = true;
        Q_EMIT changed();
    }
}

void DesktopMenuPrivate::clearWatcher()
{
    QStringList sl;
    sl << mWatcher.files();
    sl << mWatcher.directories();
    if (sl.length())
        mWatcher.removePaths(sl);
}

} // namespace Liri
