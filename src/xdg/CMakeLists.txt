# SPDX-FileCopyrightText: 2023 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

liri_add_module(Xdg
    DESCRIPTION
        "Freedesktop.org implementation of some specifications"
    SOURCES
        autostart.cpp autostart.h
        desktopfile.cpp desktopfile.h desktopfile_p.h
        desktopfileutils.cpp desktopfileutils_p.h
        desktopmenu.cpp desktopmenu.h desktopmenu_p.h
        logging.cpp logging_p.h
        xdgdirs_p.cpp xdgdirs_p_p.h
        xdgmenuapplinkprocessor_p.cpp xdgmenuapplinkprocessor_p_p.h
        xdgmenulayoutprocessor_p.cpp xdgmenulayoutprocessor_p_p.h
        xdgmenureader_p.cpp xdgmenureader_p_p.h
        xdgmenurules_p.cpp xdgmenurules_p_p.h
        xmlhelper_p.cpp xmlhelper_p_p.h
    PRIVATE_HEADERS
        desktopfile_p.h
        desktopmenu_p.h
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::Core5Compat
        Qt6::DBus
        Qt6::Xml
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6DBus
        Qt6Xml
)

liri_finalize_module(Xdg)
