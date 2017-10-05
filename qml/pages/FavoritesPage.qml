/*
    Copyright (C) 2017 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: favoritesPage
    allowedOrientations: Orientation.All

    property variant favoritesModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!favoritesModel) {
            console.log("Loading favorites for " + screenName);
            twitterApi.favorites(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: favoritesNotification
    }

    Connections {
        target: twitterApi
        onFavoritesSuccessful: {
            if (!favoritesModel) {
                favoritesModel = result;
                loaded = true;
            }
        }
        onShowUserError: {
            if (!favoritesModel) {
                loaded = true;
                favoritesNotification.show(errorMessage);
            }
        }
    }

    PageHeader {
        id: favoritesHeader
        title: qsTr("%1: Favorites").arg(userName)
    }

    SilicaFlickable {
        id: favoritesContainer
        width: parent.width
        height: parent.height - favoritesHeader.height
        anchors.top: favoritesHeader.bottom

        LoadingIndicator {
            id: favoritesLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        SilicaListView {
            id: favoritesListView

            anchors.fill: parent

            clip: true

            model: favoritesModel
            delegate: Tweet {
                tweetModel: modelData
            }
            VerticalScrollDecorator {}
        }
    }
}

