import qbs 1.0
import LiriUtils

LiriModuleProject {
    id: root

    name: "LiriNotifications"
    moduleName: "LiriNotifications"
    description: "Qt style API for freedesktop.org notifications"

    resolvedProperties: ({
        Depends: [{ name: LiriUtils.quote("Qt.core") },
                  { name: LiriUtils.quote("Qt.core-private") },
                  { name: LiriUtils.quote("Qt.dbus") },
                  { name: LiriUtils.quote("Qt.gui") }]
    })

    pkgConfigDependencies: ["Qt5Core", "Qt5DBus", "Qt5Gui"]

    cmakeDependencies: ({ "Qt5Core": "5.6.0", "Qt5DBus": "5.6.0", "Qt5Gui": "5.6.0" })
    cmakeLinkLibraries: ["Qt5::Core", "Qt5::DBus", "Qt5::Gui"]

    LiriHeaders {
        name: root.headersName
        sync.module: root.moduleName

        Group {
            name: "Headers"
            files: "**/*.h"
            fileTags: ["hpp_syncable"]
        }
    }

    LiriModule {
        name: root.moduleName
        targetName: root.targetName
        version: "0.0.0"

        Depends { name: root.headersName }
        Depends { name: "Qt"; submodules: ["core", "core-private", "dbus", "gui"] }

        cpp.defines: base.concat([
            'LIBLIRI_VERSION="' + project.version + '"',
            "QT_BUILD_LIRINOTIFICATIONS_LIB"
        ])

        files: ["*.cpp", "*.h"]

        Group {
            name: "D-Bus Interfaces"
            files: [
                "org.freedesktop.Notifications.xml"
            ]
            fileTags: ["qt.dbus.interface"]
        }

        Export {
            Depends { name: "cpp" }
            Depends { name: root.headersName }
            Depends { name: "Qt"; submodules: ["core", "core-private", "dbus", "gui"] }
        }
    }
}
