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
TARGET = harbour-piepmatz

CONFIG += sailfishapp

QT += core

include(src/o2/o2.pri)

SOURCES += src/harbour-piepmatz.cpp \
    src/accountmodel.cpp \
    src/twitterapi.cpp \
    src/timelinemodel.cpp \
    src/covermodel.cpp \
    src/searchmodel.cpp \
    src/mentionsmodel.cpp

OTHER_FILES += qml/harbour-piepmatz.qml \
    qml/pages/CoverPage.qml \
    rpm/harbour-piepmatz.changes.in \
    rpm/harbour-piepmatz.spec \
    rpm/harbour-piepmatz.yaml \
    translations/*.ts \
    harbour-piepmatz.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-piepmatz-de.ts

gui.files = qml
gui.path = /usr/share/$${TARGET}

images.files = images
images.path = /usr/share/$${TARGET}

ICONPATH = /usr/share/icons/hicolor

86.png.path = $${ICONPATH}/86x86/apps/
86.png.files += icons/86x86/harbour-piepmatz.png

108.png.path = $${ICONPATH}/108x108/apps/
108.png.files += icons/108x108/harbour-piepmatz.png

128.png.path = $${ICONPATH}/128x128/apps/
128.png.files += icons/128x128/harbour-piepmatz.png

256.png.path = $${ICONPATH}/256x256/apps/
256.png.files += icons/256x256/harbour-piepmatz.png

piepmatz.desktop.path = /usr/share/applications/
piepmatz.desktop.files = harbour-piepmatz.desktop

INSTALLS += 86.png 108.png 128.png 256.png \
            piepmatz.desktop gui images

HEADERS += \
    src/accountmodel.h \
    src/twitterapi.h \
    src/timelinemodel.h \
    src/covermodel.h \
    src/searchmodel.h \
    src/mentionsmodel.h
