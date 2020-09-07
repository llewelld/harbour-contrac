PACKAGENAME = harbour-contrac

include(../config.pri)

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

INCLUDEPATH += ../src/ ./mock/

PKGCONFIG += \
    mlite5 \
    openssl \
    protobuf-lite

LIBS += -lquazip

SOURCES += \
    test_tracing.cpp
HEADERS += \
    test_tracing.h

HEADERS += \
    mock/sailfishapp.h \
    ../src/contactmodel.h \
    ../src/appsettings.h \
    ../src/riskstatus.h \
    ../src/riskscoreclass.h \
    ../contracd/proto/contrac.pb.h \
    ../contracd/src/bleadvertisement.h \
    ../contracd/src/bleadvertisementmanager.h \
    ../contracd/src/blescanner.h \
    ../contracd/src/bloomfilter.h \
    ../contracd/src/contactinterval.h \
    ../contracd/src/contactmatch.h \
    ../contracd/src/contactstorage.h \
    ../contracd/src/contrac.h \
    ../contracd/src/controller.h \
    ../contracd/src/daystorage.h \
    ../contracd/src/diagnosiskey.h \
    ../contracd/src/exposureconfiguration.h \
    ../contracd/src/exposureinformation.h \
    ../contracd/src/exposurenotification.h \
    ../contracd/src/exposurenotification_p.h \
    ../contracd/src/exposuresummary.h \
    ../contracd/src/fnv.h \
    ../contracd/src/hkdfsha256.h \
    ../contracd/src/metadata.h \
    ../contracd/src/providediagnostickeys.h \
    ../contracd/src/rpidataitem.h \
    ../contracd/src/settings.h \
    ../contracd/src/temporaryexposurekey.h \
    ../contracd/src/zipistreambuffer.h

SOURCES += \
    ../src/contactmodel.cpp \
    ../src/appsettings.cpp \
    ../src/riskstatus.cpp \
    ../src/riskscoreclass.cpp \
    ../contracd/proto/contrac.pb.cc \
    ../contracd/src/bleadvertisement.cpp \
    ../contracd/src/bleadvertisementmanager.cpp \
    ../contracd/src/blescanner.cpp \
    ../contracd/src/bloomfilter.cpp \
    ../contracd/src/contactinterval.cpp \
    ../contracd/src/contactmatch.cpp \
    ../contracd/src/contactstorage.cpp \
    ../contracd/src/contrac.cpp \
    ../contracd/src/controller.cpp \
    ../contracd/src/daystorage.cpp \
    ../contracd/src/diagnosiskey.cpp \
    ../contracd/src/exposureconfiguration.cpp \
    ../contracd/src/exposureinformation.cpp \
    ../contracd/src/exposurenotification.cpp \
    ../contracd/src/exposuresummary.cpp \
    ../contracd/src/fnv.cpp \
    ../contracd/src/hkdfsha256.cpp \
    ../contracd/src/metadata.cpp \
    ../contracd/src/providediagnostickeys.cpp \
    ../contracd/src/rpidataitem.cpp \
    ../contracd/src/settings.cpp \
    ../contracd/src/temporaryexposurekey.cpp \
    ../contracd/src/zipistreambuffer.cpp

INCLUDEPATH += $$OUT_PWD/../contracd/proto $$OUT_PWD/../proto

data.files = sample_diagnosis_key_file.zip
data.path = /usr/share/$${PACKAGENAME}-tests

INSTALLS += target data

