set(SOURCES
    dbusservice.cpp dbusservice.h dbusservice_p.h
)

qt6_add_dbus_adaptor(SOURCES
    org.freedesktop.Application.xml
    dbusservice.h
    Liri::DBusService
)

liri_add_module(DBusService
    DESCRIPTION
        "D-Bus activation for Qt applications"
    SOURCES
        ${SOURCES}
    PUBLIC_LIBRARIES
        Qt6::Core
        Qt6::DBus
        Qt6::Gui
    PKGCONFIG_DEPENDENCIES
        Qt6Core
        Qt6DBus
        Qt6Gui
)

liri_finalize_module(DBusService)
