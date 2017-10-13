import qbs 1.0

Project {
    name: "QML Plugins"

    references: [
        "core/core.qbs",
        "device/device.qbs",
        "notifications/notifications.qbs",
    ]
}
