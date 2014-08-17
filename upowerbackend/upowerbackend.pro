#-------------------------------------------------
#
# Project created by QtCreator 2014-08-15T14:16:40
#
#-------------------------------------------------

QT       += dbus

QT       -= gui

TARGET = upowerbackend
TEMPLATE = lib

DEFINES += UPOWERBACKEND_LIBRARY

SOURCES += upowerbackend.cpp

HEADERS += upowerbackend.h\
        upowerbackend_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
