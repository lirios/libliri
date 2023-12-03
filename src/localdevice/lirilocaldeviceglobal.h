// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_LIRILOCALDEVICE_LIB)
#  define LIRILOCALDEVICE_EXPORT Q_DECL_EXPORT
#else
#  define LIRILOCALDEVICE_EXPORT Q_DECL_IMPORT
#endif
#define LIRILOCALDEVICE_NO_EXPORT Q_DECL_HIDDEN
