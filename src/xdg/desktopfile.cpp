/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDBusInterface>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

#include "desktopfile.h"
#include "desktopfile_p.h"
#include "desktopfileutils_p.h"
#include "xdgdirs_p_p.h"
#include "logging_p.h"

namespace Liri {

Q_GLOBAL_STATIC(DesktopFileCache, s_desktopFileCache)

// A list of executables that can't be run with QProcess::startDetached(). They
// will be run with QProcess::start()
static const QStringList nonDetachExecs = QStringList() << QLatin1String("pkexec");

static const QString typeKey = QStringLiteral("Type");
static const QString versionKey = QStringLiteral("Version");
static const QString nameKey = QStringLiteral("Name");
static const QString genericNameKey = QStringLiteral("GenericName");
static const QString commentKey = QStringLiteral("Comment");
static const QString keywordsKey = QStringLiteral("Keywords");
static const QString iconKey = QStringLiteral("Icon");
static const QString noDisplayKey = QStringLiteral("NoDisplay");
static const QString hiddenKey = QStringLiteral("Hidden");
static const QString onlyShowInKey = QStringLiteral("OnlyShowIn");
static const QString notShowInKey = QStringLiteral("NotShowIn");
static const QString dbusActivableKey = QStringLiteral("DBusActivatable");
static const QString tryExecKey = QStringLiteral("TryExec");
static const QString execKey = QStringLiteral("Exec");
static const QString pathKey = QStringLiteral("Path");
static const QString urlKey = QStringLiteral("URL");
static const QString terminalKey = QStringLiteral("Terminal");
static const QString startupNotifyKey = QStringLiteral("StartupNotify");
static const QString actionsKey = QStringLiteral("Actions");
static const QString mimeTypeKey = QStringLiteral("MimeType");
static const QString categoriesKey = QStringLiteral("Categories");
static const QString implementsKey = QStringLiteral("Implements");
static const QString initialPreferenceKey = QStringLiteral("InitialPreference");

static const QString applicationsStr = QStringLiteral("applications");

/*
 * DesktopFilePrivate
 */

void DesktopFilePrivate::clear()
{
    fileName.clear();
    prefix.clear();
    items.clear();
    type = DesktopFile::UnknownType;
}

bool DesktopFilePrivate::readFile()
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return false;

    QString section;
    QTextStream stream(&file);

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip empty lines
        if (line.isEmpty())
            continue;

        // Skip comments
        if (line.startsWith(QLatin1Char('#')))
            continue;

        // Detect section
        if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']'))) {
            section = line.mid(1, line.length() - 2);
            continue;
        }

        // Read key and value
        QString key = line.section(QLatin1Char('='), 0, 0).trimmed();
        QString value = line.section(QLatin1Char('='), 1).trimmed();

        if (key.isEmpty())
            continue;

        if (section.isEmpty()) {
            qCWarning(lcXdg, "Stray assignment outside section");
            file.close();
            return false;
        }

        // Prepend section and '/' separator before key
        key.prepend(QLatin1Char('/')).prepend(section);

        // Decode lists
        if (value.contains(QLatin1Char(';')))
            items.insert(key, value.split(QLatin1Char(';'), QString::SkipEmptyParts));
        else
            items.insert(key, value);
    }

    file.close();

    return true;
}

DesktopFile::Type DesktopFilePrivate::detectType(DesktopFile *q) const
{
    QString typeString = q->value(typeKey).toString();
    if (typeString == QStringLiteral("Application"))
        return DesktopFile::ApplicationType;
    else if (typeString == QStringLiteral("Link"))
        return DesktopFile::LinkType;
    else if (typeString == QStringLiteral("Directory"))
        return DesktopFile::DirectoryType;
    else if (!q->value(execKey).toString().isEmpty())
        return DesktopFile::ApplicationType;
    else
        return DesktopFile::UnknownType;
}

