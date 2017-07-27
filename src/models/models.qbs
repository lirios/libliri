import qbs 1.0
import LiriUtils

LiriModuleProject {
    id: root

    name: "LiriModels"
    moduleName: "LiriModels"
    description: "Utilities for Liri apps"

    resolvedProperties: ({
        Depends: [{ name: LiriUtils.quote("Qt.core") },
                  { name: LiriUtils.quote("Qt.qml") }]
    })

    pkgConfigDependencies: ["Qt5Core", "Qt5Qml"]

    cmakeDependencies: ({ "Qt5Core": "5.6.0", "Qt5Qml": "5.6.0" })
    cmakeLinkLibraries: ["Qt5::Core", "Qt5::Qml"]

    LiriHeaders {
        name: root.headersName
        sync.module: root.moduleName
        sync.classNames: ({
            "qquicklist.h": ["QQuickList"],
        })

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
        Depends { name: "Qt"; submodules: ["core", "qml"] }

        cpp.defines: [
            'LIBLIRI_VERSION="' + project.version + '"',
            "QT_BUILD_LIRIMODELS_LIB"
        ]

        files: ["*.cpp", "*.h"]

        Export {
            Depends { name: "cpp" }
            Depends { name: root.headersName }
            Depends { name: "Qt"; submodules: ["core", "qml"] }
        }
    }
}
