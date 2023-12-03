// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