bool DesktopFilePrivate::checkTryExec(const QString &progName) const
{
    if (progName.startsWith(QDir::separator()))
        return QFileInfo(progName).isExecutable();

    const auto dirs = qEnvironmentVariable("PATH").split(QLatin1Char(':'));
    for (const auto &dir : dirs) {
        if (QFileInfo(QDir(dir), progName).isExecutable())
            return true;
    }

    return false;
}

bool DesktopFilePrivate::contains(const QString &key) const
{
    const QString path = !prefix.isEmpty() ? QStringLiteral("%1/%2").arg(prefix, key) : key;
    return items.contains(path);
}

/************************************************
 LC_MESSAGES value      Possible keys in order of matching
 lang_COUNTRY@MODIFIER  lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
                        default value
 lang_COUNTRY           lang_COUNTRY, lang, default value
 lang@MODIFIER          lang@MODIFIER, lang, default value
 lang                   lang, default value
 ************************************************/
QString DesktopFilePrivate::localizedKey(const QString &key) const
{
    QString lang = QString::fromLocal8Bit(qgetenv("LC_MESSAGES"));

    if (lang.isEmpty())
        lang = QString::fromLocal8Bit(qgetenv("LC_ALL"));

    if (lang.isEmpty())
        lang = QString::fromLocal8Bit(qgetenv("LANG"));

    QString modifier = lang.section(QLatin1Char('@'), 1);
    if (!modifier.isEmpty())
        lang.truncate(lang.length() - modifier.length() - 1);

    QString encoding = lang.section(QLatin1Char('.'), 1);
    if (!encoding.isEmpty())
        lang.truncate(lang.length() - encoding.length() - 1);

    QString country = lang.section(QLatin1Char('_'), 1);
    if (!country.isEmpty())
        lang.truncate(lang.length() - country.length() - 1);

    if (!modifier.isEmpty() && !country.isEmpty()) {
        QString k = QString::fromLatin1("%1[%2_%3@%4]").arg(key, lang, country, modifier);
        if (contains(k))
            return k;
    }

    if (!country.isEmpty()) {
        QString k = QString::fromLatin1("%1[%2_%3]").arg(key, lang, country);
        if (contains(k))
            return k;
    }

    if (!modifier.isEmpty()) {
        QString k = QString::fromLatin1("%1[%2@%3]").arg(key, lang, modifier);
        if (contains(k))
            return k;
    }

    QString k = QString::fromLatin1("%1[%2]").arg(key, lang);
    if (contains(k))
        return k;

    return key;
}

bool DesktopFilePrivate::startApplicationDetached(DesktopFile *q, const QString &actionName, const QStringList &urls)
{
    if (q->isDBusActivatable()) {
        /* WARNING: We fallback to use Exec when the DBusActivatable fails.
         *
         * This is a violation of the standard and we know it!
         *
         * From the Standard:
         * DBusActivatable	A boolean value specifying if D-Bus activation is
         * supported for this application. If this key is missing, the default
         * value is false. If the value is true then implementations should
         * ignore the Exec key and send a D-Bus message to launch the
         * application. See D-Bus Activation for more information on how this
         * works. Applications should still include Exec= lines in their desktop
         * files for compatibility with implementations that do not understand
         * the DBusActivatable key.
         *
         * So, why are we doing it ? In the benefit of user experience.
         * We first ignore the Exec line and in use the D-Bus to lauch the
         * application. But if it fails, we try the Exec method.
         *
         * We consider that this violation is more acceptable than an failure
         * in launching an application.
         */
        if (startByDBus(actionName, urls))
            return true;
    }

    DesktopFileAction action = q->action(actionName);

    QStringList args = actionName.isEmpty() ? q->expandExecString(urls)
                                            : action.expandExecString(urls);

    if (args.isEmpty())
        return false;

    if (q->runsOnTerminal()) {
        QString term = QString::fromLocal8Bit(qgetenv("TERM"));
        if (term.isEmpty())
            term = QLatin1String("xterm");

        args.prepend(QLatin1String("-e"));
        args.prepend(term);
    }

    bool nonDetach = false;
    for (const QString &s : nonDetachExecs) {
        for (const QString &a : const_cast<const QStringList &>(args)) {
            if (a.contains(s))
                nonDetach = true;
        }
    }

    QString cmd = args.takeFirst();
    QString workingDir = q->path();
    if (!workingDir.isEmpty() && !QDir(workingDir).exists())
        workingDir = QString();

    if (nonDetach) {
        QScopedPointer<QProcess> p(new QProcess);
        p->setStandardInputFile(QProcess::nullDevice());
        p->setProcessChannelMode(QProcess::ForwardedChannels);
        if (!workingDir.isEmpty())
            p->setWorkingDirectory(workingDir);
        p->start(cmd, args);
        bool started = p->waitForStarted();
        if (started) {
            QProcess *proc = p.take(); // release the pointer(will be selfdestroyed upon finish)
            QObject::connect(
                        proc,
                        static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                        proc, &QProcess::deleteLater);
        }
        return started;
    } else {
        return QProcess::startDetached(cmd, args, workingDir);
    }
}

