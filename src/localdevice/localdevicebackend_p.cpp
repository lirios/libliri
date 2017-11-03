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

#include "localdevicebackend_p_p.h"

namespace Liri {

LocalDeviceBackend::LocalDeviceBackend(QObject *parent)
    : QObject(parent)
{
}

LocalDeviceBackend::~LocalDeviceBackend()
{
}

LocalDevice::Chassis LocalDeviceBackend::chassis() const
{
    return LocalDevice::UnknownChassis;
}

QString LocalDeviceBackend::hostName() const
{
    return QString();
}

QString LocalDeviceBackend::iconName() const
{
    return QString();
}

QString LocalDeviceBackend::operatingSystemName() const
{
    return QString();
}

QString LocalDeviceBackend::virtualization() const
{
    return QString();
}

LocalDevice::PowerSource LocalDeviceBackend::powerSource() const
{
    return LocalDevice::ACPower;
}

bool LocalDeviceBackend::lidPresent() const
{
    return false;
}

bool LocalDeviceBackend::lidClosed() const
{
    return false;
}

bool LocalDeviceBackend::canPowerOff() const
{
    return false;
}

bool LocalDeviceBackend::canRestart() const
{
    return false;
}

bool LocalDeviceBackend::canSuspend() const
{
    return false;
}

bool LocalDeviceBackend::canHibernate() const
{
    return false;
}

bool LocalDeviceBackend::canHybridSleep() const
{
    return false;
}

void LocalDeviceBackend::powerOff()
{
}

void LocalDeviceBackend::restart()
{
}

void LocalDeviceBackend::suspend()
{
}

void LocalDeviceBackend::hibernate()
{
}

void LocalDeviceBackend::hybridSleep()
{
}

} // namespace Liri
