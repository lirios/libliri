// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QDBusInterface>
#include <QLoggingCategory>

#include "localdevicebackend_p_p.h"

Q_DECLARE_LOGGING_CATEGORY(lcSystemdBackend)

namespace Liri {

class SystemdBackend : public LocalDeviceBackend
{
    Q_OBJECT
public:
    explicit SystemdBackend(QObject *parent = nullptr);

    static QString service();

    QString name() const override;

    LocalDevice::Chassis chassis() const override;

    QString hostName() const override;

    QString iconName() const override;

    QString operatingSystemName() const override;

    QString virtualization() const override;

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
    QString m_virtualization;
    QString m_chassis;
    QString m_hostName;
    QString m_iconName;
    QString m_osName;
    bool m_canPowerOff = false;
    bool m_canRestart = false;
    bool m_canSuspend = false;
    bool m_canHibernate = false;
    bool m_canHybridSleep = false;

    void getLogin1Property(const QString &name);
    void getHostname1Property(const QString &name);
    void getSystemd1Property(const QString &name);
};

} // namespace Liri
