import qbs 1.0

LiriModule {
    name: "LiriModels"
    targetName: "LiriModels"
    version: "0.0.0"

    Depends { name: "Qt"; submodules: ["core", "qml"] }

    cpp.defines: [
        'LIBLIRI_VERSION="' + project.version + '"',
        "QT_BUILD_LIRIMODELS_LIB"
    ]

    create_headers.headersMap: ({
        "qobjectlistmodel.h": "QObjectListModel",
        "qquicklist.h": "QQuickList",
    })

    create_pkgconfig.name: "Liri Models"
    create_pkgconfig.description: "Utilities for Liri apps"
    create_pkgconfig.version: project.version
    create_pkgconfig.dependencies: ["Qt5Core", "Qt5Qml"]

    create_cmake.version: project.version
    create_cmake.dependencies: ({
        "Qt5Core": "5.6.0",
        "Qt5Qml": "5.6.0",
    })
    create_cmake.linkLibraries: ["Qt5::Core", "Qt5::Qml"]

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
        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "qml"] }

        cpp.includePaths: base.concat([product.generatedHeadersDir])
    }
}
