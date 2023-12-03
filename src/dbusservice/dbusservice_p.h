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

namespace Liri {

class DBusServicePrivate
{
public:
    explicit DBusServicePrivate();

    QString getServiceName() const;
    QString getObjectPath(const QString &name) const;

    bool registered = false;
    QString errorMessage;
    QString serviceName;
};

} // namespace Liri
