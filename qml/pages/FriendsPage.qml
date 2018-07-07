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
    id: friendsPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        friendsListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        friendsListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            friendsListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            friendsListView.scrollToBottom();
            event.accepted = true;
        }
    }

    property variant friendsModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!friendsModel) {
            console.log("Loading friends for " + screenName);
            twitterApi.friends(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: friendsNotification
    }

    Connections {
        target: twitterApi
        onFriendsSuccessful: {
            if (!friendsModel) {
                friendsModel = result.users;
                loaded = true;
            }
        }
        onFriendsError: {
            if (!friendsModel) {
                loaded = true;
                friendsNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: friendsContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    friendsPage.loaded = false;
                    friendsPage.friendsModel = null;
                    twitterApi.friends(screenName);
                }
            }
        }

        LoadingIndicator {
            id: friendsLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent

            PageHeader {
                id: friendsHeader
                title: qsTr("%1: Following").arg(userName)
            }

            SilicaListView {
                id: friendsListView


                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - friendsHeader.height

                clip: true

                model: friendsModel
                delegate: User {
                    userModel: modelData
                }
                VerticalScrollDecorator {}
            }

        }

    }
}

