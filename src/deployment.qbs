import qbs 1.0

Project {
    name: "Deployment"

    InstallPackage {
        name: "libliri-artifacts"
        targetName: name
        builtByDefault: false

        archiver.type: "tar"
        archiver.outputDirectory: project.buildDirectory

        Depends { name: "LiriCore" }
        Depends { name: "LiriCore-cmake" }
        Depends { name: "LiriCore-pkgconfig" }
        Depends { name: "LiriCore-qbs" }
        Depends { name: "LiriDBusService" }
        Depends { name: "LiriDBusService-cmake" }
        Depends { name: "LiriDBusService-pkgconfig" }
        Depends { name: "LiriDBusService-qbs" }
        Depends { name: "LiriModels" }
        Depends { name: "LiriModels-cmake" }
        Depends { name: "LiriModels-pkgconfig" }
        Depends { name: "LiriModels-qbs" }
        Depends { name: "LiriNotifications" }
        Depends { name: "LiriNotifications-cmake" }
        Depends { name: "LiriNotifications-pkgconfig" }
        Depends { name: "LiriNotifications-qbs" }
        Depends { name: "liricoreplugin" }
        Depends { name: "lirinotificationsplugin" }
        Depends { name: "liri-notify" }
    }
}
