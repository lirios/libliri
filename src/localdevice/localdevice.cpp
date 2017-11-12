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

#include <QSysInfo>

#include "localdevice.h"
#include "localdevice_p.h"
#include "backends/systemdbackend_p.h"
#include "backends/upowerbackend_p.h"

namespace Liri {

LocalDevicePrivate::LocalDevicePrivate()
{
    if (SystemdBackend::check())
        systemd = new SystemdBackend();
    if (UPowerBackend::check())
        upower = new UPowerBackend();
    if (!systemd && !upower)
        qWarning("No backend available");
}

LocalDevicePrivate::~LocalDevicePrivate()
{
    if (systemd) {
        systemd->deleteLater();
        systemd = nullptr;
    }

    if (upower) {
        upower->deleteLater();
        upower = nullptr;
    }
}

/*!
 * \qmlmodule Liri.Device
 * \title Liri: Local Device
 * \ingroup libliri
 * \brief A collection of local device related utility functions.
 *
 * Provides utility functions for controlling a device, such as shutdown,
 * reboot or getting informations like chassis type.
 *
 * Import the module as follows:
 *
 * \badcode
 * import Liri.Device 1.0 as LiriDevice
 * \endcode
 *
 * \note Some functions may not be available on all platforms.
 */

/*!
 * \qmltype LocalDevice
 * \inqmlmodule Liri.LocalDevice
 * \brief Singleton type providing access to local device utilities.
 *
 * LocalDevice is an interface to various utility functions related to
 * the local device.
 *
 * \code
 * import Liri.Device 1.0 as LiriDevice
 *
 * Text {
 *     text: LiriDevice.LocalDevice.hostName
 * }
 * \endcode
 */

/*!
 * \class LocalDevice
 * \inmodule LiriLocalDevice
 * \brief Singleton type providing access to local device utilities.
 *
 * LocalDevice is an interface to various utility functions related to
 * the local device.
 */

/*!
 * Constructs a LocalDevice with the given \a parent.
 */
LocalDevice::LocalDevice(QObject *parent)
    : QObject(parent)
    , d_ptr(new LocalDevicePrivate())
{
    if (d_func()->systemd) {
        connect(d_func()->systemd, &LocalDeviceBackend::canPowerOffChanged, this, &LocalDevice::canPowerOffChanged);
        connect(d_func()->systemd, &LocalDeviceBackend::canRestartChanged, this, &LocalDevice::canRestartChanged);
        connect(d_func()->systemd, &LocalDeviceBackend::canSuspendChanged, this, &LocalDevice::canSuspendChanged);
        connect(d_func()->systemd, &LocalDeviceBackend::canHibernateChanged, this, &LocalDevice::canHibernateChanged);
        connect(d_func()->systemd, &LocalDeviceBackend::canHybridSleepdChanged, this, &LocalDevice::canHybridSleepdChanged);
    }

    if (d_func()->upower) {
        connect(d_func()->upower, &LocalDeviceBackend::powerSourceChanged, this, &LocalDevice::powerSourceChanged);
        connect(d_func()->upower, &LocalDeviceBackend::lidPresentChanged, this, &LocalDevice::lidPresentChanged);
        connect(d_func()->upower, &LocalDeviceBackend::lidClosedChanged, this, &LocalDevice::lidClosedChanged);
    }
}

/*!
 * Destroys a LocalDevice object.
 */
LocalDevice::~LocalDevice()
{
    delete d_ptr;
}

/*!
 * \enum LocalDevice::Chassis
 *
 * This enum type is used to specify the device chassis type.
 *
 * \value UnknownChassis The chassis type is not known.
 * \value DesktopChassis The device is a desktop computer.
 * \value LaptopChassis The device is a laptop computer.
 * \value ServerChassis The device is a server computer.
 * \value TabletChassis The device is a tablet.
 * \value PhoneChassis The device is a phone.
 * \value VirtualMachineChassis The device is a virtual machine.
 * \value ContainerChassis The device is a container.
 *
 * \sa LocalDevice::chassis
 */

/*!
 * \qmlproperty Liri::LocalDevice::LocalDevice::Chassis Liri::LocalDevice::LocalDevice::chassis
 *
 * This property holds the device chassis type.
 * The default value is LocalDevice.UnknownChassis.
 */

/*!
 * \property LocalDevice::chassis
 *
 * This property holds the device chassis type.
 * The default value is LocalDevice::UnknownChassis.
 */
LocalDevice::Chassis LocalDevice::chassis() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->chassis();

