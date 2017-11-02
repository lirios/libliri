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

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>

#include "backends/systemdsystembackend_p.h"

#define SYSTEMD1_SERVICE QStringLiteral("org.freedesktop.systemd1")
#define SYSTEMD1_PATH QStringLiteral("/org/freedesktop/systemd1")
#define SYSTEMD1_INTERFACE QStringLiteral("org.freedestkop.systemd1.Manager")

#define HOSTNAME1_SERVICE QStringLiteral("org.freedesktop.hostname1")
#define HOSTNAME1_PATH QStringLiteral("/org/freedesktop/hostname1")

#define DBUS_PROPERTIES_INTERFACE QStringLiteral("org.freedesktop.DBus.Properties")

namespace Liri {

SystemdSystemBackend::SystemdSystemBackend(QObject *parent)
    : LocalDeviceSystemBackend(parent)
{
    getSystemd1Property(QStringLiteral("Virtualization"), &m_virtualization);
    getHostname1Property(QStringLiteral("Chassis"), &m_chassis);
    getHostname1Property(QStringLiteral("Hostname"), &m_hostName);
    getHostname1Property(QStringLiteral("IconName"), &m_iconName);
    getHostname1Property(QStringLiteral("OperatingSystemPrettyName"), &m_osName);
}

LocalDevice::Chassis SystemdSystemBackend::chassis() const
{
    if (m_chassis == QStringLiteral("desktop") || m_chassis.isEmpty())
        return LocalDevice::DesktopChassis;
    else if (m_chassis == QStringLiteral("laptop"))
        return LocalDevice::LaptopChassis;
    else if (m_chassis == QStringLiteral("server"))
        return LocalDevice::ServerChassis;
    else if (m_chassis == QStringLiteral("tablet"))
        return LocalDevice::TabletChassis;
    else if (m_chassis == QStringLiteral("handset"))
        return LocalDevice::PhoneChassis;
    else if (m_chassis == QStringLiteral("vm"))
        return LocalDevice::VirtualMachineChassis;
    else if (m_chassis == QStringLiteral("container"))
        return LocalDevice::ContainerChassis;
    return LocalDevice::UnknownChassis;
}

QString SystemdSystemBackend::hostName() const
{
    return m_hostName;
}

QString SystemdSystemBackend::iconName() const
{
    return m_iconName;
}

QString SystemdSystemBackend::materialIconName() const
{
    switch (chassis()) {
    case LocalDevice::UnknownChassis:
    case LocalDevice::DesktopChassis:
    case LocalDevice::ServerChassis:
    case LocalDevice::VirtualMachineChassis:
    case LocalDevice::ContainerChassis:
        return QStringLiteral("hardware/computer");
    case LocalDevice::LaptopChassis:
        return QStringLiteral("hardware/laptop");
    case LocalDevice::TabletChassis:
        return QStringLiteral("hardware/tablet");
    case LocalDevice::PhoneChassis:
        return QStringLiteral("hardware/smartphone");
    }
}

QString SystemdSystemBackend::operatingSystemName() const
{
    return m_osName;
}

QString SystemdSystemBackend::virtualization() const
{
    return m_virtualization;
}

bool SystemdSystemBackend::check()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(SYSTEMD1_SERVICE);
}

void SystemdSystemBackend::getSystemd1Property(const QString &name, QString *prop)
{
    if (!prop)
        return;

    auto msg = QDBusMessage::createMethodCall(SYSTEMD1_SERVICE, SYSTEMD1_PATH,
                                              DBUS_PROPERTIES_INTERFACE,
                                              QStringLiteral("Get"));
    msg << SYSTEMD1_INTERFACE << name;

    auto reply = QDBusConnection::systemBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid())
            *prop = reply.value();
        watcher->deleteLater();
    });
}

void SystemdSystemBackend::getHostname1Property(const QString &name, QString *prop)
{
    if (!prop)
        return;

    auto msg = QDBusMessage::createMethodCall(HOSTNAME1_SERVICE, HOSTNAME1_PATH,
                                              DBUS_PROPERTIES_INTERFACE,
                                              QStringLiteral("Get"));
    msg << HOSTNAME1_SERVICE << name;

    auto reply = QDBusConnection::systemBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid())
            *prop = reply.value();
        watcher->deleteLater();
    });
}

} // namespace Liri
