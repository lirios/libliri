liri_add_module(Core
    DESCRIPTION
        "Utilities for Liri apps"
    SOURCES
        kformat.cpp kformat.h
        kformatprivate.cpp kformatprivate_p.h
    PUBLIC_LIBRARIES
        Qt6::Core
    PKGCONFIG_DEPENDENCIES
        Qt6Core
)

liri_finalize_module(Core)