bool DesktopFilePrivate::startLinkDetached(DesktopFile *q)
{
    const QUrl url = q->url();

    if (url.scheme().isEmpty() || url.scheme().toLower() == QStringLiteral("file")) {
        // Local file
        QMimeDatabase db;
        QMimeType mimeType = db.mimeTypeForFile(url.toLocalFile());
        DesktopFile *desktopFile = DesktopFileCache::getDefaultApp(mimeType.name());
        if (desktopFile)
            desktopFile->startDetached(url.toString());
    } else {
        // Remote URL
        return QDesktopServices::openUrl(url);
    }

    return false;
}

bool DesktopFilePrivate::startByDBus(const QString &action, const QStringList &urls)
{
    const QString id = DesktopFile::id(fileName);

    // Determine object path
    QString path(id);
    path = path.replace(QLatin1Char('.'), QLatin1Char('/')).prepend(QLatin1Char('/'));

    // Platform data
    QVariantMap platformData;

    // Call a method
    QDBusInterface interface(id, path, QStringLiteral("org.freedesktop.Application"));
    QDBusMessage reply;
    if (!action.isEmpty()) {
        QVariantList variantUrls;
        for (const auto &url : urls)
            variantUrls.append(url);
        reply = interface.call(QStringLiteral("ActivateAction"), action, variantUrls, platformData);
    } else if (urls.isEmpty()) {
        reply = interface.call(QStringLiteral("Activate"), platformData);
    } else {
        reply = interface.call(QStringLiteral("Open"), urls, platformData);
    }

    return reply.type() != QDBusMessage::ErrorMessage;
}

/*
 * DesktopFile
 */

DesktopFile::DesktopFile(const QString &fileName)
    : d(new DesktopFilePrivate())
{
    if (!fileName.isEmpty())
        load(fileName);
}

DesktopFile::DesktopFile(const DesktopFile &other)
    : d(other.d)
{
}

DesktopFile::~DesktopFile()
{
}

DesktopFile &DesktopFile::operator=(const DesktopFile &other)
{
    d = other.d;
    return *this;
}

bool DesktopFile::operator==(const DesktopFile &other) const
{
    return d->items == other.d->items;
}

QString DesktopFile::fileName() const
{
    return d->fileName;
}

bool DesktopFile::isValid() const
{
    return !d->fileName.isEmpty() && d->type != UnknownType;
}

DesktopFile::Type DesktopFile::type() const
{
    return d->type;
}

QString DesktopFile::version() const
{
    return value(versionKey, QStringLiteral("1.1")).toString();
}

QString DesktopFile::name() const
{
    return localizedValue(nameKey).toString();
}

QString DesktopFile::genericName() const
{
    return localizedValue(genericNameKey).toString();
}

QString DesktopFile::comment() const
{
    return localizedValue(commentKey).toString();
}

