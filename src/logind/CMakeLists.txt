liri_add_module(Logind
    DESCRIPTION
        "Qt API for logind"
    SOURCES
        defaultlogind_p_p.h
        logind.cpp
        logind.h
        logind_p.h
        logindtypes.cpp
        logindtypes_p.h
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        QT_NO_KEYWORDS
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
    PKGCONFIG_DEPENDENCIES
        Qt5Core
        Qt5DBus
)

liri_finalize_module(Logind)
