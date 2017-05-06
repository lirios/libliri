import qbs 1.0

Project {
    name: "Liri Library"

    readonly property string version: "0.9.0"

    minimumQbsVersion: "1.6"

    qbsSearchPaths: "qbs-shared"

    references: [
        "src/core/core.qbs",
        "src/dbusservice/dbusservice.qbs",
        "src/notifications/notifications.qbs",
        "src/notify/notify.qbs",
        "src/imports/core/core.qbs",
        "src/imports/notifications/notifications.qbs",
    ]
}
