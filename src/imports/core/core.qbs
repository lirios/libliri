import qbs 1.0

LiriQmlPlugin {
    name: "liricoreplugin"
    pluginPath: "Liri/Core"

    Depends { name: "LiriCore" }

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]
}
