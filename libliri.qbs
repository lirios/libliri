import qbs 1.0

Project {
    name: "LibLiri"

    readonly property string version: "0.9.0"
    readonly property var versionParts: version.split('.').map(function(part) { return parseInt(part); })

    property bool useStaticAnalyzer: false

    property bool autotestEnabled: false
    property stringList autotestArguments: []
    property stringList autotestWrapper: []

    condition: qbs.targetOS.contains("linux")

    minimumQbsVersion: "1.8.0"

    references: [
        "src/deployment.qbs",
        "src/core/core.qbs",
        "src/dbusservice/dbusservice.qbs",
        "src/localdevice/localdevice.qbs",
        "src/models/models.qbs",
        "src/notifications/notifications.qbs",
        "src/imports/imports.qbs",
        "src/tools.qbs",
        "tests/auto/auto.qbs",
    ]
}
