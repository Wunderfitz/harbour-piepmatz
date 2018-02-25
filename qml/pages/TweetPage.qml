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
import "../js/functions.js" as Functions

Page {
    id: tweetPage
    allowedOrientations: Orientation.All

    property variant tweetModel;
    property string  myTweetId;
    property variant conversationTweets;
    property string replyToStatusId;
    property variant replyToTweet;
    property bool replyToTweetLoaded;

    Component.onCompleted: {
        if (tweetModel.retweeted_status) {
            if (tweetModel.retweeted_status.in_reply_to_status_id_str) {
                replyToStatusId = tweetModel.retweeted_status.in_reply_to_status_id_str;
            }
        } else {
            if (tweetModel.in_reply_to_status_id_str) {
                replyToStatusId = tweetModel.in_reply_to_status_id_str;
            }
        }
        if (replyToStatusId) {
            twitterApi.showStatus(replyToStatusId);
        }
        myTweetId = Functions.getRelevantTweet(tweetModel).id_str;
        twitterApi.getSingleTweet(myTweetId, Functions.getTweetUrl(tweetModel));
    }

    Connections {
        target: twitterApi
        onShowStatusSuccessful: {
            if (tweetPage.replyToStatusId === result.id_str) {
                tweetPage.replyToTweet = result;
                tweetPage.replyToTweetLoaded = true;
            }
        }
        onTweetConversationReceived: {
            console.log("Conversation received for: " + tweetId + ", my Tweet: " + tweetPage.myTweetId);
            tweetPage.conversationTweets = receivedTweets;
            showConversationButton.visible = true;
        }
    }

    SilicaFlickable {
        id: tweetContainer
        anchors.fill: parent
        contentHeight: tweetColumn.height

        PullDownMenu {
            id: tweetMenu
            visible: tweetColumn.visible
            MenuItem {
                onClicked: {
                    Clipboard.text = Functions.getTweetUrl(tweetModel);
                }
                text: qsTr("Copy URL to Clipboard")
            }
            MenuItem {
                onClicked: {
                    Qt.openUrlExternally(Functions.getTweetUrl(tweetModel));
                }
                text: qsTr("Open in Browser")
            }
            MenuItem {
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/NewTweetPage.qml"), {"attachmentTweet": tweetModel});
                }
                text: qsTr("Retweet with Comment")
            }
            MenuItem {
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/NewTweetPage.qml"), {"replyToStatusId": tweetElement.tweetId});
                }
                text: qsTr("Reply to Tweet")
            }
        }

        Column {
            id: tweetColumn
            width: tweetPage.width
            spacing: Theme.paddingSmall
            Behavior on opacity { NumberAnimation {} }

            PageHeader {
                id: tweetHeader
                title: qsTr("Tweet")
            }

            Component {
                id: replyToTweetComponent
                TweetElement {
                    id: inReplyToTweetItem
                    tweetModel: tweetPage.replyToTweet
                    extendedMode: true
                }
            }

            Loader {
                id: inReplyToTweetLoader
                active: tweetPage.replyToTweetLoaded
                width: parent.width
                sourceComponent: replyToTweetComponent
            }

            TweetElement {
                id: tweetElement
                tweetModel: tweetPage.tweetModel
                extendedMode: true
                withSeparator: false
            }

            Button {
                id: showConversationButton
                Behavior on opacity { NumberAnimation {} }
                text: qsTr("Show Thread")
                preferredWidth: Theme.buttonWidthLarge
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false
                opacity: visible ? 1 : 0
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/TweetConversationPage.qml"), {"conversationTweets": tweetPage.conversationTweets, "sourceTweetId": tweetPage.myTweetId })
                }
            }

        }

        VerticalScrollDecorator { }
    }
}

