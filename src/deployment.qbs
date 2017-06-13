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
        Depends { name: "LiriDBusService" }
        Depends { name: "LiriModels" }
        Depends { name: "LiriNotifications" }
        Depends { name: "liricoreplugin" }
        Depends { name: "lirinotificationsplugin" }
        Depends { name: "liri-notify" }
    }
}
