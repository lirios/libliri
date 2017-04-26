TARGET = lirinotificationsplugin
TARGETPATH = Liri/Notifications
IMPORT_VERSION = 1.0

QT += dbus qml quick LiriNotifications

DBUS_ADAPTORS += \
    $$LIBLIRI_SOURCE_TREE/src/notifications/org.freedesktop.Notifications.xml

SOURCES += \
    $$PWD/notifications.cpp \
    $$PWD/notificationsdaemon.cpp \
    $$PWD/notificationsimage.cpp \
    $$PWD/notificationsimageprovider.cpp \
    $$PWD/plugin.cpp

CONFIG += no_cxx_module
load(liri_qml_plugin)
