// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_LIRICORE_LIB)
#  define LIRICORE_EXPORT Q_DECL_EXPORT
#else
#  define LIRICORE_EXPORT Q_DECL_IMPORT
#endif
#define LIRICORE_NO_EXPORT Q_DECL_HIDDEN
