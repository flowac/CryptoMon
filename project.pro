TARGET		= run
DESTDIR		= bin
MOC_DIR		= bin
OBJECTS_DIR	= bin
QT		+= core network widgets

#DEFINES += QT_DEPRECATED_WARNINGS
HEADERS += lib.h
SOURCES += app.cpp chart.cpp log.cpp overlay.cpp web.cpp main.cpp

QMAKE_CXXFLAGS += -std=gnu++11 -Wno-comment

CONFIG -= debug debug_and_release release

