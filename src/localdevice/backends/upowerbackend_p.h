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

#include "localdevicebackend_p_p.h"

namespace Liri {

class UPowerBackend : public LocalDeviceBackend
{
    Q_OBJECT
public:
    explicit UPowerBackend(QObject *parent = nullptr);

    static QString service();

    QString name() const override;

    LocalDevice::PowerSource powerSource() const override;

    bool lidPresent() const override;
    bool lidClosed() const override;

    void powerOff() override;
    void restart() override;
    void suspend() override;
    void hibernate() override;

    static bool check();

private:
    bool m_onBattery = false;
    bool m_lidPresent = false;
    bool m_lidClosed = false;

    void getUPowerProperty(const QString &name);

private Q_SLOTS:
    void upowerChanged(const QString &interface, QVariantMap &properties, const QStringList &invalidatedProperties);
};

} // namespace Liri
