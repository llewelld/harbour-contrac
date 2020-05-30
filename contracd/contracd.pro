TARGET = contracd

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 1

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}-$${VERSION_BUILD}

CONFIG += sailfishapp

HEADERS += \
    src/contracd.h \
    src/dbusinterface.h

SOURCES += \
    src/contracd.cpp \
    src/dbusinterface.cpp

DISTFILES += \
    contracd.service \
    contracd.privileges \

PKGCONFIG += \
    openssl \
    protobuf-lite \

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

