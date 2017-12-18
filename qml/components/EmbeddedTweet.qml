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
import "../pages"
import "../js/functions.js" as Functions

Item {

    id: embeddedTweet

    property variant tweetModel;
    property bool withReferenceUrl : true;

    width: parent.width
    height: embeddedTweetColumn.height

    Column {
        id: embeddedTweetColumn
        spacing: Theme.paddingMedium
        width: parent.width - Theme.paddingMedium
        anchors.right: parent.right
        height: embeddedTweetSeparatorTop.height + tweetUserRow.height + tweetContentText.height + 3 * ( Theme.paddingMedium ) + ( videoLoader.active ? videoLoader.height + Theme.paddingMedium : 0 ) + ( tweetImageSlideshow.visible ? tweetImageSlideshow.height + Theme.paddingMedium : 0 )

        Separator {
            id: embeddedTweetSeparatorTop
            width: parent.width
            color: Theme.primaryColor
            horizontalAlignment: Qt.AlignHCenter
        }

        TweetUser {
            id: tweetUserRow
            tweetUser: tweetModel.retweeted_status ? tweetModel.retweeted_status.user : tweetModel.user
        }

        Text {
            width: parent.width
            id: tweetContentText
            text: Functions.enhanceTweetText(Functions.getRelevantTweet(tweetModel).full_text, Functions.getRelevantTweet(tweetModel).entities, Functions.getRelevantTweet(tweetModel).extended_entities, withReferenceUrl, false)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            textFormat: Text.StyledText
            onLinkActivated: Functions.handleLink(link)
            linkColor: Theme.highlightColor
        }

        TweetImageSlideshow {
            id: tweetImageSlideshow
            visible: Functions.hasImage(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            tweet: tweetModel
        }

        Loader {
            id: videoLoader
            active: Functions.containsVideo(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            width: parent.width
            height: Functions.getVideoHeight(parent.width, tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            sourceComponent: tweetVideoComponent
        }

        Component {
            id: tweetVideoComponent
            TweetVideo {
                tweet: tweetModel
            }
        }

    }

    MouseArea {
        anchors.fill: parent
        onClicked: pageStack.push(Qt.resolvedUrl("../pages/TweetPage.qml"), {"tweetModel": embeddedTweet.tweetModel });
    }

}
