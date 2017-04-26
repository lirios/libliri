TARGET = LiriCore
MODULE = LiriCore

QT += core
QT_PRIVATE += core-private
CONFIG += link_pkgconfig
PKGCONFIG += Qt5Xdg

CMAKE_MODULE_NAME = LiriCore
CMAKE_MODULE_NAMESPACE = LiriCore
CONFIG += liri_create_cmake

HEADERS += \
    $$PWD/desktopfileaction.h \
    $$PWD/desktopfile.h \
    $$PWD/kformat.h

SOURCES += \
    $$PWD/desktopfileaction.cpp \
    $$PWD/desktopfile.cpp \
    $$PWD/kformat.cpp \
    $$PWD/kformatprivate.cpp

QMAKE_PKGCONFIG_NAME = LiriCore
QMAKE_PKGCONFIG_DESCRIPTION = Utilities for Liri apps
QMAKE_PKGCONFIG_VERSION = $$LIBLIRI_VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

load(liri_qt_module)
