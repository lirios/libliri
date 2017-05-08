import qbs 1.0

LiriModule {
    name: "LiriNotifications"
    targetName: "LiriNotifications"
    version: "0.0.0"

    Depends { name: "Qt"; submodules: ["core", "core-private", "dbus", "gui"] }

    cpp.defines: base.concat([
        'LIBLIRI_VERSION="' + project.version + '"',
        "QT_BUILD_LIRINOTIFICATIONS_LIB"
    ])

    create_headers.headersMap: ({
        "notification.h": "Notification",
    })

    create_pkgconfig.name: "Liri Notifications"
    create_pkgconfig.description: "Qt style API for freedesktop.org notifications"
    create_pkgconfig.version: project.version
    create_pkgconfig.dependencies: ["Qt5Core", "Qt5DBus", "Qt5Gui"]

    create_cmake.version: project.version
    create_cmake.dependencies: ({
        "Qt5Core": "5.6.0",
        "Qt5DBus": "5.6.0",
        "Qt5Gui": "5.6.0",
    })
    create_cmake.linkLibraries: ["Qt5::Core", "Qt5::DBus", "Qt5::Gui"]

    files: ["*.cpp"]

    Group {
        name: "Headers"
        files: ["*.h"]
        excludeFiles: ["*_p.h"]
        fileTags: ["public_headers"]
    }

    Group {
        name: "Private Headers"
        files: ["*_p.h"]
        fileTags: ["private_headers"]
    }

    Group {
        name: "D-Bus Interfaces"
        files: [
            "org.freedesktop.Notifications.xml"
        ]
        fileTags: ["qt.dbus.interface"]
    }

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "core-private", "dbus", "gui"] }

        cpp.includePaths: base.concat([product.generatedHeadersDir])
    }
}
