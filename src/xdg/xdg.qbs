import qbs 1.0
import LiriUtils

LiriModuleProject {
    name: "LiriXdg"
    moduleName: "LiriXdg"
    description: "Qt style API for freedesktop.org specifications"

    resolvedProperties: ({
        Depends: [{ name: LiriUtils.quote("Qt.core") },
                  { name: LiriUtils.quote("Qt.dbus") },
                  { name: LiriUtils.quote("Qt.gui") }]
    })

    pkgConfigDependencies: ["Qt5Core", "Qt5Gui"]

    cmakeDependencies: ({ "Qt5Core": "5.6.0", "Qt5Gui": "5.6.0" })
    cmakeLinkLibraries: ["Qt5::Core", "Qt5::Gui"]

    LiriHeaders {
        name: project.headersName
        sync.module: project.moduleName

        Group {
            name: "Headers"
            files: [
                "lirixdgglobal.h",
            ]
            fileTags: ["hpp_syncable"]
        }
    }

    LiriModule {
        name: project.moduleName
        targetName: project.targetName
        version: "0.0.0"

        Depends { name: project.headersName }
        Depends { name: "Qt"; submodules: ["core", "dbus", "gui"] }

        cpp.defines: base.concat([
            'LIBLIRI_VERSION="' + project.version + '"',
            "LIRI_BUILD_LIRIXDG_LIB"
        ])

        files: [
            "desktopfile.cpp",
            "desktopfile.h",
            "desktopfile_p.h",
            "lirixdgglobal.h",
            "xdgdesktopfilereader_p.cpp",
            "xdgdesktopfilereader_p.h",
            "xdgdesktopfilereader_p_p.h",
            "xdgdirs_p.cpp",
            "xdgdirs_p.h",
        ]

        Export {
            Depends { name: "cpp" }
            Depends { name: project.headersName }
            Depends { name: "Qt"; submodules: ["core", "dbus", "gui"] }
        }
    }
}
