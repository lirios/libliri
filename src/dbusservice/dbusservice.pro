TARGET = LiriDBusService
MODULE = LiriDBusService

QT += core dbus
QT_PRIVATE += core-private

CMAKE_MODULE_NAME = LiriDBusService
CMAKE_MODULE_NAMESPACE = LiriDBusService
CONFIG += liri_create_cmake

DBUS_ADAPTORS += \
    $$PWD/org.freedesktop.Application.xml

HEADERS += \
    $$PWD/dbusservice.h

SOURCES += \
    $$PWD/dbusservice.cpp

INCLUDEPATH += $$PWD

QMAKE_PKGCONFIG_NAME = LiriDBusService
QMAKE_PKGCONFIG_DESCRIPTION = Register applications with D-Bus
QMAKE_PKGCONFIG_VERSION = $$LIBLIRI_VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

load(liri_qt_module)
