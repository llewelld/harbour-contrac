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

CONFIG += sailfishapp

HEADERS += \
    src/dbusproxy.h \
    src/contactmodel.h \
    contracd/src/exposuresummary.h \
    contracd/src/exposureinformation.h \
    contracd/src/temporaryexposurekey.h \
    contracd/src/exposureconfiguration.h

SOURCES += src/harbour-contrac.cpp \
    src/dbusproxy.cpp \
    src/contactmodel.cpp \
    contracd/src/exposuresummary.cpp \
    contracd/src/exposureinformation.cpp \
    contracd/src/temporaryexposurekey.cpp \
    contracd/src/exposureconfiguration.cpp

DISTFILES += qml/harbour-contrac.qml \
    qml/cover/CoverPage.qml \
    qml/pages/About.qml \
    qml/pages/Main.qml \
    qml/components/InfoRow.qml
    translations/*.ts \

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

QT += dbus

OTHER_FILES += \
    icons/*.svg
