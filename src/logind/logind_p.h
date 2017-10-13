/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#pragma once

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

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QLoggingCategory>
#include <QVector>

Q_DECLARE_LOGGING_CATEGORY(lcLogind)

namespace Liri {

class Logind;

class LogindPrivate
{
    Q_DECLARE_PUBLIC(Logind)
public:
    explicit LogindPrivate(Logind *qq);

    void _q_serviceRegistered();
    void _q_serviceUnregistered();
    void _q_sessionPropertiesChanged();

    void checkServiceRegistration();

    QDBusConnection bus;
    QDBusServiceWatcher *watcher = nullptr;
    bool isConnected = false;
    bool hasSessionControl = false;
    QString sessionPath;
    bool sessionActive = false;
    int vt = -1;
    QVector<int> inhibitFds;

protected:
    Logind *q_ptr;

private:
    void getSessionActive();
    void getVirtualTerminal();
};

} // namespace Liri
