/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "backends/systemdbackend_p.h"

#define LOGIN1_SERVICE QStringLiteral("org.freedesktop.login1")
#define LOGIN1_PATH QStringLiteral("/org/freedesktop/login1")
#define LOGIN1_INTERFACE QStringLiteral("org.freedesktop.login1.Manager")

#define SYSTEMD1_SERVICE QStringLiteral("org.freedesktop.systemd1")
#define SYSTEMD1_PATH QStringLiteral("/org/freedesktop/systemd1")
#define SYSTEMD1_INTERFACE QStringLiteral("org.freedestkop.systemd1.Manager")

#define HOSTNAME1_SERVICE QStringLiteral("org.freedesktop.hostname1")
#define HOSTNAME1_PATH QStringLiteral("/org/freedesktop/hostname1")

#define DBUS_PROPERTIES_INTERFACE QStringLiteral("org.freedesktop.DBus.Properties")

Q_LOGGING_CATEGORY(lcSystemdBackend, "liri.localdevice.systemd")

namespace Liri {

SystemdBackend::SystemdBackend(QObject *parent)
    : LocalDeviceBackend(parent)
{
    getSystemd1Property(QStringLiteral("Virtualization"));
    getHostname1Property(QStringLiteral("Chassis"));
    getHostname1Property(QStringLiteral("Hostname"));
    getHostname1Property(QStringLiteral("IconName"));
    getHostname1Property(QStringLiteral("OperatingSystemPrettyName"));
    getLogin1Property(QStringLiteral("CanPowerOff"));
    getLogin1Property(QStringLiteral("CanReboot"));
    getLogin1Property(QStringLiteral("CanSuspend"));
    getLogin1Property(QStringLiteral("CanHibernate"));
    getLogin1Property(QStringLiteral("CanHybridSleep"));
}

QString SystemdBackend::service()
{
    return LOGIN1_SERVICE;
}

QString SystemdBackend::name() const
{
    return QStringLiteral("systemd");
}

LocalDevice::Chassis SystemdBackend::chassis() const
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

QString SystemdBackend::hostName() const
{
    return m_hostName;
}

QString SystemdBackend::iconName() const
{
    return m_iconName;
}

QString SystemdBackend::operatingSystemName() const
{
    return m_osName;
}

QString SystemdBackend::virtualization() const
{
    return m_virtualization;
}

bool SystemdBackend::canPowerOff() const
{
    return m_canPowerOff;
}

bool SystemdBackend::canRestart() const
{
    return m_canRestart;
}

bool SystemdBackend::canSuspend() const
{
    return m_canSuspend;
}

bool SystemdBackend::canHibernate() const
{
    return m_canHibernate;
}

bool SystemdBackend::canHybridSleep() const
{
    return m_canHybridSleep;
}

void SystemdBackend::powerOff()
{
    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, QStringLiteral("PowerOff"));
    msg << true;
    QDBusConnection::systemBus().asyncCall(msg);
}

void SystemdBackend::restart()
{
    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, QStringLiteral("Reboot"));
    msg << true;
    QDBusConnection::systemBus().asyncCall(msg);
}

void SystemdBackend::suspend()
{
    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, QStringLiteral("Suspend"));
    msg << true;
    QDBusConnection::systemBus().asyncCall(msg);
}

void SystemdBackend::hibernate()
{
    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, QStringLiteral("Hibernate"));
    msg << true;
    QDBusConnection::systemBus().asyncCall(msg);
}

void SystemdBackend::hybridSleep()
{
    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, QStringLiteral("HybridSleep"));
    msg << true;
    QDBusConnection::systemBus().asyncCall(msg);
}

bool SystemdBackend::check()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(LOGIN1_SERVICE);
}

void SystemdBackend::getLogin1Property(const QString &name)
{
    QStringList validValues;
    validValues << QStringLiteral("yes") << QStringLiteral("challenge");

    auto msg = QDBusMessage::createMethodCall(LOGIN1_SERVICE, LOGIN1_PATH,
                                              LOGIN1_INTERFACE, name);
    auto reply = QDBusConnection::systemBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid()) {
            if (name == QStringLiteral("CanPowerOff")) {
                m_canPowerOff = validValues.contains(reply.value());
                Q_EMIT canPowerOffChanged();
            } else if (name == QStringLiteral("CanReboot")) {
                m_canRestart = validValues.contains(reply.value());
                Q_EMIT canRestartChanged();
            } else if (name == QStringLiteral("CanSuspend")) {
                m_canSuspend = validValues.contains(reply.value());
                Q_EMIT canSuspendChanged();
            } else if (name == QStringLiteral("CanHibernate")) {
                m_canHibernate = validValues.contains(reply.value());
                Q_EMIT canHibernateChanged();
            } else if (name == QStringLiteral("CanHybridSleep")) {
                m_canHybridSleep = validValues.contains(reply.value());
                Q_EMIT canHybridSleepdChanged();
            }
        } else {
            qCWarning(lcSystemdBackend,
                      "Failed to call method \"%s\" on D-Bus interface \"%s\": %s",
                      qPrintable(name), qPrintable(LOGIN1_INTERFACE),
                      qPrintable(reply.error().message()));
        }
        watcher->deleteLater();
    });
}

void SystemdBackend::getSystemd1Property(const QString &name)
{
    auto msg = QDBusMessage::createMethodCall(SYSTEMD1_SERVICE, SYSTEMD1_PATH,
                                              DBUS_PROPERTIES_INTERFACE,
                                              QStringLiteral("Get"));
    msg << SYSTEMD1_INTERFACE << name;

    auto reply = QDBusConnection::systemBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QString> reply = *watcher;
        if (reply.isValid()) {
            if (name == QStringLiteral("Virtualization"))
                m_virtualization = reply.value();
        } else {
            qCWarning(lcSystemdBackend,
                      "Failed to get property \"%s\" for \"%s\": %s",
                      qPrintable(name), qPrintable(SYSTEMD1_INTERFACE),
                      qPrintable(reply.error().message()));
        }
        watcher->deleteLater();
    });
}

void SystemdBackend::getHostname1Property(const QString &name)
{
    auto msg = QDBusMessage::createMethodCall(HOSTNAME1_SERVICE, HOSTNAME1_PATH,
                                              DBUS_PROPERTIES_INTERFACE,
                                              QStringLiteral("Get"));
    msg << HOSTNAME1_SERVICE << name;

    auto reply = QDBusConnection::systemBus().asyncCall(msg);
    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        QDBusPendingReply<QVariant> reply = *watcher;
        if (reply.isValid()) {
            if (name == QStringLiteral("Chassis"))
                m_chassis = reply.value().toString();
            else if (name == QStringLiteral("Hostname"))
                m_hostName = reply.value().toString();
            else if (name == QStringLiteral("IconName"))
                m_iconName = reply.value().toString();
            else if (name == QStringLiteral("OperatingSystemPrettyName"))
                m_osName = reply.value().toString();
        } else {
            qCWarning(lcSystemdBackend,
                      "Failed to get property \"%s\" for \"%s\": %s",
                      qPrintable(name), qPrintable(HOSTNAME1_SERVICE),
                      qPrintable(reply.error().message()));
        }
        watcher->deleteLater();
    });
}

} // namespace Liri
