import qbs 1.0
import LiriUtils

LiriModuleProject {
    id: root

    name: "LiriLogind"
    moduleName: "LiriLogind"
    description: "Qt API for logind"

    resolvedProperties: ({
        Depends: [{ name: LiriUtils.quote("Qt.core") },
                  { name: LiriUtils.quote("Qt.dbus") }],
    })

    pkgConfigDependencies: ["Qt5Core", "Qt5DBus"]

    cmakeDependencies: ({ "Qt5Core": "5.6.0", "Qt5DBus": "5.6.0" })
    cmakeLinkLibraries: ["Qt5::Core", "Qt5::DBus"]

    LiriHeaders {
        name: root.headersName
        sync.module: root.moduleName

        Group {
            name: "Headers"
            files: "**/*.h"
            excludeFiles: ["*_p_p.h"]
            fileTags: ["hpp_syncable"]
        }
    }

    LiriModule {
        name: root.moduleName
        targetName: root.targetName
        version: "0.0.0"

        Depends { name: root.headersName }
        Depends { name: "Qt"; submodules: ["core", "dbus"] }

        cpp.defines: [
            'LIBLIRI_VERSION="' + project.version + '"',
            "QT_BUILD_LIRILOGIND_LIB"
        ]

        files: ["*.cpp", "*.h"]

        Export {
            Depends { name: "cpp" }
            Depends { name: root.headersName }
            Depends { name: "Qt"; submodules: ["core", "dbus"] }
        }
    }
}