    return LocalDevice::UnknownChassis;
}

/*!
 * \enum LocalDevice::PowerSource
 *
 * This enum type is used to specify the power source of this device.
 *
 * \value ACPower AC power.
 * \value BatteryPower Battery power.
 *
 * \sa LocalDevice::powerSource
 */

/*!
 * \qmlproperty Liri::LocalDevice::LocalDevice::PowerSource Liri::LocalDevice::LocalDevice::powerSource
 *
 * This property holds whether the device is on AC power or battery power.
 */

/*!
 * \property LocalDevice::powerSource
 *
 * This property holds whether the device is on AC power or battery power.
 */
LocalDevice::PowerSource LocalDevice::powerSource() const
{
    Q_D(const LocalDevice);

    if (d->upower)
        return d->upower->powerSource();

    return LocalDevice::ACPower;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::lidPresent
 *
 * This property holds whether the device has a lid or not.
 */

/*!
 * \property LocalDevice::lidPresent
 *
 * This property holds whether the device has a lid or not.
 */
bool LocalDevice::lidPresent() const
{
    Q_D(const LocalDevice);

    if (d->upower)
        return d->upower->lidPresent();

    return false;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::lidClosed
 *
 * This property holds whether the device has the lid closed.
 */

/*!
 * \property LocalDevice::lidClosed
 *
 * This property holds whether the device has the lid closed.
 */
bool LocalDevice::lidClosed() const
{
    Q_D(const LocalDevice);

    if (d->upower)
        return d->upower->lidClosed();

    return false;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::hostName
 *
 * This property holds the system host name.
 */

/*!
 * \property LocalDevice::hostName
 *
 * This property holds the system host name.
 */
QString LocalDevice::hostName() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->hostName();

    return QStringLiteral("localhost");
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::iconName
 *
 * This property holds the name of the freedesktop.org icon representing
 * the system's form factor (e.g. "computer-laptop").
 *
 * \sa LocalDevice::chassis
 */

/*!
 * \property LocalDevice::materialIconName
 *
 * This property holds the name of the freedesktop.org icon representing
 * the system's form factor (e.g. "computer-laptop").
 *
 * \sa LocalDevice::chassis
 */
QString LocalDevice::iconName() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->iconName();

    return QStringLiteral("computer");
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::materialIconName
 *
 * This property holds the name of the Material Design icon representing
 * the system's form factor (e.g. "hardware/laptop").
 *
 * \sa LocalDevice::chassis
 */

/*!
 * \property LocalDevice::materialIconName
 *
 * This property holds the name of the Material Design icon representing
 * the system's form factor (e.g. "hardware/laptop").
 *
 * \sa LocalDevice::chassis
 */
QString LocalDevice::materialIconName() const
{
    switch (chassis()) {
    case LocalDevice::LaptopChassis:
        return QStringLiteral("hardware/laptop");
    case LocalDevice::TabletChassis:
        return QStringLiteral("hardware/tablet");
    case LocalDevice::PhoneChassis:
        return QStringLiteral("hardware/smartphone");
    default:
        break;
    }

    return QStringLiteral("hardware/computer");
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::operatingSystemName
 *
 * This property holds the operating system name.
 */

/*!
 * \property LocalDevice::operatingSystemName
 *
 * This property holds the operating system name.
 */
QString LocalDevice::operatingSystemName() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->operatingSystemName();

    return QSysInfo::prettyProductName();
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::virtualization
 *
 * This property holds the virtualization technology used, if applicable.
 * If the device is a bare-metal machine this property will be empty.
 */

/*!
 * \property LocalDevice::virtualization
 *
 * This property holds the virtualization technology used, if applicable.
 * If the device is a bare-metal machine this property will be empty.
 */
QString LocalDevice::virtualization() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->virtualization();

    return QString();
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::canPowerOff
 *
 * This property holds whether the current user can power off the system.
 *
 * \sa Liri::LocalDevice::LocalDevice::powerOff()
 */

/*!
 * \property LocalDevice::canPowerOff
 *
 * This property holds whether the current user can power off the system.
 *
 * \sa LocalDevice::powerOff()
 */
bool LocalDevice::canPowerOff() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->canPowerOff();

    return false;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::canRestart
 *
 * This property holds whether the current user can restart the system.
 *
 * \sa Liri::LocalDevice::LocalDevice::restart()
 */

/*!
 * \property LocalDevice::canRestart
 *
 * This property holds whether the current user can restart the system.
 *
 * \sa LocalDevice::restart()
 */
bool LocalDevice::canRestart() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->canRestart();

    return false;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::canSuspend
 *
 * This property holds whether the current user can suspend the system.
 *
 * \sa Liri::LocalDevice::LocalDevice::suspend()
 */

/*!
 * \property LocalDevice::canSuspend
 *
 * This property holds whether the current user can suspend the system.
 *
 * \sa LocalDevice::suspend()
 */
bool LocalDevice::canSuspend() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->canSuspend();

    return false;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::canHibernate
 *
 * This property holds whether the current user can hibernate the system.
 *
 * \sa Liri::LocalDevice::LocalDevice::hibernate()
 */

/*!
 * \property LocalDevice::canHibernate
 *
 * This property holds whether the current user can hibernate the system.
 *
 * \sa LocalDevice::hibernate()
 */
bool LocalDevice::canHibernate() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->canHibernate();

    return false;
}

/*!
 * \qmlproperty bool Liri::LocalDevice::LocalDevice::canHybridSleep
 *
 * This property holds whether the current user can hybrid sleep the system.
 *
 * \sa Liri::LocalDevice::LocalDevice::hybridSleep()
 */

/*!
 * \property LocalDevice::canHybridSleep
 *
 * This property holds whether the current user can hybrid sleep the system.
 *
 * \sa LocalDevice::hybridSleep()
 */
bool LocalDevice::canHybridSleep() const
{
    Q_D(const LocalDevice);

    if (d->systemd)
        return d->systemd->canHybridSleep();

    return false;
}

/*!
 * Processes are stopped and the system powered down.
 */
void LocalDevice::powerOff()
{
    Q_D(LocalDevice);

    if (d->systemd)
        d->systemd->powerOff();
    else if (d->upower)
        d->upower->powerOff();
}

/*!
 * Processes are stopped and the system is restarted.
 */
void LocalDevice::restart()
{
    Q_D(LocalDevice);

    if (d->systemd)
        d->systemd->restart();
    else if (d->upower)
        d->upower->restart();
}

/*!
 * Processes are stopped and some hardware is deactivated.
 */
void LocalDevice::suspend()
{
    Q_D(LocalDevice);

    if (d->systemd)
        d->systemd->suspend();
    else if (d->upower)
        d->upower->suspend();
}

/*!
 * The state of the machine is saved to disk and the machine is powered down.
 */
void LocalDevice::hibernate()
{
    Q_D(LocalDevice);

    if (d->systemd)
        d->systemd->hibernate();
    else if (d->upower)
        d->upower->hibernate();
}

/*!
 * The state of the machine is saved to disk like for regular hibernation,
 * but then instead of powering down, the system enters sleep mode.
 */
void LocalDevice::hybridSleep()
{
    Q_D(LocalDevice);

    if (d->systemd)
        d->systemd->hybridSleep();
    else if (d->upower)
        d->upower->hybridSleep();
}

} // namespace Liri
