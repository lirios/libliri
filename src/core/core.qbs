import qbs 1.0

LiriModule {
    name: "LiriCore"
    targetName: "LiriCore"
    version: "0.0.0"

    Depends { name: "Qt"; submodules: ["core", "core-private"] }
    Depends { name: "Qt5Xdg" }

    cpp.defines: [
        'LIBLIRI_VERSION="' + project.version + '"',
        "QT_BUILD_LIRICORE_LIB"
    ]

    create_headers.headersMap: ({
        "desktopfile.h": "DesktopFile",
        "desktopfileaction.h": "DesktopFileAction",
    })

    create_pkgconfig.name: "Liri Core"
    create_pkgconfig.description: "Utilities for Liri apps"
    create_pkgconfig.version: project.version
    create_pkgconfig.dependencies: ["Qt5Core", "Qt5Xdg"]

    create_cmake.version: project.version
    create_cmake.dependencies: ({
        "Qt5Core": "5.6.0",
        "qt5xdg": "2.0.0",
    })
    create_cmake.linkLibraries: ["Qt5::Core", "Qt5Xdg"]

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

    Export {
        property bool found: true

        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "core-private"] }
        Depends { name: "Qt5Xdg" }

        cpp.includePaths: base.concat([product.generatedHeadersDir])
    }
}