QStringList DesktopFile::keywords() const
{
    return localizedValue(keywordsKey).toStringList();
}

QString DesktopFile::iconName() const
{
    return localizedValue(iconKey).toString();
}

bool DesktopFile::noDisplay() const
{
    return value(noDisplayKey, false).toBool();
}

bool DesktopFile::isHidden() const
{
    return value(hiddenKey, false).toBool();
}

QStringList DesktopFile::onlyShowIn() const
{
    QStringList list = value(onlyShowInKey).toStringList();
    std::transform(list.begin(), list.end(), list.begin(), [](const QString &s) { return s.toLower(); });
    return list;
}

QStringList DesktopFile::notShowIn() const
{
    QStringList list = value(notShowInKey).toStringList();
    std::transform(list.begin(), list.end(), list.begin(), [](const QString &s) { return s.toLower(); });
    return list;
}

bool DesktopFile::isDBusActivatable() const
{
    return value(dbusActivableKey, false).toBool();
}

QString DesktopFile::tryExec() const
{
    return value(tryExecKey).toString();
}

QString DesktopFile::exec() const
{
    return value(execKey).toString();
}

QString DesktopFile::path() const
{
    return value(pathKey).toString();
}

QUrl DesktopFile::url() const
{
    if (type() != LinkType)
        return QUrl();

    QUrl url = value(urlKey).toUrl();
    if (!url.isEmpty())
        return url;

    url = expandDynamicUrl(value(QStringLiteral("URL[$e]")).toString());
    if (!url.isEmpty())
        return url;

    return QString();
}

bool DesktopFile::runsOnTerminal() const
{
    return value(terminalKey, false).toBool();
}

bool DesktopFile::startupNotify() const
{
    return value(startupNotifyKey, false).toBool();
}

QStringList DesktopFile::actionNames() const
{
    return value(actionsKey).toStringList();
}

DesktopFileAction DesktopFile::action(const QString &name) const
{
    return DesktopFileAction(*this, name);
}

QStringList DesktopFile::mimeTypes() const
{
    return value(mimeTypeKey).toStringList();
}

QStringList DesktopFile::categories() const
{
    return value(categoriesKey).toStringList();
}

QStringList DesktopFile::implements() const
{
    return value(implementsKey).toStringList();
}

QVariant DesktopFile::value(const QString &key, const QVariant &defaultValue) const
{
    const QString path = key.contains(QLatin1Char('/')) || d->prefix.isEmpty() ? key : QStringLiteral("%1/%2").arg(d->prefix, key);
    QVariant result = d->items.value(path, defaultValue);
    if (result.type() == QVariant::String) {
        QString string = result.toString();
        return unEscape(string);
    }

    return result;
}

void DesktopFile::setValue(const QString &key, const QVariant &value)
{
    const QString path = key.contains(QLatin1Char('/')) || d->prefix.isEmpty() ? key : QStringLiteral("%1/%2").arg(d->prefix, key);
    if (value.type() == QVariant::String) {
        QString string = value.toString();

        if (key.toLower() == execKey.toLower())
            escapeExec(string);
        else
            escape(string);

        d->items[path] = QVariant(string);

        if (key.toLower() == typeKey.toLower())
            d->type = d->detectType(this);
    } else {
        d->items[path] = value;
    }
}

QVariant DesktopFile::localizedValue(const QString &key, const QVariant &defaultValue) const
{
    return value(d->localizedKey(key), defaultValue);
}

void DesktopFile::setLocalizedValue(const QString &key, const QVariant &value)
{
    setValue(d->localizedKey(key), value);
}

bool DesktopFile::load(const QString &fileName)
{
    d->clear();

    d->fileName = fileName;
    if (!d->readFile())
        return false;

    beginGroup(QStringLiteral("Desktop Entry"));

    d->type = d->detectType(this);

    return true;
}

