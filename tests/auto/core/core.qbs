import qbs 1.0

QtApplication {
    name: "tst_liri_core"
    type: base.concat(["autotest"])

    Depends { name: "Qt"; submodules: ["core", "testlib"] }
    Depends { name: "libLiriCore" }

    files: ["*.cpp"]
}
