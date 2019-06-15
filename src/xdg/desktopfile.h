/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LIRI_DESKTOPFILE_H
#define LIRI_DESKTOPFILE_H

#include <QProcess>
#include <QSharedDataPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QUrl>

#include <LiriXdg/lirixdgglobal.h>

namespace Liri {

class DesktopFilePrivate;
class DesktopFileCachePrivate;
class DesktopFileAction;

class LIRIXDG_EXPORT DesktopFile
{
public:
    enum Type {
        UnknownType,
        ApplicationType,
        LinkType,
        DirectoryType,
    };

    explicit DesktopFile(const QString &fileName = QString());
    DesktopFile(const DesktopFile &other);
    virtual ~DesktopFile();

    DesktopFile &operator=(const DesktopFile &other);
    bool operator==(const DesktopFile &other) const;
    inline bool operator!=(const DesktopFile &other) const { return !operator==(other); }

    QString fileName() const;

    bool isValid() const;

    Type type() const;
    QString version() const;
    QString name() const;
    QString genericName() const;
    QString comment() const;
    QStringList keywords() const;
    QString iconName() const;

    bool noDisplay() const;
    bool isHidden() const;

    QStringList onlyShowIn() const;
    QStringList notShowIn() const;

    bool isDBusActivatable() const;

    QString tryExec() const;
    QString exec() const;

    QString path() const;

    QUrl url() const;

    bool runsOnTerminal() const;
    bool startupNotify() const;

    QStringList actionNames() const;
    DesktopFileAction action(const QString &name) const;

    QStringList mimeTypes() const;
    QStringList categories() const;
    QStringList implements() const;

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);

    QVariant localizedValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setLocalizedValue(const QString &key, const QVariant &value);

    bool load(const QString &fileName);
    bool save(const QString &fileName);

    void beginGroup(const QString &group);
    void endGroup();
    QString group() const;

    bool contains(const QString &key) const;

    bool isVisible() const;
    bool isSuitable(const QString &desktopEnvironment = QString()) const;

    QStringList expandExecString(const QStringList &urls = QStringList()) const;

    void setProcessEnvironment(const QProcessEnvironment &env);

    bool startDetached(const QStringList &urls);
    bool startDetached(const QString &url = QString());

    static QString id(const QString &fileName);

protected:
    QSharedDataPointer<DesktopFilePrivate> d;
};

typedef QList<DesktopFile> DesktopFileList;

class LIRIXDG_EXPORT DesktopFileAction : public DesktopFile
{
private:
    friend class DesktopFile;

    explicit DesktopFileAction(const DesktopFile &parent, const QString &action);
};

class LIRIXDG_EXPORT DesktopFileCache
{
public:
    explicit DesktopFileCache();
    ~DesktopFileCache();

    static DesktopFileCache *instance();

    static DesktopFile *getFile(const QString &fileName);
    static QList<DesktopFile *> getApps(const QString &mimeType);
    static DesktopFile *getDefaultApp(const QString &mimeType);

private:
    DesktopFileCachePrivate *const d_ptr;
};

} // namespace Liri

#endif // LIRI_DESKTOPFILE_H
