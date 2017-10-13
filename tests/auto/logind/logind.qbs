import qbs 1.0

QtApplication {
    name: "tst_liri_logind"
    type: base.concat(["autotest"])

    Depends { name: "Qt"; submodules: ["gui", "testlib"] }
    Depends { name: "LiriLogind" }

    files: ["*.cpp", "*.h"]
}
