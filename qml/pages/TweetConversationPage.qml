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
import "../js/functions.js" as Functions

Page {
    id: tweetConversationPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        tweetConversationListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        tweetConversationListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            tweetConversationListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            tweetConversationListView.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            tweetConversationListView.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            tweetConversationListView.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    property variant conversationTweets;
    property variant sourceTweetId;

    SilicaFlickable {
        id: tweetConversationContainer
        width: parent.width
        height: parent.height

        Column {
            anchors.fill: parent

            PageHeader {
                id: tweetConversationHeader
                title: qsTr("Thread")
            }

            SilicaListView {
                id: tweetConversationListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - tweetConversationHeader.height

                clip: true

                model: tweetConversationPage.conversationTweets
                delegate: Tweet {
                    tweetModel: modelData
                    enabled: modelData.id_str !== sourceTweetId
                }

                VerticalScrollDecorator {}
            }

        }

    }

}

