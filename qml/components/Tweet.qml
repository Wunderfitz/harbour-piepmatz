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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

ListItem {

    id: singleTweet

    property variant tweetModel;
    property string userId : accountModel.getCurrentAccount().id_str;
    property bool isRetweetMention : false;

    contentHeight: tweetElement.height
    contentWidth: parent.width

    Connections {
        target: twitterApi
        onDestroySuccessful: {
            if (result.id_str === tweetModel.id_str) {
                var newModel = singleTweet.parent.parent.model;
                newModel.splice(index, 1);
                singleTweet.parent.parent.model = newModel;
            }
        }
    }

    menu: ContextMenu {
        MenuItem {
            visible: ( tweetModel.retweeted_status ? false : ( tweetModel.user.id_str === singleTweet.userId ) )
            onClicked: {
                tweetRemorseItem.execute(singleTweet, qsTr("Deleting tweet"), function() { twitterApi.destroyTweet(tweetModel.id_str); } );
            }
            text: qsTr("Delete Tweet")
        }

        MenuItem {
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/NewTweetPage.qml"), {"replyToStatusId": tweetElement.tweetId});
            }
            text: qsTr("Reply to Tweet")
        }
        MenuItem {
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/NewTweetPage.qml"), {"attachmentTweet": tweetModel});
            }
            text: qsTr("Retweet with Comment")
        }
        MenuItem {
            onClicked: {
                Qt.openUrlExternally(Functions.getTweetUrl(tweetModel));
            }
            text: qsTr("Open in Browser")
        }
        MenuItem {
            onClicked: {
                Clipboard.text = Functions.getTweetUrl(tweetModel);
            }
            text: qsTr("Copy URL to Clipboard")
        }
    }

    RemorseItem {
        id: tweetRemorseItem
    }

    TweetElement {
        id: tweetElement
        tweetModel: singleTweet.tweetModel
        isRetweetMention: singleTweet.isRetweetMention
    }

    onClicked: pageStack.push(Qt.resolvedUrl("../pages/TweetPage.qml"), {"tweetModel": singleTweet.tweetModel });

}
