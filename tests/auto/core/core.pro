TARGET = tst_core

osx:CONFIG -= app_bundle

QT += core qml testlib LiriCore

CONFIG += testcase

SOURCES += \
    $$PWD/tst_qobjectlistmodel.cpp \
    $$LIBLIRI_SOURCE_TREE/src/imports/core/qobjectlistmodel.cpp

INCLUDEPATH += $$LIBLIRI_SOURCE_TREE/src/imports/core
