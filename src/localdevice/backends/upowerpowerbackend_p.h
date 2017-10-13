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

#include <QDBusInterface>

#include "localdevicepowerbackend_p_p.h"

namespace Liri {

class UPowerPowerBackend : public LocalDevicePowerBackend
{
    Q_OBJECT
public:
    explicit UPowerPowerBackend(QObject *parent = nullptr);
    virtual ~UPowerPowerBackend();

    static QString service();

    QString name() const override;

    bool canPowerOff() const override;
    bool canRestart() const override;
    bool canSuspend() const override;
    bool canHibernate() const override;
    bool canHybridSleep() const override;

    void powerOff() override;
    void restart() override;
    void suspend() override;
    void hibernate() override;
    void hybridSleep() override;

    static bool check();

private:
    QDBusInterface *m_interface = nullptr;

    bool m_canPowerOff = false;
    bool m_canRestart = false;
    bool m_canSuspend = false;
    bool m_canHibernate = false;
    bool m_canHybridSleep = false;

    void getUPowerProperty(const QString &name, bool *prop);

private Q_SLOTS:
    void upowerChanged(const QString &interface, QVariantMap &properties, const QStringList &invalidatedProperties);
};

} // namespace Liri
