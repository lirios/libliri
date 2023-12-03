// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
