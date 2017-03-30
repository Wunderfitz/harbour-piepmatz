/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0
import "../js/twitter-text.js" as TwitterText


Page {
    id: newTweetPage
    allowedOrientations: Orientation.All

    property variant configuration;

    function getRemainingCharacters(text, configuration) {
        return TwitterText.MAX_LENGTH - TwitterText.twttr.txt.getTweetLength(text, configuration);
    }

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
            width: newTweetPage.width

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
                    remainingCharactersText.text = getRemainingCharacters(enterTweetTextArea.text, newTweetPage.configuration);
                }
            }

            Text {
                id: remainingCharactersText
                anchors {
                    right: parent.right
                    rightMargin: Theme.horizontalPageMargin
                }
                color: remainingCharactersText.text < 0 ? Theme.highlightColor : Theme.primaryColor
                font.pixelSize: remainingCharactersText.text < 0 ? Theme.fontSizeSmall : Theme.fontSizeExtraSmall
                font.bold: remainingCharactersText.text < 0 ? true : false
                text: getRemainingCharacters(enterTweetTextArea.text, newTweetPage.configuration)
            }

            VerticalScrollDecorator {}
        }

    }
}
