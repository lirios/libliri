import qbs 1.0
import qbs.FileInfo

Project {
    name: "Autotests"

    references: [
        "core/core.qbs",
        "logind/logind.qbs",
    ]

    AutotestRunner {
        Depends { name: "lirideployment" }

        builtByDefault: project.autotestEnabled
        name: "libliri-autotest"
        arguments: project.autotestArguments
        wrapper: project.autotestWrapper
        environment: {
            var env = base;
            env.push("LD_LIBRARY_PATH=" + FileInfo.joinPaths(qbs.installRoot, qbs.installPrefix, lirideployment.libDir));
            return env;
        }
    }
}
