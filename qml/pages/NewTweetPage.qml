/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0
import "../js/twitter-text.js" as TwitterText


Page {
    id: page
    allowedOrientations: Orientation.All

    property variant configuration;

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Tweet")
                onClicked: {
                    twitterApi.tweet(enterTweetTextArea.text)
                    pageStack.pop()
                }
            }
        }

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("New Tweet")
            }

            TextArea {
                id: enterTweetTextArea
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width - 2 * Theme.paddingLarge
                focus: true
                onTextChanged: {
                    // Text length calculation comes here using TwitterText.twttr.txt.getTweetLength and maybe by detecting an attachment URL, see https://dev.twitter.com/overview/api/upcoming-changes-to-tweets
                    //console.log(TwitterText.twttr.txt.getTweetLength(enterTweetTextArea.text));
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
