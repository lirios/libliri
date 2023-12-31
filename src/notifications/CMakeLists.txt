# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

set(SOURCES
    notification.cpp notification.h notification_p.h
)

qt6_add_dbus_interface(SOURCES
    org.freedesktop.Notifications.xml
    notifications_interface
)

liri_add_module(Notifications
    DESCRIPTION
        "Qt style API for freedesktop.org notifications"
    SOURCES
        ${SOURCES}
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::DBus
        Qt6::QmlIntegration
        Qt6::Gui
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6DBus
        Qt6QmlIntegration
        Qt6Gui
)
liri_finalize_module(Notifications)

ecm_add_qml_module(Notifications
    URI Liri.Notifications
    VERSION 1.0
    GENERATE_PLUGIN_SOURCE
)
ecm_finalize_qml_module(Notifications)
