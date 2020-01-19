/*
    Copyright (C) 2017-20 Sebastian J. Wolf

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
    id: followersPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        followersListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        followersListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            followersListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            followersListView.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            followersListView.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            followersListView.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    property variant followersModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!followersModel) {
            console.log("Loading followers for " + screenName);
            twitterApi.followers(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: followersNotification
    }

    Connections {
        target: twitterApi
        onFollowersSuccessful: {
            if (!followersModel) {
                followersModel = result.users;
                loaded = true;
            }
        }
        onFollowersError: {
            if (!followersModel) {
                loaded = true;
                followersNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: followersContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    followersPage.loaded = false;
                    followersPage.followersModel = null;
                    twitterApi.followers(screenName);
                }
            }
        }

        LoadingIndicator {
            id: followersLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent

            PageHeader {
                id: followersHeader
                title: qsTr("%1: Followers").arg(userName)
            }

            SilicaListView {
                id: followersListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - followersHeader.height

                clip: true

                model: followersModel
                delegate: User {
                    userModel: modelData
                }
                VerticalScrollDecorator {}
            }

        }

    }
}

