/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0
import "../js/twitter-text.js" as TwitterText
import "../components"


Page {
    id: newTweetPage
    allowedOrientations: Orientation.All

    property variant configuration;
    property string replyToStatusId;
    property variant replyToTweet;
    property bool replyToTweetLoaded;

    function getRemainingCharacters(text, configuration) {
        return TwitterText.MAX_LENGTH - TwitterText.twttr.txt.getTweetLength(text, configuration);
    }

    Component.onCompleted: {
        if (replyToStatusId) {
            twitterApi.showStatus(replyToStatusId);
        }
    }

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: replyToStatusId ? qsTr("Send Reply") : qsTr("Send Tweet")
                onClicked: {
                    if (replyToStatusId) {
                        twitterApi.replyToTweet(enterTweetTextArea.text, newTweetPage.replyToStatusId);
                    } else {
                        twitterApi.tweet(enterTweetTextArea.text);
                    }
                    pageStack.pop();
                }
            }
        }

        Column {
            id: column
            width: newTweetPage.width

            PageHeader {
                title: replyToStatusId ? qsTr("Reply") : qsTr("New Tweet")
            }

            Connections {
                target: twitterApi
                onShowStatusSuccessful: {
                    if (newTweetPage.replyToStatusId === result.id_str) {
                        newTweetPage.replyToTweet = result;
                        newTweetPage.replyToTweetLoaded = true;
                    }
                }
            }

            Component {
                id: replyToTweetComponent
                Tweet {
                    id: inReplyToTweetItem
                    tweetModel: newTweetPage.replyToTweet
                }
            }

            Loader {
                id: inReplyToTweetLoader
                active: newTweetPage.replyToTweetLoaded
                width: parent.width
                sourceComponent: replyToTweetComponent
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
