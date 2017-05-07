import qbs 1.0

LiriDynamicLibrary {
    name: "liricoreplugin"
    targetName: "liricoreplugin"

    Depends { name: "lirideployment" }
    Depends { name: "Qt"; submodules: ["qml", "quick"] }
    Depends { name: "LiriCore" }

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
        qbs.install: true
        qbs.installDir: lirideployment.qmlDir + "/Liri/Core"
        fileTagsFilter: ["dynamiclibrary", "qml"]
    }
}
