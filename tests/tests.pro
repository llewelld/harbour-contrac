PACKAGENAME = harbour-contrac

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = $${PACKAGENAME}-tests

target.path = /usr/lib/$${PACKAGENAME}-tests

QMAKE_CXXFLAGS += --coverage
QMAKE_LFLAGS += --coverage

CONFIG += link_prl
DEFINES += UNIT_TEST
QMAKE_EXTRA_TARGETS = check

check.depends = $$TARGET
check.commands = LD_LIBRARY_PATH=../../lib ./$$TARGET

INCLUDEPATH += ../src/

SOURCES += test_storage.cpp
HEADERS += test_storage.h

INSTALLS += target
