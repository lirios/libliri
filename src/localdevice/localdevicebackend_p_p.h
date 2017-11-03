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
