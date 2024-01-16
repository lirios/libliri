# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

## Enable feature summary at the end of the configure run:
include(FeatureSummary)

## Find Qt:
set(QT_MIN_VERSION "6.6.0")
find_package(Qt6 "${QT_MIN_VERSION}"
    REQUIRED
    COMPONENTS
        Core
	Core5Compat
        Xml
        DBus
        Qml
        QmlIntegration
        Quick
        Gui
        Test
)

#### Features

## Features summary:
if(NOT LIRI_SUPERBUILD)
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