bool DesktopFile::save(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return false;

    QTextStream stream(&file);
    QMap<QString, QVariant>::const_iterator it = d->items.constBegin();

    QString section;
    while (it != d->items.constEnd()) {
        QString path = it.key();
        QString sect = path.section(QLatin1Char('/'), 0, 0);
        if (sect != section) {
            section = sect;
            stream << QLatin1Char('[') << section << QLatin1Char(']') << endl;
        }
        QString key = path.section(QLatin1Char('/'), 1);
        stream << key << QLatin1Char('=') << it.value().toString() << endl;
        ++it;
    }

    return true;
}

void DesktopFile::beginGroup(const QString &group)
{
    d->prefix = group;
}

void DesktopFile::endGroup()
{
    d->prefix = QString();
}

QString DesktopFile::group() const
{
    return d->prefix;
}

bool DesktopFile::contains(const QString &key) const
{
    const QString path = key.contains(QLatin1Char('/')) || d->prefix.isEmpty() ? key : QStringLiteral("%1/%2").arg(d->prefix, key);
    return d->items.contains(path);
}

bool DesktopFile::isVisible() const
{
    if (noDisplay() || isHidden())
        return false;

    const QString tryExecString = tryExec();
    if (!tryExecString.isEmpty() && d->checkTryExec(tryExecString))
        return false;

    return true;
}

bool DesktopFile::isSuitable(const QString &desktopEnvironment) const
{
    const QString env = desktopEnvironment.isEmpty()
            ? qEnvironmentVariable("XDG_CURRENT_DESKTOP").toUpper()
            : desktopEnvironment.toUpper();

    if (!env.isEmpty()) {
        const QStringList whitelist = onlyShowIn();
        const QStringList blacklist = notShowIn();

        if (!whitelist.isEmpty() && !whitelist.contains(env))
            return false;
        if (!blacklist.isEmpty() && blacklist.contains(env))
            return false;
    }

    return true;
}

QStringList DesktopFile::expandExecString(const QStringList &urls) const
{
    if (type() != ApplicationType)
        return QStringList();

    QStringList result;

    QString execStr = exec();
    unEscapeExec(execStr);

    const QStringList tokens = parseCombinedArgString(execStr);
    for (QString token : tokens) {
        // The parseCombinedArgString() splits the string by the space symbols,
        // we temporarily replaced them on the special characters.
        // Now we reverse it.
        token.replace(01, QLatin1Char(' '));
        token.replace(02, QLatin1Char('\t'));
        token.replace(03, QLatin1Char('\n'));

        // ----------------------------------------------------------
        // A single file name, even if multiple files are selected.
        if (token == QLatin1String("%f")) {
            if (!urls.isEmpty())
                result << expandEnvVariables(urls.at(0));
            continue;
        }

        // ----------------------------------------------------------
        // A list of files. Use for apps that can open several local files at once.
        // Each file is passed as a separate argument to the executable program.
        if (token == QLatin1String("%F")) {
            result << expandEnvVariables(urls);
            continue;
        }

        // ----------------------------------------------------------
        // A single URL. Local files may either be passed as file: URLs or as file path.
        if (token == QLatin1String("%u")) {
            if (!urls.isEmpty()) {
                QUrl url;
                url.setUrl(expandEnvVariables(urls.at(0)));
                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile()
                                                          : QString::fromUtf8(url.toEncoded()));
            }
            continue;
        }

        // ----------------------------------------------------------
        // A list of URLs. Each URL is passed as a separate argument to the executable
        // program. Local files may either be passed as file: URLs or as file path.
        if (token == QLatin1String("%U")) {
            for (const QString &s : urls) {
                QUrl url(expandEnvVariables(s));
                result << ((!url.toLocalFile().isEmpty()) ? url.toLocalFile()
                                                          : QString::fromUtf8(url.toEncoded()));
            }
            continue;
        }

        // ----------------------------------------------------------
        // The Icon key of the desktop entry expanded as two arguments, first --icon
        // and then the value of the Icon key. Should not expand to any arguments if
        // the Icon key is empty or missing.
        if (token == QLatin1String("%i")) {
            QString icon = value(iconKey).toString();
            if (!icon.isEmpty())
                result << QLatin1String("-icon")
                       << icon.replace(QLatin1Char('%'), QLatin1String("%%"));
            continue;
        }

        // ----------------------------------------------------------
        // The translated name of the application as listed in the appropriate Name key
        // in the desktop entry.
        if (token == QLatin1String("%c")) {
            result << localizedValue(nameKey).toString().replace(QLatin1Char('%'),
                                                                 QLatin1String("%%"));
            continue;
        }

        // ----------------------------------------------------------
        // The location of the desktop file as either a URI (if for example gotten from
        // the vfolder system) or a local filename or empty if no location is known.
        if (token == QLatin1String("%k")) {
            result << fileName().replace(QLatin1Char('%'), QLatin1String("%%"));
            break;
        }

        // ----------------------------------------------------------
        // Deprecated.
        // Deprecated field codes should be removed from the command line and ignored.
        if (token == QLatin1String("%d") || token == QLatin1String("%D")
                || token == QLatin1String("%n") || token == QLatin1String("%N")
                || token == QLatin1String("%v") || token == QLatin1String("%m")) {
            continue;
        }

        // ----------------------------------------------------------
        result << expandEnvVariables(token);
    }

    return result;
}

