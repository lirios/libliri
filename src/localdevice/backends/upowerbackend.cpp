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

#include "backends/upowerbackend_p.h"

#include <unistd.h>
#include <sys/reboot.h>

#define UPOWER_SERVICE QStringLiteral("org.freedesktop.UPower")
#define UPOWER_PATH QStringLiteral("/org/freedesktop/UPower")
#define UPOWER_OBJECT QStringLiteral("org.freedesktop.UPower")

#define DBUS_PROPERTIES_INTERFACE QStringLiteral("org.freedesktop.DBus.Properties")

namespace Liri {

UPowerBackend::UPowerBackend(QObject *parent)
    : LocalDeviceBackend(parent)
{
    getUPowerProperty(QStringLiteral("OnBattery"));
    getUPowerProperty(QStringLiteral("LidIsPresent"));
    getUPowerProperty(QStringLiteral("LidIsClosed"));

    QDBusConnection::systemBus().connect(UPOWER_SERVICE, UPOWER_PATH,
                                         DBUS_PROPERTIES_INTERFACE,
                                         QStringLiteral("PropertiesChanged"),
                                         this, SLOT(upowerChanged(QString,QVariantMap,QStringList)));
}

QString UPowerBackend::service()
{
    return UPOWER_SERVICE;
}

QString UPowerBackend::name() const
{
    return QStringLiteral("upower");
}

LocalDevice::PowerSource UPowerBackend::powerSource() const
{
    return m_onBattery ? LocalDevice::BatteryPower : LocalDevice::ACPower;
}

bool UPowerBackend::lidPresent() const
{
    return m_lidPresent;
}

bool UPowerBackend::lidClosed() const
{
    return m_lidClosed;
}

void UPowerBackend::powerOff()
{
    sync();
    ::reboot(RB_POWER_OFF);
}

void UPowerBackend::restart()
{
    sync();
    ::reboot(RB_AUTOBOOT);
}

void UPowerBackend::suspend()
{
    auto message = QDBusMessage::createMethodCall(
                UPOWER_SERVICE,
                UPOWER_PATH,
                UPOWER_OBJECT,
                QStringLiteral("Suspend"));
    QDBusConnection::systemBus().asyncCall(message);
}

void UPowerBackend::hibernate()
{
    auto message = QDBusMessage::createMethodCall(
                UPOWER_SERVICE,
                UPOWER_PATH,
                UPOWER_OBJECT,
                QStringLiteral("Hibernate"));
    QDBusConnection::systemBus().asyncCall(message);
}

void UPowerBackend::getUPowerProperty(const QString &name)
{
    auto message = QDBusMessage::createMethodCall(
                UPOWER_SERVICE,
                UPOWER_PATH,
                DBUS_PROPERTIES_INTERFACE,
                QStringLiteral("Get"));
    message << QStringLiteral("org.freedesktop.UPower") << name;
    QDBusPendingReply<QVariant> reply = QDBusConnection::systemBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=]() {
        watcher->deleteLater();
        if (!reply.isValid())
            return;

        if (name == QStringLiteral("OnBattery")) {
            m_onBattery = reply.value().toBool();
            Q_EMIT powerSourceChanged();
        } else if (name == QStringLiteral("LidIsPresent")) {
            m_lidPresent = reply.value().toBool();
            Q_EMIT lidPresentChanged();
        } else if (name == QStringLiteral("LidIsClosed")) {
            m_lidClosed = reply.value().toBool();
            Q_EMIT lidClosedChanged();
        }
    });
}

void UPowerBackend::upowerChanged(const QString &interface, QVariantMap &properties, const QStringList &invalidatedProperties)
{
    Q_UNUSED(interface);
    Q_UNUSED(invalidatedProperties);

    if (properties.contains(QStringLiteral("OnBattery"))) {
        m_onBattery = properties[QStringLiteral("OnBattery")].toBool();
        Q_EMIT powerSourceChanged();
    }

    if (properties.contains(QStringLiteral("LidIsPresent"))) {
        m_lidPresent = properties[QStringLiteral("LidIsPresent")].toBool();
        Q_EMIT lidPresentChanged();
    }

    if (properties.contains(QStringLiteral("LidIsClosed"))) {
        m_lidClosed = properties[QStringLiteral("LidIsClosed")].toBool();
        Q_EMIT lidClosedChanged();
    }
}

bool UPowerBackend::check()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(UPOWER_SERVICE);
}

} // namespace Liri
