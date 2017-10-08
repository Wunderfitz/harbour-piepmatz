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
    id: tweetPage
    allowedOrientations: Orientation.All

    property variant tweetModel;

    SilicaFlickable {
        id: tweetContainer
        anchors.fill: parent
        contentHeight: tweetColumn.height

        PullDownMenu {
            id: tweetMenu
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

            PageHeader {
                id: tweetHeader
                title: qsTr("Tweet")
            }

            TweetElement {
                id: tweetElement
                tweetModel: tweetPage.tweetModel
                extendedMode: true
            }

        }

        VerticalScrollDecorator {}
    }
}

