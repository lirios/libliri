ecm_add_qml_module(NotificationsQmlPlugin
    URI
        "Liri.Notifications"
    VERSION
        "1.0"
    GENERATE_PLUGIN_SOURCE
)

target_link_libraries(NotificationsQmlPlugin
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Liri::Notifications
)

ecm_finalize_qml_module(NotificationsQmlPlugin)
