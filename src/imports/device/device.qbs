import qbs 1.0

LiriQmlPlugin {
    name: "lirideviceplugin"
    pluginPath: "Liri/Device"

    Depends { name: "LiriLocalDevice" }

    files: ["*.cpp", "*.h", "qmldir", "*.qmltypes"]
}
