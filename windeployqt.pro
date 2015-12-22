TEMPLATE = app
TARGET = windeployqt

CONFIG += console
CONFIG -= app_bundle

QT += core
QT -= gui

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

SOURCES += main.cpp utils.cpp qmlutils.cpp \
           elfreader.cpp options.cpp qtmodules.cpp \
           commandlineparser.cpp \
           deployment.cpp \
           jsonoutput.cpp
HEADERS += utils.h qmlutils.h elfreader.h \
           types.h qtmodules.h options.h \
           commandlineparser.h \
           deployment.h \
           jsonoutput.h

win32: LIBS += -lShlwapi
