PACKAGENAME = harbour-contrac

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = $${PACKAGENAME}-tests

target.path = /usr/lib/$${PACKAGENAME}-tests

QMAKE_CXXFLAGS += --coverage
QMAKE_LFLAGS += --coverage

CONFIG += link_pkgconfig
DEFINES += UNIT_TEST
QMAKE_EXTRA_TARGETS = check

check.depends = $$TARGET
check.commands = LD_LIBRARY_PATH=../../lib ./$$TARGET

INCLUDEPATH += ../src/

PKGCONFIG += \
    openssl

SOURCES += \
    test_tracing.cpp
HEADERS += \
    test_tracing.h

SOURCES += \
    ../src/bloomfilter.cpp \
    ../src/contrac.cpp \
    ../src/fnv.cpp \
    ../src/hkdfsha256.cpp \
    ../src/contactstorage.cpp \
    ../src/rpidataitem.cpp \
    ../src/daystorage.cpp \

HEADERS += \
    ../src/bloomfilter.h \
    ../src/contrac.h \
    ../src/fnv.h \
    ../src/hkdfsha256.h \
    ../src/contactstorage.h \
    ../src/rpidataitem.h \
    ../src/daystorage.h \

INSTALLS += target
