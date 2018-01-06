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

#pragma once

#include <QObject>

#include <LiriLocalDevice/lirilocaldeviceglobal.h>

namespace Liri {

class LocalDevicePrivate;

class LIRILOCALDEVICE_EXPORT LocalDevice : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LocalDevice)
    Q_DISABLE_COPY(LocalDevice)
    Q_PROPERTY(Chassis chassis READ chassis CONSTANT)
    Q_PROPERTY(PowerSource powerSource READ powerSource NOTIFY powerSourceChanged)
    Q_PROPERTY(bool lidPresent READ lidPresent NOTIFY lidPresentChanged)
    Q_PROPERTY(bool lidClosed READ lidClosed NOTIFY lidClosedChanged)
    Q_PROPERTY(QString hostName READ hostName CONSTANT)
    Q_PROPERTY(QString iconName READ iconName CONSTANT)
    Q_PROPERTY(QString materialIconName READ materialIconName CONSTANT)
    Q_PROPERTY(QString operatingSystemName READ operatingSystemName CONSTANT)
    Q_PROPERTY(QString virtualization READ virtualization CONSTANT)
    Q_PROPERTY(bool canPowerOff READ canPowerOff NOTIFY canPowerOffChanged)
    Q_PROPERTY(bool canRestart READ canRestart NOTIFY canRestartChanged)
    Q_PROPERTY(bool canSuspend READ canSuspend NOTIFY canSuspendChanged)
    Q_PROPERTY(bool canHibernate READ canHibernate NOTIFY canHibernateChanged)
    Q_PROPERTY(bool canHybridSleep READ canHybridSleep NOTIFY canHybridSleepdChanged)
public:
    enum Chassis {
        UnknownChassis = 0,
        DesktopChassis,
        LaptopChassis,
        ServerChassis,
        TabletChassis,
        PhoneChassis,
        VirtualMachineChassis,
        ContainerChassis
    };
    Q_ENUM(Chassis)

    enum PowerSource {
        BatteryPower = 0,
        ACPower
    };
    Q_ENUM(PowerSource)

    explicit LocalDevice(QObject *parent = nullptr);
    ~LocalDevice();

    Chassis chassis() const;

    PowerSource powerSource() const;

    bool lidPresent() const;
    bool lidClosed() const;

    QString hostName() const;

    QString iconName() const;
    QString materialIconName() const;

    QString operatingSystemName() const;

    QString virtualization() const;

    bool canPowerOff() const;
    bool canRestart() const;
    bool canSuspend() const;
    bool canHibernate() const;
    bool canHybridSleep() const;

Q_SIGNALS:
    void powerSourceChanged();
    void lidPresentChanged();
    void lidClosedChanged();
    void canPowerOffChanged();
    void canRestartChanged();
    void canSuspendChanged();
    void canHibernateChanged();
    void canHybridSleepdChanged();

public Q_SLOTS:
    void powerOff();
    void restart();
    void suspend();
    void hibernate();
    void hybridSleep();

private:
    LocalDevicePrivate *const d_ptr;
};

} // namespace Liri

Q_DECLARE_METATYPE(Liri::LocalDevice *)
