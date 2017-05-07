import qbs 1.0

Project {
    name: "Liri Library"

    readonly property string version: "0.9.0"
    property stringList autotestArguments: []
    property stringList autotestWrapper: []

    minimumQbsVersion: "1.6"

    qbsSearchPaths: "qbs-shared"

    references: [
        "src/core/core.qbs",
        "src/dbusservice/dbusservice.qbs",
        "src/notifications/notifications.qbs",
        "src/notify/notify.qbs",
        "src/imports/core/core.qbs",
        "src/imports/notifications/notifications.qbs",
        "tests/auto/core/core.qbs",
    ]

    AutotestRunner {
        name: "libliri-autotest"
        arguments: project.autotestArguments
        wrapper: project.autotestWrapper
    }
}
