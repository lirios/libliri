load(liri_deployment)

TEMPLATE = app
TARGET = liri-notify

QT += core LiriNotifications

SOURCES += \
    $$PWD/main.cpp

target.path = $$LIRI_INSTALL_BINDIR
INSTALLS += target
