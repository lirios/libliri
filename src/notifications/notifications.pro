TARGET = LiriNotifications
MODULE = LiriNotifications

QT += core dbus
QT_PRIVATE += core-private

CMAKE_MODULE_NAME = LiriNotifications
CMAKE_MODULE_NAMESPACE = LiriNotifications
CONFIG += liri_create_cmake

DBUS_INTERFACES += \
    $$PWD/org.freedesktop.Notifications.xml

HEADERS += \
    $$PWD/notification.h

SOURCES += \
    $$PWD/notification.cpp

QMAKE_PKGCONFIG_NAME = LiriNotifications
QMAKE_PKGCONFIG_DESCRIPTION = Qt style API for freedesktop.org notifications
QMAKE_PKGCONFIG_VERSION = $$LIBLIRI_VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

load(liri_qt_module)
