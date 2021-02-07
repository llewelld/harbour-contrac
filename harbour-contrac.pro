TEMPLATE = subdirs
SUBDIRS = contrac.pro tests contracd
tests.depends = contracd contrac.pro

DISTFILES += \
    qml/pages/Info.qml \
    rpm/harbour-contrac.changes \
    rpm/harbour-contrac.changes.run.in \
    rpm/harbour-contrac.spec \
    harbour-contrac.desktop

OTHER_FILES += \
    README.md \
    .clang-format
