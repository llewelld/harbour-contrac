# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-contrac

VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_BUILD = 1

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}-$${VERSION_BUILD}

#protobuf build step
PRE_TARGETDEPS += proto/submissionpayload.pb.cc
protobuf.target = proto/submissionpayload.pb.cc
protobuf.path = $$OUT_PWD
protobuf.commands = \
    mkdir -p $$OUT_PWD/proto; \
    protoc --proto_path=$$PWD/src --cpp_out=$$OUT_PWD/proto $$PWD/src/submissionpayload.proto

INCLUDEPATH += $$OUT_PWD/proto

QMAKE_EXTRA_TARGETS += protobuf

CONFIG += sailfishapp

HEADERS += \
    proto/submissionpayload.pb.h \
    src/dbusproxy.h \
    src/contactmodel.h \
    src/download.h \
    src/s3access.h \
    src/s3/s3.h \
    src/s3/s3internal.h \
    src/s3/s3xml.h \
    contracd/src/exposuresummary.h \
    contracd/src/exposureinformation.h \
    contracd/src/temporaryexposurekey.h \
    contracd/src/exposureconfiguration.h \
    src/upload.h

SOURCES += \
    proto/submissionpayload.pb.cc \
    src/harbour-contrac.cpp \
    src/dbusproxy.cpp \
    src/contactmodel.cpp \
    src/download.cpp \
    src/s3access.cpp \
    src/s3/s3bucket.c \
    src/s3/s3digest.c \
    src/s3/s3ops.c \
    src/s3/s3string.c \
    src/s3/s3xml.c \
    contracd/src/exposuresummary.cpp \
    contracd/src/exposureinformation.cpp \
    contracd/src/temporaryexposurekey.cpp \
    contracd/src/exposureconfiguration.cpp \
    src/upload.cpp

DISTFILES += \
    qml/harbour-contrac.qml \
    qml/cover/CoverPage.qml \
    qml/pages/About.qml \
    qml/pages/Main.qml \
    qml/components/InfoRow.qml \
    translations/*.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
CONFIG += sailfishapp_i18n_idbased
CONFIG += sailfishapp_i18n_unfinished

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-contrac.ts
TRANSLATIONS += translations/harbour-contrac-en.ts
TRANSLATIONS += translations/harbour-contrac-zh_CN.ts

PKGCONFIG += \
    openssl \
    libxml-2.0 \
    libcurl \
    protobuf-lite \

DEFINES += LINUX

QT += dbus

OTHER_FILES += \
    src/submissionpayload.proto \
    icons/*.svg
