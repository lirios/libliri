import qbs 1.0

LiriDynamicLibrary {
    name: "lirinotificationsplugin"
    targetName: "lirinotificationsplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["qml", "quick", "gui"] }
    Depends { name: "libLiriNotifications" }

    cpp.defines: base.concat(['LIBLIRI_VERSION="' + project.version + '"'])

    files: ["*.cpp", "*.h"]

    Group {
        name: "QML Files"
        files: [
            "qmldir",
            "plugins.qmltypes"
        ]
        fileTags: ["qml"]
    }

    Group {
        name: "D-Bus Adaptors"
        files: [
            "../../notifications/org.freedesktop.Notifications.xml"
        ]
        fileTags: ["qt.dbus.adaptor"]
    }

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Liri/Notifications"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
