import qbs 1.0

CppApplication {
    name: "notify"

    targetName: "notify"

    Depends { name: "Qt.core" }
    Depends { name: "LiriNotifications" }

    cpp.defines: base.concat(['LIBLIRI_VERSION="' + project.version + '"'])

    files: ["*.cpp", "*.h"]
}
