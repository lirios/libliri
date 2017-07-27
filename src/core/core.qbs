import qbs 1.0
import LiriUtils

LiriModuleProject {
    id: root

    name: "LiriCore"
    moduleName: "LiriCore"
    description: "Utilities for Liri apps"

    resolvedProperties: ({
        Depends: [{ name: LiriUtils.quote("Qt.core") },
                  { name: LiriUtils.quote("Qt.core-private") },
                  { name: LiriUtils.quote("Qt5Xdg") }],
    })

    pkgConfigDependencies: ["Qt5Core", "Qt5Xdg"]

    cmakeDependencies: ({ "Qt5Core": "5.6.0", "qt5xdg": "2.0.0" })
    cmakeLinkLibraries: ["Qt5::Core", "Qt5Xdg"]

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
        Depends { name: "Qt"; submodules: ["core", "core-private"] }
        Depends { name: "Qt5Xdg" }

        condition: {
            if (!Qt5Xdg.found) {
                console.error("Qt5Xdg is required to build " + targetName);
                return false;
            }

            return true;
        }

        cpp.defines: [
            'LIBLIRI_VERSION="' + project.version + '"',
            "QT_BUILD_LIRICORE_LIB"
        ]

        files: ["*.cpp", "*.h"]

        Export {
            Depends { name: "cpp" }
            Depends { name: root.headersName }
            Depends { name: "Qt"; submodules: ["core", "core-private"] }
            Depends { name: "Qt5Xdg" }
        }
    }
}
