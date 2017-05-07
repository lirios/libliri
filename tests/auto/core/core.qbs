import qbs 1.0

QtApplication {
    name: "tst_liri_core"
    type: base.concat(["autotest"])

    Depends { name: "Qt"; submodules: ["core", "qml", "testlib"] }
    Depends { name: "libLiriCore" }

    cpp.includePaths: base.concat(["../../../src/imports/core"])

    files: [
        "*.cpp",
        "../../../src/imports/core/qobjectlistmodel.cpp",
        "../../../src/imports/core/qobjectlistmodel.h"
    ]
}
