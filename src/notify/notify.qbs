import qbs 1.0

CppApplication {
    name: "liri-notify"
    targetName: "liri-notify"

    Depends { name: "lirideployment" }
    Depends { name: "Qt.core" }
    Depends { name: "LiriNotifications" }

    cpp.defines: base.concat(['LIBLIRI_VERSION="' + project.version + '"'])
    cpp.includePaths: base.concat([".."])

    files: ["*.cpp", "*.h"]

    Group {
        qbs.install: true
        qbs.installDir: lirideployment.binDir
        fileTagsFilter: product.type
    }
}
