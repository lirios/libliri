// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QtGlobal>

#include "localdevice.h"

namespace Liri {

class LocalDeviceBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LocalDevice::PowerSource powerSource READ powerSource NOTIFY powerSourceChanged)
    Q_PROPERTY(bool lidPresent READ lidPresent NOTIFY lidPresentChanged)
    Q_PROPERTY(bool lidClosed READ lidClosed NOTIFY lidClosedChanged)
    Q_PROPERTY(bool canPowerOff READ canPowerOff NOTIFY canPowerOffChanged)
    Q_PROPERTY(bool canRestart READ canRestart NOTIFY canRestartChanged)
    Q_PROPERTY(bool canSuspend READ canSuspend NOTIFY canSuspendChanged)
    Q_PROPERTY(bool canHibernate READ canHibernate NOTIFY canHibernateChanged)
    Q_PROPERTY(bool canHybridSleep READ canHybridSleep NOTIFY canHybridSleepdChanged)
public:
    explicit LocalDeviceBackend(QObject *parent = nullptr);
    virtual ~LocalDeviceBackend();

    virtual QString name() const = 0;

    virtual LocalDevice::Chassis chassis() const;

    virtual QString hostName() const;

    virtual QString iconName() const;

    virtual QString operatingSystemName() const;

    virtual QString virtualization() const;

    virtual LocalDevice::PowerSource powerSource() const;

    virtual bool lidPresent() const;
    virtual bool lidClosed() const;

    virtual bool canPowerOff() const;
    virtual bool canRestart() const;
    virtual bool canSuspend() const;
    virtual bool canHibernate() const;
    virtual bool canHybridSleep() const;

    virtual void powerOff();
    virtual void restart();
    virtual void suspend();
    virtual void hibernate();
    virtual void hybridSleep();

Q_SIGNALS:
    void powerSourceChanged();
    void lidPresentChanged();
    void lidClosedChanged();
    void canPowerOffChanged();
    void canRestartChanged();
    void canSuspendChanged();
    void canHibernateChanged();
    void canHybridSleepdChanged();
};

} // namespace Liri
