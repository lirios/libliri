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

#include <QString>
#include <QVariantMap>

class OrgFreedesktopNotificationsInterface;

namespace Liri {

class Notification;

class NotificationPrivate
{
public:
    explicit NotificationPrivate();

    OrgFreedesktopNotificationsInterface *iface;

    QString appName;
    QString appIcon;
    QString summary;
    QString body;
    quint32 replacesId;
    int timeout;
    QString defaultAction;
    QString defaultActionTarget = QStringLiteral("default");
    QVariantMap hints;
    QStringList actions;
};

} // namespace Liri
