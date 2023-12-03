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

#include <QVector>

#include "localdevice.h"
#include "backends/upowerbackend_p.h"
#include "backends/systemdbackend_p.h"

namespace Liri {

class LocalDevicePrivate
{
public:
    explicit LocalDevicePrivate();
    ~LocalDevicePrivate();

    OsRelease *osRelease = nullptr;
    UPowerBackend *upower = nullptr;
    SystemdBackend *systemd = nullptr;
};

} // namespace Liri
