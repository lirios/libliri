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
#include "backends/systemdpowerbackend_p.h"
#include "backends/systemdsystembackend_p.h"
#include "backends/upowerpowerbackend_p.h"

namespace Liri {

LocalDevicePrivate::LocalDevicePrivate()
{
    if (SystemdPowerBackend::check())
        powerBackend = new SystemdPowerBackend();
    else if (UPowerPowerBackend::check())
        powerBackend = new UPowerPowerBackend();

    if (SystemdSystemBackend::check())
        systemBackend = new SystemdSystemBackend();
}

LocalDevicePrivate::~LocalDevicePrivate()
{
    if (powerBackend) {
        powerBackend->deleteLater();
        powerBackend = nullptr;
    }

    if (systemBackend) {
        systemBackend->deleteLater();
        systemBackend = nullptr;
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
    if (d_func()->powerBackend) {
        connect(d_func()->powerBackend, &LocalDevicePowerBackend::canPowerOffChanged, this, &LocalDevice::canPowerOffChanged);
        connect(d_func()->powerBackend, &LocalDevicePowerBackend::canRestartChanged, this, &LocalDevice::canRestartChanged);
        connect(d_func()->powerBackend, &LocalDevicePowerBackend::canSuspendChanged, this, &LocalDevice::canSuspendChanged);
        connect(d_func()->powerBackend, &LocalDevicePowerBackend::canHibernateChanged, this, &LocalDevice::canHibernateChanged);
        connect(d_func()->powerBackend, &LocalDevicePowerBackend::canHybridSleepdChanged, this, &LocalDevice::canHybridSleepdChanged);
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

    if (d->systemBackend)
        return d->systemBackend->chassis();

    return LocalDevice::UnknownChassis;
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

    if (d->systemBackend)
        return d->systemBackend->hostName();

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

    if (d->systemBackend)
        return d->systemBackend->iconName();

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
    Q_D(const LocalDevice);

    if (d->systemBackend)
        return d->systemBackend->materialIconName();

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

    if (d->systemBackend)
        return d->systemBackend->operatingSystemName();

    return QSysInfo::prettyProductName();
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

    if (d->powerBackend)
        return d->powerBackend->canPowerOff();

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

    if (d->powerBackend)
        return d->powerBackend->canRestart();

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

    if (d->powerBackend)
        return d->powerBackend->canSuspend();

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

    if (d->powerBackend)
        return d->powerBackend->canHibernate();

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

    if (d->powerBackend)
        return d->powerBackend->canHybridSleep();

    return false;
}

/*!
 * Processes are stopped and the system powered down.
 */
void LocalDevice::powerOff()
{
    Q_D(LocalDevice);

    if (canPowerOff() && d->powerBackend)
        d->powerBackend->powerOff();
}

/*!
 * Processes are stopped and the system is restarted.
 */
void LocalDevice::restart()
{
    Q_D(LocalDevice);

    if (canRestart() && d->powerBackend)
        d->powerBackend->restart();
}

/*!
 * Processes are stopped and some hardware is deactivated.
 */
void LocalDevice::suspend()
{
    Q_D(LocalDevice);

    if (canSuspend() && d->powerBackend)
        d->powerBackend->suspend();
}

/*!
 * The state of the machine is saved to disk and the machine is powered down.
 */
void LocalDevice::hibernate()
{
    Q_D(LocalDevice);

    if (canHibernate() && d->powerBackend)
        d->powerBackend->hibernate();
}

/*!
 * The state of the machine is saved to disk like for regular hibernation,
 * but then instead of powering down, the system enters sleep mode.
 */
void LocalDevice::hybridSleep()
{
    Q_D(LocalDevice);

    if (canHybridSleep() && d->powerBackend)
        d->powerBackend->hybridSleep();
}

} // namespace Liri
