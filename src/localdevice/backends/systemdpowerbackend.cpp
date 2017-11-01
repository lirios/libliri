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

#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>

#include "backends/systemdpowerbackend_p.h"

#define LOGIN1_SERVICE QStringLiteral("org.freedesktop.login1")
#define LOGIN1_PATH QStringLiteral("/org/freedesktop/login1")
#define LOGIN1_OBJECT QStringLiteral("org.freedesktop.login1.Manager")

namespace Liri {

SystemdPowerBackend::SystemdPowerBackend(QObject *parent)
    : LocalDevicePowerBackend(parent)
{
    m_interface = new QDBusInterface(LOGIN1_SERVICE, LOGIN1_PATH,
                                     LOGIN1_OBJECT, QDBusConnection::systemBus());

    getLogin1Property(QStringLiteral("CanPowerOff"), &m_canPowerOff);
    getLogin1Property(QStringLiteral("CanReboot"), &m_canRestart);
    getLogin1Property(QStringLiteral("CanSuspend"), &m_canSuspend);
    getLogin1Property(QStringLiteral("CanHibernate"), &m_canHibernate);
    getLogin1Property(QStringLiteral("CanHybridSleep"), &m_canHybridSleep);
}

SystemdPowerBackend::~SystemdPowerBackend()
{
    m_interface->deleteLater();
}

QString SystemdPowerBackend::service()
{
    return LOGIN1_SERVICE;
}

QString SystemdPowerBackend::name() const
{
    return QStringLiteral("systemd");
}

bool SystemdPowerBackend::canPowerOff() const
{
    return m_canPowerOff;
}

bool SystemdPowerBackend::canRestart() const
{
    return m_canRestart;
}

bool SystemdPowerBackend::canSuspend() const
{
    return m_canSuspend;
}

bool SystemdPowerBackend::canHibernate() const
{
    return m_canHibernate;
}

bool SystemdPowerBackend::canHybridSleep() const
{
    return m_canHybridSleep;
}

void SystemdPowerBackend::powerOff()
{
    if (m_canPowerOff)
        m_interface->asyncCall(QStringLiteral("PowerOff"), true);
}

void SystemdPowerBackend::restart()
{
    if (m_canRestart)
        m_interface->asyncCall(QStringLiteral("Reboot"), true);
}

void SystemdPowerBackend::suspend()
{
    if (m_canSuspend)
        m_interface->asyncCall(QStringLiteral("Suspend"), true);
}

void SystemdPowerBackend::hibernate()
{
    if (m_canHibernate)
        m_interface->asyncCall(QStringLiteral("Hibernate"), true);
}

void SystemdPowerBackend::hybridSleep()
{
    if (m_canHybridSleep)
        m_interface->asyncCall(QStringLiteral("HybridSleep"), true);
}

bool SystemdPowerBackend::check()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(LOGIN1_SERVICE);
}

void SystemdPowerBackend::getLogin1Property(const QString &name, bool *prop)
{
    if (!prop)
        return;

    QStringList validValues;
    validValues << QStringLiteral("yes") << QStringLiteral("challenge");

    QDBusPendingCall pendingCall = m_interface->asyncCall(name);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid()) {
            *prop = validValues.contains(reply.value());

            if (name == QStringLiteral("CanPowerOff"))
                Q_EMIT canPowerOffChanged();
            else if (name == QStringLiteral("CanReboot"))
                Q_EMIT canRestartChanged();
            else if (name == QStringLiteral("CanSuspend"))
                Q_EMIT canSuspendChanged();
            else if (name == QStringLiteral("CanHibernate"))
                Q_EMIT canHibernateChanged();
            else if (name == QStringLiteral("CanHybridSleep"))
                Q_EMIT canHybridSleepdChanged();
        }
        watcher->deleteLater();
    });
}

} // namespace Liri
