import qbs 1.0

Project {
    name: "Liri Library"

    readonly property string version: "0.9.0"

    property bool autotestEnabled: false
    property stringList autotestArguments: []
    property stringList autotestWrapper: []

    condition: qbs.targetOS.contains("linux")

    minimumQbsVersion: "1.6"

    qbsSearchPaths: "qbs-shared"

    references: [
        "src/core/core.qbs",
        "src/dbusservice/dbusservice.qbs",
        "src/models/models.qbs",
        "src/notifications/notifications.qbs",
        "src/notify/notify.qbs",
        "src/imports/core/core.qbs",
        "src/imports/notifications/notifications.qbs",
        "tests/auto/core/core.qbs",
    ]

    AutotestRunner {
        builtByDefault: autotestEnabled
        name: "libliri-autotest"
        arguments: project.autotestArguments
        wrapper: project.autotestWrapper
    }
}
