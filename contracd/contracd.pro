TARGET = contracd

include(../version.pri)

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" \
    "VERSION_MINOR=$$VERSION_MINOR" \
    "VERSION_BUILD=$$VERSION_BUILD" \
    "VERSION=\\\"$$VERSION\\\""

#protobuf build step
PRE_TARGETDEPS += proto/contrac.pb.cc
protobuf.target = proto/contrac.pb.cc
protobuf.path = $$OUT_PWD/contracd
protobuf.commands = \
    mkdir -p $$OUT_PWD/proto; \
    protoc --proto_path=$$PWD/src --cpp_out=$$OUT_PWD/proto $$PWD/src/contrac.proto

INCLUDEPATH += $$OUT_PWD/proto

QMAKE_EXTRA_TARGETS += protobuf

CONFIG += sailfishapp

HEADERS += \
    proto/contrac.pb.h \
    src/blescanner.h \
    src/contracd.h \
    src/dbusinterface.h \
    src/bleadvertisement.h \
    src/bleadvertisementmanager.h \
    src/bloomfilter.h \
    src/contactinterval.h \
    src/contactmatch.h \
    src/contactstorage.h \
    src/contrac.h \
    src/controller.h \
    src/daystorage.h \
    src/diagnosiskey.h \
    src/exposureconfiguration.h \
    src/exposureinformation.h \
    src/exposurenotification.h \
    src/exposurenotification_p.h \
    src/exposuresummary.h \
    src/fnv.h \
    src/hkdfsha256.h \
    src/metadata.h \
    src/providediagnostickeys.h \
    src/rpidataitem.h \
    src/settings.h \
    src/temporaryexposurekey.h \
    src/zipistreambuffer.h

SOURCES += \
    proto/contrac.pb.cc \
    src/contracd.cpp \
    src/dbusinterface.cpp \
    src/bleadvertisement.cpp \
    src/bleadvertisementmanager.cpp \
    src/blescanner.cpp \
    src/bloomfilter.cpp \
    src/contactinterval.cpp \
    src/contactmatch.cpp \
    src/contactstorage.cpp \
    src/contrac.cpp \
    src/controller.cpp \
    src/daystorage.cpp \
    src/diagnosiskey.cpp \
    src/exposureconfiguration.cpp \
    src/exposureinformation.cpp \
    src/exposurenotification.cpp \
    src/exposuresummary.cpp \
    src/fnv.cpp \
    src/hkdfsha256.cpp \
    src/metadata.cpp \
    src/providediagnostickeys.cpp \
    src/rpidataitem.cpp \
    src/settings.cpp \
    src/temporaryexposurekey.cpp \
    src/zipistreambuffer.cpp

DISTFILES += \
    contracd.service \
    contracd.privileges \

PKGCONFIG += \
    openssl \
    protobuf-lite \
    quazip

QT += dbus

OTHER_FILES += \
    src/contrac.proto

service.files = $${TARGET}.service
service.path = /usr/lib/systemd/user/

privilege.files = $${TARGET}.privileges
privilege.path = /usr/share/mapplauncherd/privileges.d/

INSTALLS += \
    target \
    service \
    privilege
