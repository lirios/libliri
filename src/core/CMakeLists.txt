liri_add_module(Core
    DESCRIPTION
        "Utilities for Liri apps"
    SOURCES
        kformat.cpp
        kformat.h
        kformatprivate.cpp
        kformatprivate_p.h
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
    PKGCONFIG_DEPENDENCIES
        Qt5Core
)

liri_finalize_module(Core)
