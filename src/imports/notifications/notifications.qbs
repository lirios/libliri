import qbs 1.0

LiriQmlPlugin {
    name: "lirinotificationsplugin"
    pluginPath: "Liri/Notifications"

    Depends { name: "LiriNotifications" }

    cpp.defines: base.concat(['LIBLIRI_VERSION="' + project.version + '"'])

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]

    Group {
        name: "D-Bus Adaptors"
        files: [
            "../../notifications/org.freedesktop.Notifications.xml"
        ]
        fileTags: ["qt.dbus.adaptor"]
    }
}
