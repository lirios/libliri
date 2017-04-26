TARGET = liricoreplugin
TARGETPATH = Liri/Core
IMPORT_VERSION = 1.0

QT += qml quick LiriCore

SOURCES += \
    $$PWD/formatter.cpp \
    $$PWD/qobjectlistmodel.cpp \
    $$PWD/plugin.cpp

CONFIG += no_cxx_module
load(liri_qml_plugin)
