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

#ifndef LIRI_DESKTOPFILE_P_H
#define LIRI_DESKTOPFILE_P_H

#include "desktopfile.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace Liri {

class DesktopFilePrivate : public QSharedData
{
public:
    explicit DesktopFilePrivate() {}

    void clear();

    bool readFile();

    DesktopFile::Type detectType(DesktopFile *q) const;

    bool checkTryExec(const QString &progName) const;

    bool contains(const QString &key) const;

    QString localizedKey(const QString &key) const;

    bool startApplicationDetached(DesktopFile *q, const QString &actionName,
                                  const QStringList &urls);
    bool startLinkDetached(DesktopFile *q);
    bool startByDBus(const QString &action, const QStringList &urls);

    QString fileName;
    QString prefix;
    QMap<QString, QVariant> items;
    DesktopFile::Type type = DesktopFile::UnknownType;
};

class DesktopFileCachePrivate
{
public:
    explicit DesktopFileCachePrivate();

    void initialize();
    void initialize(const QString &path);

    DesktopFile *load(const QString &fileName);

    QHash<QString, DesktopFile *> cache;
    QHash<QString, QList<DesktopFile *>> defaultAppsCache;
};

} // namespace Liri

#endif // LIRI_DESKTOPFILE_P_H
