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

SOURCES += src/harbour-contrac.cpp \
    src/bleadvertisement.cpp \
    src/bleadvertisementmanager.cpp \
    src/blescanner.cpp \
    src/contactmodel.cpp \
    src/contactstorage.cpp \
    src/contrac.cpp \
    src/controller.cpp \
    src/hkdfsha256.cpp

DISTFILES += qml/harbour-contrac.qml \
    qml/cover/CoverPage.qml \
    qml/pages/About.qml \
    qml/pages/Main.qml \
    qml/components/InfoRow.qml \
    rpm/harbour-contrac.changes.in \
    rpm/harbour-contrac.changes.run.in \
    rpm/harbour-contrac.spec \
    rpm/harbour-contrac.yaml \
    translations/*.ts \
    harbour-contrac.desktop

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

PKGCONFIG += \
    openssl

QT += dbus

HEADERS += \
    src/bleadvertisement.h \
    src/bleadvertisementmanager.h \
    src/bleascanner.h \
    src/contactmodel.h \
    src/contactstorage.h \
    src/contrac.h \
    src/controller.h \
    src/hkdfsha256.h

OTHER_FILES += \
    README.md \
    icons/*.svg
