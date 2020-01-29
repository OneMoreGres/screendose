#-------------------------------------------------
#
# Project created by QtCreator 2018-09-14T20:46:50
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = screendose
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

ROOT = $$PWD
APP_VERSION = "$$cat($$ROOT/version)"
DEFINES += VERSION_STRING=$$APP_VERSION

QMAKE_TARGET_COMPANY = Gres
QMAKE_TARGET_PRODUCT = ScreenDose
QMAKE_TARGET_COPYRIGHT = Copyright (c) Gres
VERSION = $$APP_VERSION.0


SOURCES += \
    src/main.cpp \
    src/manager.cpp \
    src/desktopoverlay.cpp \
    src/screenoverlay.cpp \
    src/breakschedule.cpp

HEADERS += \
    src/manager.h \
    src/desktopoverlay.h \
    src/screenoverlay.h \
    src/breakschedule.h

OTHER_FILES += \
    share/* \
    version


linux {
    PREFIX = /usr

    target.path = $$PREFIX/bin

    shortcuts.files = $$ROOT/share/screendose.desktop
    shortcuts.path = $$PREFIX/share/applications/
    pixmaps.files += $$ROOT/images/screendose.png
    pixmaps.path = $$PREFIX/share/icons/hicolor/512x512/apps/

    INSTALLS += target shortcuts pixmaps
}
win32 {
    RC_ICONS = $$ROOT/images/icon.ico
    target.path = /
    INSTALLS += target
}
mac {
    ICON = $$ROOT/images/icon.icns
}

RESOURCES += \
    resources.qrc
