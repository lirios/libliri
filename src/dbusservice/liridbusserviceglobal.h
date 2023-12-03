// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_LIRIDBUSSERVICE_LIB)
#  define LIRIDBUSSERVICE_EXPORT Q_DECL_EXPORT
#else
#  define LIRIDBUSSERVICE_EXPORT Q_DECL_IMPORT
#endif
#define LIRIDBUSSERVICE_NO_EXPORT Q_DECL_HIDDEN
