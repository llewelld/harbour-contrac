PACKAGENAME = harbour-contrac

QT += testlib dbus
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
    openssl \
    protobuf \

LIBS += -lquazip

SOURCES += \
    test_tracing.cpp
HEADERS += \
    test_tracing.h

HEADERS += \
    ../src/bleadvertisement.h \
    ../src/bleadvertisementmanager.h \
    ../src/bleascanner.h \
    ../src/bloomfilter.h \
    ../src/contactinterval.h \
    ../src/contactmatch.h \
    ../src/contactmodel.h \
    ../src/contactstorage.h \
    ../src/contrac.h \
    ../src/contrac.pb.h \
    ../src/controller.h \
    ../src/daystorage.h \
    ../src/diagnosiskey.h \
    ../src/exposureconfiguration.h \
    ../src/exposureinformation.h \
    ../src/exposurenotification.h \
    ../src/exposurenotification_p.h \
    ../src/exposuresummary.h \
    ../src/fnv.h \
    ../src/hkdfsha256.h \
    ../src/rpidataitem.h \
    ../src/temporaryexposurekey.h \
    ../src/zipistreambuffer.h \

SOURCES += \
    ../src/bleadvertisement.cpp \
    ../src/bleadvertisementmanager.cpp \
    ../src/blescanner.cpp \
    ../src/bloomfilter.cpp \
    ../src/contactinterval.cpp \
    ../src/contactmatch.cpp \
    ../src/contactmodel.cpp \
    ../src/contactstorage.cpp \
    ../src/contrac.cpp \
    ../src/contrac.pb.cc \
    ../src/controller.cpp \
    ../src/daystorage.cpp \
    ../src/diagnosiskey.cpp \
    ../src/exposureconfiguration.cpp \
    ../src/exposureinformation.cpp \
    ../src/exposurenotification.cpp \
    ../src/exposuresummary.cpp \
    ../src/fnv.cpp \
    ../src/hkdfsha256.cpp \
    ../src/rpidataitem.cpp \
    ../src/temporaryexposurekey.cpp \
    ../src/zipistreambuffer.cpp \

data.files = sample_diagnosis_key_file.zip
data.path = /usr/share/$${PACKAGENAME}-tests

INSTALLS += target data

