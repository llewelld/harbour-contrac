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

include(version.pri)

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" \
    "VERSION_MINOR=$$VERSION_MINOR" \
    "VERSION_BUILD=$$VERSION_BUILD" \
    "VERSION=\\\"$$VERSION\\\""

#protobuf build step
PRE_TARGETDEPS += proto/submissionpayload.pb.cc \
    proto/applicationConfiguration.pb.cc

submissionpayload.target = proto/submissionpayload.pb.cc
submissionpayload.path = $$OUT_PWD
submissionpayload.commands = \
    mkdir -p $$OUT_PWD/proto; \
    protoc --proto_path=$$PWD/src --cpp_out=$$OUT_PWD/proto $$PWD/src/submissionpayload.proto

applicationconfig.target = proto/applicationConfiguration.pb.cc
applicationconfig.path = $$OUT_PWD
applicationconfig.commands = \
    mkdir -p $$OUT_PWD/proto; \
    protoc --proto_path=$$PWD/src --cpp_out=$$OUT_PWD/proto $$PWD/src/applicationConfiguration.proto

INCLUDEPATH += $$OUT_PWD/proto

QMAKE_EXTRA_TARGETS += submissionpayload applicationconfig

CONFIG += sailfishapp

HEADERS += \
    proto/submissionpayload.pb.h \
    proto/applicationConfiguration.pb.h \
    src/dbusproxy.h \
    src/contactmodel.h \
    src/download.h \
    src/downloadconfig.h \
    src/imageprovider.h \
    contracd/src/exposuresummary.h \
    contracd/src/exposureinformation.h \
    contracd/src/temporaryexposurekey.h \
    contracd/src/exposureconfiguration.h \
    contracd/src/zipistreambuffer.h \
    src/serveraccess.h \
    src/settings.h \
    src/upload.h

SOURCES += \
    proto/submissionpayload.pb.cc \
    proto/applicationConfiguration.pb.cc \
    src/downloadconfig.cpp \
    src/harbour-contrac.cpp \
    src/dbusproxy.cpp \
    src/contactmodel.cpp \
    src/download.cpp \
    src/imageprovider.cpp \
    contracd/src/exposuresummary.cpp \
    contracd/src/exposureinformation.cpp \
    contracd/src/temporaryexposurekey.cpp \
    contracd/src/exposureconfiguration.cpp \
    contracd/src/zipistreambuffer.cpp \
    src/serveraccess.cpp \
    src/settings.cpp \
    src/upload.cpp

DISTFILES += \
    qml/harbour-contrac.qml \
    qml/cover/CoverPage.qml \
    qml/pages/Main.qml \
    qml/pages/About.qml \
    qml/pages/Settings.qml \
    qml/pages/TeleTAN.qml \
    qml/pages/UploadInfo.qml \
    qml/pages/DownloadInfo.qml \
    qml/components/Dash.qml \
    qml/components/InfoRow.qml \
    qml/components/TanChar.qml \
    qml/images/* \
    src/applicationConfiguration.proto \
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
TRANSLATIONS += translations/harbour-contrac-de.ts

PKGCONFIG += \
    mlite5 \
    openssl \
    libxml-2.0 \
    libcurl \
    protobuf-lite \
    quazip

DEFINES += LINUX

QT += dbus

OTHER_FILES += \
    version.pri \
    src/submissionpayload.proto \
    icons/*.svg
