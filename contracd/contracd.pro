TARGET = contracd

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 1

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}-$${VERSION_BUILD}

CONFIG += sailfishapp

HEADERS += \
    src/contracd.h \
    src/dbusinterface.h \
    src/bleadvertisement.h \
    src/bleadvertisementmanager.h \
    src/bleascanner.h \
    src/bloomfilter.h \
    src/contactinterval.h \
    src/contactmatch.h \
    src/contactmodel.h \
    src/contactstorage.h \
    src/contrac.h \
    src/contrac.pb.h \
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
    src/rpidataitem.h \
    src/temporaryexposurekey.h \
    src/zipistreambuffer.h

SOURCES += \
    src/contracd.cpp \
    src/dbusinterface.cpp \
    src/bleadvertisement.cpp \
    src/bleadvertisementmanager.cpp \
    src/blescanner.cpp \
    src/bloomfilter.cpp \
    src/contactinterval.cpp \
    src/contactmatch.cpp \
    src/contactmodel.cpp \
    src/contactstorage.cpp \
    src/contrac.cpp \
    src/contrac.pb.cc \
    src/controller.cpp \
    src/daystorage.cpp \
    src/diagnosiskey.cpp \
    src/exposureconfiguration.cpp \
    src/exposureinformation.cpp \
    src/exposurenotification.cpp \
    src/exposuresummary.cpp \
    src/fnv.cpp \
    src/hkdfsha256.cpp \
    src/rpidataitem.cpp \
    src/temporaryexposurekey.cpp \
    src/zipistreambuffer.cpp

DISTFILES += \
    src/contrac.proto \
    contracd.service \
    contracd.privileges \

PKGCONFIG += \
    openssl \
    protobuf \

QT += dbus

LIBS += -lquazip

OTHER_FILES += \

service.files = $${TARGET}.service
service.path = /usr/lib/systemd/user/

privilege.files = $${TARGET}.privileges
privilege.path = /usr/share/mapplauncherd/privileges.d/

INSTALLS += \
    target \
    service \
    privilege \

