find_package(Qt5Xdg REQUIRED)

liri_add_module(Core
    DESCRIPTION
        "Utilities for Liri apps"
    SOURCES
        desktopfileaction.cpp
        desktopfileaction.h
        desktopfile.cpp
        desktopfile.h
        kformat.cpp
        kformat.h
        kformatprivate.cpp
        kformatprivate_p.h
    INSTALL_HEADERS
        kformat.h
    FORWARDING_HEADERS
        DesktopFileAction
        DesktopFile
        KFormat
    PRIVATE_HEADERS
        kformatprivate_p.h
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
    LIBRARIES
        Qt5Xdg
    PKGCONFIG_DEPENDENCIES
        Qt5Core
        Qt5Xdg
)
