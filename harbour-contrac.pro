TEMPLATE = subdirs
SUBDIRS = contrac.pro tests contracd
tests.depends = contracd contrac.pro

DISTFILES += \
    rpm/harbour-contrac.changes.in \
    rpm/harbour-contrac.changes.run.in \
    rpm/harbour-contrac.spec \
    rpm/harbour-contrac.yaml \
    harbour-contrac.desktop

OTHER_FILES += \
    README.md \
