/*
    Copyright (C) 2017-18 Sebastian J. Wolf

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

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        favoritesListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        favoritesListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            favoritesListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            favoritesListView.scrollToBottom();
            event.accepted = true;
        }
    }

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

    SilicaFlickable {
        id: favoritesContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    favoritesPage.loaded = false;
                    favoritesPage.favoritesModel = null;
                    twitterApi.favorites(screenName);
                }
            }
        }

        LoadingIndicator {
            id: favoritesLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent

            PageHeader {
                id: favoritesHeader
                title: qsTr("%1: Favorites").arg(userName)
            }

            SilicaListView {
                id: favoritesListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - favoritesHeader.height

                clip: true

                model: favoritesModel
                delegate: Tweet {
                    tweetModel: modelData
                }
                VerticalScrollDecorator {}
            }

        }

    }
}

