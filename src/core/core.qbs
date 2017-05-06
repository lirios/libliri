import qbs 1.0
import qbs.Probes

LiriModule {
    name: "libLiriCore"

    targetName: "LiriCore"
    version: "0.0.0"

    Depends { name: "Qt"; submodules: ["core", "core-private"] }

    // FIXME: This will be handled by PkgConfigProbe with Qbs 1.8
    Probes.PkgConfigProbe {
        id: pkgConfig

        readonly property stringList validDefines: {
            if (pkgConfig.cflags == undefined)
                return [];

            var defines = [];
            for (var i = 0; i < pkgConfig.cflags.length; ++i) {
                var flag = pkgConfig.cflags[i];
                if (flag.startsWith("-D"))
                    defines.push(flag);
            }
            return defines;
        }

        readonly property stringList validCompilerFlags: {
            if (pkgConfig.cflags == undefined)
                return [];

            var compilerFlags = [];
            for (var i = 0; i < pkgConfig.cflags.length; ++i) {
                var flag = pkgConfig.cflags[i];
                if (!flag.startsWith("-I") && !flag.startsWith("-D"))
                    compilerFlags.push(flag);
            }
            return compilerFlags;
        }

        readonly property stringList validIncludePaths: {
            if (pkgConfig.cflags == undefined)
                return [];

            var includePaths = [];
            for (var i = 0; i < pkgConfig.cflags.length; ++i) {
                var flag = pkgConfig.cflags[i];
                if (flag.startsWith("-I"))
                    includePaths.push(flag.slice(2));
            }
            return includePaths;
        }

        readonly property stringList validLibraryPaths: {
            if (pkgConfig.libs == undefined)
                return [];

            var libraryPaths = [];
            for (var i = 0; i < pkgConfig.libs.length; ++i) {
                var flag = pkgConfig.libs[i];
                if (flag.startsWith("-L"))
                    libraryPaths.push(flag.slice(2));
            }
            return libraryPaths;
        }

        readonly property stringList validLibraries: {
            if (pkgConfig.libs == undefined)
                return [];

            var libraries = [];
            for (var i = 0; i < pkgConfig.libs.length; ++i) {
                var flag = pkgConfig.libs[i];
                if (flag.startsWith("-l"))
                    libraries.push(flag.slice(2));
            }
            return libraries;
        }

        name: "Qt5Xdg"
        minVersion: "2.0.0"
    }

    cpp.defines: pkgConfig.validDefines.concat([
        'LIBLIRI_VERSION="' + project.version + '"',
        "QT_BUILD_LIRICORE_LIB"
    ])
    cpp.commonCompilerFlags: base.concat(pkgConfig.validCompilerFlags)
    cpp.includePaths: base.concat(pkgConfig.validIncludePaths)
    cpp.libraryPaths: base.concat(pkgConfig.validLibraryPaths)
    cpp.dynamicLibraries: base.concat(pkgConfig.validLibraries)

    create_headers.headersMap: ({
        "desktopfile.h": "DesktopFile",
        "desktopfileaction.h": "DesktopFileAction",
    })

    create_pkgconfig.name: "Liri Core"
    create_pkgconfig.description: "Utilities for Liri apps"
    create_pkgconfig.version: project.version
    create_pkgconfig.dependencies: ["Qt5Core", "Qt5Xdg"]

    create_cmake.version: project.version
    create_cmake.dependencies: ({
        "Qt5Core": "5.6.0",
        "qt5xdg": "2.0.0",
    })
    create_cmake.linkLibraries: ["Qt5::Core", "Qt5Xdg"]

    files: ["*.cpp"]

    Group {
        name: "Headers"
        files: ["*.h"]
        excludeFiles: ["*_p.h"]
        fileTags: ["public_headers"]
    }

    Group {
        name: "Private Headers"
        files: ["*_p.h"]
        fileTags: ["private_headers"]
    }

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "core-private"] }

        cpp.defines: base.concat(pkgConfig.validDefines)
        cpp.commonCompilerFlags: base.concat(pkgConfig.validCompilerFlags)
        cpp.includePaths: base.concat(pkgConfig.validIncludePaths).concat([product.generatedHeadersDir])
        cpp.libraryPaths: base.concat(pkgConfig.validLibraryPaths)
        cpp.dynamicLibraries: base.concat(pkgConfig.validLibraries)
    }
}
