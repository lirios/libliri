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

#include "backends/upowerpowerbackend_p.h"

#include <unistd.h>
#include <sys/reboot.h>

#define UPOWER_SERVICE QStringLiteral("org.freedesktop.UPower")
#define UPOWER_PATH QStringLiteral("/org/freedesktop/UPower")
#define UPOWER_OBJECT QStringLiteral("org.freedesktop.UPower")

namespace Liri {

UPowerPowerBackend::UPowerPowerBackend(QObject *parent)
    : LocalDevicePowerBackend(parent)
{
    m_interface = new QDBusInterface(UPOWER_SERVICE, UPOWER_PATH,
                                     UPOWER_OBJECT, QDBusConnection::systemBus());

    getUPowerProperty(QStringLiteral("SuspendAllowed"), &m_canSuspend);
    getUPowerProperty(QStringLiteral("HibernateAllowed"), &m_canHibernate);

    QDBusConnection::systemBus().connect(UPOWER_SERVICE, UPOWER_PATH,
                                         QStringLiteral("org.freedesktop.DBus.Properties"),
                                         QStringLiteral("PropertiesChanged"),
                                         this, SLOT(upowerChanged(QString,QVariantMap,QStringList)));
}

UPowerPowerBackend::~UPowerPowerBackend()
{
    m_interface->deleteLater();
}

QString UPowerPowerBackend::service()
{
    return UPOWER_SERVICE;
}

QString UPowerPowerBackend::name() const
{
    return QStringLiteral("upower");
}

bool UPowerPowerBackend::canPowerOff() const
{
    return m_canPowerOff;
}

bool UPowerPowerBackend::canRestart() const
{
    return m_canRestart;
}

bool UPowerPowerBackend::canSuspend() const
{
    return m_canSuspend;
}

bool UPowerPowerBackend::canHibernate() const
{
    return m_canHibernate;
}

bool UPowerPowerBackend::canHybridSleep() const
{
    return m_canHybridSleep;
}

void UPowerPowerBackend::powerOff()
{
    sync();
    ::reboot(RB_POWER_OFF);
}

void UPowerPowerBackend::restart()
{
    sync();
    ::reboot(RB_AUTOBOOT);
}

void UPowerPowerBackend::suspend()
{
    if (m_canSuspend)
        m_interface->asyncCall(QStringLiteral("Suspend"));
}

void UPowerPowerBackend::hibernate()
{
    if (m_canHibernate)
        m_interface->asyncCall(QStringLiteral("Hibernate"));
}

void UPowerPowerBackend::hybridSleep()
{
}

void UPowerPowerBackend::getUPowerProperty(const QString &name, bool *prop)
{
    if (!prop)
        return;

    QDBusPendingCall pendingCall = m_interface->asyncCall(name);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid())
            *prop = reply.value() == QStringLiteral("yes");
        watcher->deleteLater();
    });
}

void UPowerPowerBackend::upowerChanged(const QString &interface, QVariantMap &properties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interface);
    Q_UNUSED(invalidatedProperties);

    if (properties.contains(QStringLiteral("CanSuspend"))) {
        m_canSuspend = properties[QStringLiteral("CanSuspend")].toBool();
        Q_EMIT canSuspendChanged();
    }

    if (properties.contains(QStringLiteral("CanHibernate"))) {
        m_canHibernate = properties[QStringLiteral("CanHibernate")].toBool();
        Q_EMIT canHibernateChanged();
    }
}

bool UPowerPowerBackend::check()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(UPOWER_SERVICE);
}

} // namespace Liri