bool DesktopFile::startDetached(const QStringList &urls)
{
    switch (d->type) {
    case ApplicationType:
        return d->startApplicationDetached(this, QString(), urls);
    case LinkType:
        return d->startLinkDetached(this);
    default:
        break;
    }

    return false;
}

bool DesktopFile::startDetached(const QString &url)
{
    if (url.isEmpty())
        return startDetached(QStringList());
    return startDetached(QStringList(url));
}

QString DesktopFile::id(const QString &fileName)
{
    const QFileInfo info(fileName);

    QString id = info.absoluteFilePath();

    const QStringList dataDirs = XdgDirs::dataDirs();
    for (const auto &d : dataDirs) {
        if (id.startsWith(d)) {
            // Remove only the first occurence
            id.replace(id.indexOf(d), d.size(), QString());
        }
    }

    const QLatin1Char slash('/');
    const QString s = slash + applicationsStr + slash;
    if (!id.startsWith(s))
        return QString();

    id.replace(id.indexOf(s), s.size(), QString());
    id.replace(slash, QLatin1Char('-'));

    return id;
}

/*
 * DesktopAction
 */

DesktopFileAction::DesktopFileAction(const DesktopFile &parent, const QString &action)
    : DesktopFile(parent)
{
    beginGroup(QStringLiteral("Desktop Action %1").arg(action));
}

/*
 * DesktopFileCache
 */

DesktopFileCachePrivate::DesktopFileCachePrivate()
{
    initialize();
}

void DesktopFileCachePrivate::initialize()
{
    const QStringList locations =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                      applicationsStr,
                                      QStandardPaths::LocateDirectory);
    for (const auto &path : locations)
        initialize(path);
}

