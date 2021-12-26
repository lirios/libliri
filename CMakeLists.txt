cmake_minimum_required(VERSION 3.10.0)

project("LibLiri"
    VERSION "0.9.0"
    DESCRIPTION "Libraries for Liri apps and desktop environment"
    LANGUAGES CXX C
)

## Shared macros and functions:
if(NOT LIRI_LOCAL_ECM)
    find_package(LiriCMakeShared "2.0.0" REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH "${LCS_MODULE_PATH}")
endif()

## Set minimum versions required:
set(QT_MIN_VERSION "5.10.0")

## Liri specific setup common for all modules:
include(LiriSetup)

## Only build on the appropriate platforms.
if(NOT UNIX OR ANDROID)
    message(NOTICE "Skipping the build as the condition \"UNIX OR ANDROID\" is not met.")
    return()
endif()

## Find Qt 5:
find_package(Qt5 "${QT_MIN_VERSION}"
    CONFIG REQUIRED
    COMPONENTS
        Core
        Xml
        DBus
        Qml
        Quick
        Gui
)

## Add subdirectories:
add_subdirectory(src/core)
add_subdirectory(src/dbusservice)
add_subdirectory(src/imports/core)
add_subdirectory(src/imports/dbusservice)
add_subdirectory(src/imports/device)
add_subdirectory(src/imports/notifications)
add_subdirectory(src/localdevice)
add_subdirectory(src/logind)
add_subdirectory(src/models)
add_subdirectory(src/notifications)
add_subdirectory(src/notify)
add_subdirectory(src/xdg)
if(LIRI_BUILD_TESTING)
    if(TARGET Liri::Models)
        add_subdirectory(tests/auto/core)
    endif()
    if(TARGET Liri::Logind)
        add_subdirectory(tests/auto/logind)
    endif()
    if(TARGET Liri::Xdg)
        add_subdirectory(tests/auto/xdg)
    endif()
endif()
