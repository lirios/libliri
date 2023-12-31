# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

liri_add_module(LocalDevice
    DESCRIPTION
        "Operations on local device such as reboot, power off, suspend, etc..."
    SOURCES
        localdevicebackend_p.cpp localdevicebackend_p_p.h
        localdevice.cpp localdevice.h localdevice_p.h
        osrelease.cpp osrelease.h osrelease_p.h
        backends/systemdbackend.cpp backends/systemdbackend_p.h
        backends/upowerbackend.cpp backends/upowerbackend_p.h
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::DBus
        Qt6::Qml
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6DBus
        Qt6Qml
)
liri_finalize_module(LocalDevice)

ecm_add_qml_module(LocalDevice
    URI Liri.Device
    VERSION 1.0
    GENERATE_PLUGIN_SOURCE
)
ecm_finalize_qml_module(LocalDevice)
