#-------------------------------------------------
#
# Project created by QtCreator 2011-01-15T17:54:14
#
#-------------------------------------------------

QT       += testlib \
            gui

TARGET = winfileinfotest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += winfileinfotest.cpp \
    ../../winfileinfo.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../winfileinfo.h

LIBS += -lshlwapi \
        -lgdi32 \
        -lole32 \
        -lmpr
