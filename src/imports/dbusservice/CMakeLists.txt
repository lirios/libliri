ecm_add_qml_module(DBusServiceQmlPlugin
    URI
        "Liri.DBusService"
    VERSION
        "1.0"
    GENERATE_PLUGIN_SOURCE
)

target_sources(DBusServiceQmlPlugin
    PRIVATE
        application.cpp application.h
)

target_link_libraries(DBusServiceQmlPlugin
    PUBLIC
        Qt6::Core
        Qt6::DBus
        Qt6::Qml
        Liri::Core
)

ecm_finalize_qml_module(DBusServiceQmlPlugin)
