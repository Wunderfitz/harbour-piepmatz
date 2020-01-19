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
    id: userTimelinePage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        userTimelineListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        userTimelineListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            userTimelineListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            userTimelineListView.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            userTimelineListView.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            userTimelineListView.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    property variant userTimelineModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!userTimelineModel) {
            console.log("Loading timeline for " + screenName);
            twitterApi.userTimeline(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: userTimelineNotification
    }

    Connections {
        target: twitterApi
        onUserTimelineSuccessful: {
            if (!userTimelineModel) {
                userTimelineModel = result;
                loaded = true;
            }
        }
        onShowUserError: {
            if (!userTimelineModel) {
                loaded = true;
                userTimelineNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: userTimelineContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    userTimelinePage.loaded = false;
                    userTimelinePage.userTimelineModel = null;
                    twitterApi.userTimeline(screenName);
                }
            }
        }

        LoadingIndicator {
            id: userTimelineLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent
            PageHeader {
                id: userTimelineHeader
                title: userName
            }

            SilicaListView {
                id: userTimelineListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - userTimelineHeader.height

                clip: true

                model: userTimelineModel
                delegate: Tweet {
                    tweetModel: modelData
                }
                VerticalScrollDecorator {}
            }

        }

    }
}

