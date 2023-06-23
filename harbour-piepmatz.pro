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

CONFIG += sailfishapp c++11
PKGCONFIG += qt5embedwidget libsailfishkeyprovider

QT += core dbus positioning sql

include(src/o2/o2.pri)
#include(src/wagnis/wagnis.pri)
include(src/QGumboParser/QGumboParser.pri)

SOURCES += src/harbour-piepmatz.cpp \
    src/accountmodel.cpp \
    src/dbusadaptor.cpp \
    src/dbusinterface.cpp \
    src/emojisearchworker.cpp \
    src/twitterapi.cpp \
    src/timelinemodel.cpp \
    src/covermodel.cpp \
    src/searchmodel.cpp \
    src/mentionsmodel.cpp \
    src/imagesmodel.cpp \
    src/imagessearchworker.cpp \
    src/imageresponsehandler.cpp \
    src/directmessagesmodel.cpp \
    src/searchusersmodel.cpp \
    src/imageprocessor.cpp \
    src/trendsmodel.cpp \
    src/locationinformation.cpp \
    src/downloadresponsehandler.cpp \
    src/ownlistsmodel.cpp \
    src/membershiplistsmodel.cpp \
    src/tweetconversationhandler.cpp \
    src/savedsearchesmodel.cpp \
    src/imagemetadataresponsehandler.cpp \
    src/contentextractor.cpp

OTHER_FILES += qml/harbour-piepmatz.qml \
    qml/pages/CoverPage.qml \
    rpm/harbour-piepmatz.changes.in \
    rpm/harbour-piepmatz.spec \
    rpm/harbour-piepmatz.yaml \
    translations/*.ts \
    harbour-piepmatz.desktop \
    db/emojis.db

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-piepmatz-cs.ts \
                translations/harbour-piepmatz-de.ts \
                translations/harbour-piepmatz-es.ts \
                translations/harbour-piepmatz-fi.ts \
                translations/harbour-piepmatz-fr.ts \
                translations/harbour-piepmatz-hi.ts \
                translations/harbour-piepmatz-hu.ts \
                translations/harbour-piepmatz-it.ts \
                translations/harbour-piepmatz-ja.ts \
                translations/harbour-piepmatz-nl.ts \
                translations/harbour-piepmatz-pl.ts \
                translations/harbour-piepmatz-pt_BR.ts \
                translations/harbour-piepmatz-ro.ts \
                translations/harbour-piepmatz-ru.ts \
                translations/harbour-piepmatz-sl.ts \
                translations/harbour-piepmatz-sv.ts \
                translations/harbour-piepmatz-zh_CN.ts

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

172.png.path = $${ICONPATH}/172x172/apps/
172.png.files += icons/172x172/harbour-piepmatz.png

256.png.path = $${ICONPATH}/256x256/apps/
256.png.files += icons/256x256/harbour-piepmatz.png

piepmatz.desktop.path = /usr/share/applications/
piepmatz.desktop.files = harbour-piepmatz.desktop

database.files = db
database.path = /usr/share/$${TARGET}

INSTALLS += 86.png 108.png 128.png 172.png 256.png \
            piepmatz.desktop gui images database

HEADERS += \
    src/accountmodel.h \
    src/dbusadaptor.h \
    src/dbusinterface.h \
    src/emojisearchworker.h \
    src/twitterapi.h \
    src/timelinemodel.h \
    src/covermodel.h \
    src/searchmodel.h \
    src/mentionsmodel.h \
    src/imagesmodel.h \
    src/imagessearchworker.h \
    src/imageresponsehandler.h \
    src/directmessagesmodel.h \
    src/searchusersmodel.h \
    src/imageprocessor.h \
    src/trendsmodel.h \
    src/locationinformation.h \
    src/downloadresponsehandler.h \
    src/ownlistsmodel.h \
    src/membershiplistsmodel.h \
    src/tweetconversationhandler.h \
    src/savedsearchesmodel.h \
    src/imagemetadataresponsehandler.h \
    src/contentextractor.h

DISTFILES += \
    qml/pages/*.qml \
    qml/components/*.qml \
    qml/pages/TweetWebViewPage.qml
