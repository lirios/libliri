ecm_add_qml_module(DeviceQmlPlugin
    URI
        "Liri.Device"
    VERSION
        "1.0"
    GENERATE_PLUGIN_SOURCE
)

target_link_libraries(DeviceQmlPlugin
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Liri::LocalDevice
)

ecm_finalize_qml_module(DeviceQmlPlugin)
