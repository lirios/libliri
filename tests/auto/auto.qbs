import qbs 1.0

Project {
    name: "Autotests"

    references: [
        "core/core.qbs",
        //"logind/logind.qbs",
    ]

    AutotestRunner {
        builtByDefault: project.autotestEnabled
        name: "libliri-autotest"
        arguments: project.autotestArguments
        wrapper: project.autotestWrapper
    }
}
