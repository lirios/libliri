/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "desktopmenu.h"
#include "xdgmenuapplinkprocessor_p_p.h"
#include "xmlhelper_p_p.h"
#include "desktopfile.h"

#include <QDir>

namespace Liri {

XdgMenuApplinkProcessor::XdgMenuApplinkProcessor(QDomElement &element, Liri::DesktopMenu *menu, XdgMenuApplinkProcessor *parent)
    : QObject(parent)
    , mElement(element)
    , mParent(parent)
    , mMenu(menu)
{
    mOnlyUnallocated = element.attribute(QStringLiteral("onlyUnallocated")) == QStringLiteral("1");

    MutableDomElementIterator i(element, QStringLiteral("Menu"));
    while (i.hasNext()) {
        QDomElement e = i.next();
        mChilds.append(new XdgMenuApplinkProcessor(e, mMenu, this));
    }
}

XdgMenuApplinkProcessor::~XdgMenuApplinkProcessor()
{
}

void XdgMenuApplinkProcessor::run()
{
    step1();
    step2();
}

void XdgMenuApplinkProcessor::step1()
{
    fillAppFileInfoList();
    createRules();

    // Check Include rules & mark as allocated ............
    XdgMenuAppFileInfoHashIterator i = mAppFileInfoHash.begin();
    while (i != mAppFileInfoHash.end()) {
        Liri::DesktopFile *file = i.value()->desktopFile();

        if (mRules.checkInclude(i.key(), *file)) {
            if (!mOnlyUnallocated)
                i.value()->setAllocated(true);

            if (!mRules.checkExclude(i.key(), *file)) {
                mSelected.append(i.value());
            }
        }

        ++i;
    }

    // Process childs menus ...............................

    for (XdgMenuApplinkProcessor *child : const_cast<const QLinkedList<XdgMenuApplinkProcessor *> &>(mChilds))
        child->step1();
}

void XdgMenuApplinkProcessor::step2()
{
    // Create AppLinks elements ...........................
    QDomDocument doc = mElement.ownerDocument();

    for (XdgMenuAppFileInfo *fileInfo : const_cast<const QLinkedList<XdgMenuAppFileInfo *> &>(mSelected)) {
        if (mOnlyUnallocated && fileInfo->allocated())
            continue;

        Liri::DesktopFile *file = fileInfo->desktopFile();

        bool show = false;
        QStringList envs = mMenu->environments();
        const int N = envs.size();
        for (int i = 0; i < N; ++i) {
            if (file->isVisible() && file->isSuitable(envs.at(i))) {
                show = true;
                break;
            }
        }

        if (!show)
            continue;

        QDomElement appLink = doc.createElement(QStringLiteral("AppLink"));

        appLink.setAttribute(QStringLiteral("id"), fileInfo->id());
        appLink.setAttribute(QStringLiteral("title"), file->name());
        appLink.setAttribute(QStringLiteral("comment"), file->comment());
        appLink.setAttribute(QStringLiteral("genericName"), file->genericName());
        appLink.setAttribute(QStringLiteral("exec"), file->exec());
        appLink.setAttribute(QStringLiteral("terminal"), file->runsOnTerminal());
        appLink.setAttribute(QStringLiteral("startupNoify"), file->startupNotify());
        appLink.setAttribute(QStringLiteral("path"), file->path());
        appLink.setAttribute(QStringLiteral("icon"), file->iconName());
        appLink.setAttribute(QStringLiteral("desktopFile"), file->fileName());

        mElement.appendChild(appLink);
    }

    // Process childs menus ...............................
    for (XdgMenuApplinkProcessor *child : const_cast<const QLinkedList<XdgMenuApplinkProcessor *> &>(mChilds))
        child->step2();
}

/************************************************
 For each <Menu> element, build a pool of desktop entries by collecting entries found
 in each <AppDir> for the menu element. If two entries have the same desktop-file id,
 the entry for the earlier (closer to the top of the file) <AppDir> must be discarded.

 Next, add to the pool the entries for any <AppDir>s specified by ancestor <Menu>
 elements. If a parent menu has a duplicate entry (same desktop-file id), the entry
 for the child menu has priority.
 ************************************************/
void XdgMenuApplinkProcessor::fillAppFileInfoList()
{
    // Build a pool by collecting entries found in <AppDir>
    {
        MutableDomElementIterator i(mElement, QStringLiteral("AppDir"));
        i.toBack();
        while (i.hasPrevious()) {
            QDomElement e = i.previous();
            findDesktopFiles(e.text(), QString());
            mElement.removeChild(e);
        }
    }

    // Add the entries for ancestor <Menu> ................
    if (mParent) {
        XdgMenuAppFileInfoHashIterator i = mParent->mAppFileInfoHash.begin();
        while (i != mParent->mAppFileInfoHash.end()) {
            //if (!mAppFileInfoHash.contains(i.key()))
            mAppFileInfoHash.insert(i.key(), i.value());

            ++i;
        }
    }
}

void XdgMenuApplinkProcessor::findDesktopFiles(const QString &dirName, const QString &prefix)
{
    QDir dir(dirName);
    mMenu->addWatchPath(dir.absolutePath());
    const QFileInfoList files = dir.entryInfoList(QStringList(QStringLiteral("*.desktop")), QDir::Files);

    for (const QFileInfo &file : files) {
        Liri::DesktopFile *f = Liri::DesktopFileCache::getFile(file.canonicalFilePath());
        if (f)
            mAppFileInfoHash.insert(prefix + file.fileName(), new XdgMenuAppFileInfo(f, prefix + file.fileName(), this));
    }

    // Working recursively ............
    const QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &d : dirs) {
        QString dn = d.canonicalFilePath();
        if (dn != dirName) {
            findDesktopFiles(dn, QStringLiteral("%1%2-").arg(prefix, d.fileName()));
        }
    }
}

void XdgMenuApplinkProcessor::createRules()
{
    MutableDomElementIterator i(mElement, QString());
    while (i.hasNext()) {
        QDomElement e = i.next();
        if (e.tagName() == QLatin1String("Include")) {
            mRules.addInclude(e);
            mElement.removeChild(e);
        }

        else if (e.tagName() == QLatin1String("Exclude")) {
            mRules.addExclude(e);
            mElement.removeChild(e);
        }
    }
}

/************************************************
 Check if the program is actually installed.
 ************************************************/
bool XdgMenuApplinkProcessor::checkTryExec(const QString &progName)
{
    if (progName.startsWith(QDir::separator()))
        return QFileInfo(progName).isExecutable();

    const QStringList dirs = QFile::decodeName(qgetenv("PATH")).split(QLatin1Char(':'));

    for (const QString &dir : dirs) {
        if (QFileInfo(QDir(dir), progName).isExecutable())
            return true;
    }

    return false;
}

} // namespace Liri
