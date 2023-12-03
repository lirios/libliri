// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QtQmlIntegration>

#include <LiriLocalDevice/OsRelease>

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
    Q_PROPERTY(OsRelease *osRelease READ osRelease CONSTANT)
    QML_ELEMENT
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

    OsRelease *osRelease() const;

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