void DesktopFileCachePrivate::initialize(const QString &path)
{
    QDir dir(path);

    const QFileInfoList infos =
            dir.entryInfoList(QStringList(), QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    for (const auto &info : infos) {
        const QString absoluteFilePath = info.absoluteFilePath();

        // Recursively initialize directories
        if (info.isDir()) {
            initialize(absoluteFilePath);
            continue;
        }

        DesktopFile *file = load(absoluteFilePath);
        if (!file)
            continue;

        if (!cache.contains(absoluteFilePath))
            cache.insert(absoluteFilePath, file);

        const QStringList mimeTypes = file->mimeTypes();
        for (const auto &mime : mimeTypes) {
            int preference = file->value(initialPreferenceKey, 0).toInt();

            // We move the desktopFile forward in the list for this mime, so that
            // no desktopfile in front of it have a lower initialPreference
            int position = defaultAppsCache[mime].length();
            while (position > 0
                   && defaultAppsCache[mime][position - 1]->value(initialPreferenceKey, 0).toInt() < preference)
                position--;
            defaultAppsCache[mime].insert(position, file);
        }
    }
}

DesktopFile *DesktopFileCachePrivate::load(const QString &fileName)
{
    DesktopFile *desktopFile = new DesktopFile();
    Q_CHECK_PTR(desktopFile);

    if (desktopFile && desktopFile->load(fileName))
        return desktopFile;

    delete desktopFile;
    return nullptr;
}

DesktopFileCache::DesktopFileCache()
    : d_ptr(new DesktopFileCachePrivate())
{
}

DesktopFileCache::~DesktopFileCache()
{
    delete d_ptr;
}

DesktopFileCache *DesktopFileCache::instance()
{
    return s_desktopFileCache();
}

DesktopFile *DesktopFileCache::getFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return nullptr;

    DesktopFileCachePrivate *d = instance()->d_ptr;

    if (d->cache.contains(fileName))
        return d->cache.value(fileName);

    QString file;
    if (!fileName.startsWith(QDir::separator())) {
        // It's a relative path, search desktop file
        file = findDesktopFile(fileName);
        if (file.isEmpty())
            return nullptr;
    } else {
        file = fileName;
    }

    DesktopFile *desktopFile = nullptr;
    if (!d->cache.contains(file)) {
        // Cache it
        desktopFile = d->load(file);
        if (desktopFile) {
            d->cache.insert(file, desktopFile);
            return desktopFile;
        } else {
            return nullptr;
        }
    } else {
        // Already in the cache
        desktopFile = d->cache.value(file);
        return desktopFile;
    }
}

QList<DesktopFile *> DesktopFileCache::getApps(const QString &mimeType)
{
    return instance()->d_ptr->defaultAppsCache.value(mimeType);
}

DesktopFile *DesktopFileCache::getDefaultApp(const QString &mimeType)
{
    // First, we look in following places for a default in specified order:
    // ~/.config/mimeapps.list
    // /etc/xdg/mimeapps.list
    // ~/.local/share/applications/mimeapps.list
    // /usr/local/share/applications/mimeapps.list
    // /usr/share/applications/mimeapps.list
    QStringList mimeDirsList;

    mimeDirsList.append(XdgDirs::configHome(false));
    mimeDirsList.append(XdgDirs::configDirs());
    mimeDirsList.append(XdgDirs::dataHome(false) + QStringLiteral("/applications"));
    mimeDirsList.append(XdgDirs::dataDirs(QStringLiteral("/applications")));

    for (const QString &mimeDir : const_cast<const QStringList &>(mimeDirsList)) {
        QString defaultsListPath = mimeDir + QStringLiteral("/mimeapps.list");
        if (QFileInfo::exists(defaultsListPath)) {
            DesktopFile *defaultsFile = new DesktopFile();
            if (defaultsFile->load(defaultsListPath)) {
                defaultsFile->beginGroup(QStringLiteral("Default Applications"));
                if (defaultsFile->contains(mimeType)) {
                    QVariant value = defaultsFile->value(mimeType);
                    if (value.canConvert<QStringList>()) {
                        const QStringList values = value.toStringList();
                        for (const QString &desktopFileName : values) {
                            DesktopFile *desktopFile = DesktopFileCache::getFile(desktopFileName);
                            if (desktopFile)
                                return desktopFile;
                            else
                                qCWarning(lcXdg) << desktopFileName << "not a valid desktop file";
                        }
                    }
                }
                defaultsFile->endGroup();
            }
            delete defaultsFile;
        }
    }

    // If we havent found anything up to here, we look for a desktopfile that declares
    // the ability to handle the given mimetype. See getApps.
    QList<DesktopFile *> apps = getApps(mimeType);
    DesktopFile *desktopFile = apps.isEmpty() ? 0 : apps[0];
    return desktopFile;
}

} // namespace Liri
