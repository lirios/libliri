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

#ifndef LIRI_DESKTOPMENU_P_H
#define LIRI_DESKTOPMENU_P_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QStringList>
#include <QTimer>

#include "desktopmenu.h"

#define REBUILD_DELAY 3000

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

class DesktopMenuPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(DesktopMenu)
public:
    explicit DesktopMenuPrivate(DesktopMenu *parent);

    void simplify(QDomElement &element);
    void mergeMenus(QDomElement &element);
    void moveMenus(QDomElement &element);
    void deleteDeletedMenus(QDomElement &element);
    void processDirectoryEntries(QDomElement &element, const QStringList &parentDirs);
    void processApps(QDomElement &element);
    void deleteEmpty(QDomElement &element);
    void processLayouts(QDomElement &element);
    void fixSeparators(QDomElement &element);

    bool loadDirectoryFile(const QString &fileName, QDomElement &element);
    void prependChilds(QDomElement &srcElement, QDomElement &destElement);
    void appendChilds(QDomElement &srcElement, QDomElement &destElement);

    void saveLog(const QString &logFileName);
    void load(const QString &fileName);

    void clearWatcher();

    QString mErrorString;
    QStringList mEnvironments;
    QString mMenuFileName;
    QString mLogDir;
    QDomDocument mXml;
    QByteArray mHash;
    QTimer mRebuildDelayTimer;

    QFileSystemWatcher mWatcher;
    bool mOutDated;

public Q_SLOTS:
    void rebuild();

Q_SIGNALS:
    void changed();

private:
    DesktopMenu *const q_ptr;
};

} // namespace Liri

#endif // LIRI_DESKTOPMENU_P_H
