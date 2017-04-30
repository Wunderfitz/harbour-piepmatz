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
    property bool withImages : false;
    property variant attachedImages;

    function getRemainingCharacters(text, configuration) {
        return TwitterText.MAX_LENGTH - TwitterText.twttr.txt.getTweetLength(text, configuration);
    }

    Component.onCompleted: {
        if (replyToStatusId) {
            twitterApi.showStatus(replyToStatusId);
        }
        imagesModel.clearModel();
    }

    Connections {
        target: imagesModel
        onImagesSelected: {
            newTweetPage.withImages = true;
            newTweetPage.attachedImages = imagesModel.getSelectedImages();
        }
    }

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Attach Images")
                onClicked: pageStack.push(attachImagesPage)
            }
            MenuItem {
                text: replyToStatusId ? qsTr("Send Reply") : qsTr("Send Tweet")
                onClicked: {
                    if (replyToStatusId) {
                        if (withImages) {
                            imagesModel.replyToTweetWithSelectedImages(enterTweetTextArea.text, newTweetPage.replyToStatusId);
                        } else {
                            twitterApi.replyToTweet(enterTweetTextArea.text, newTweetPage.replyToStatusId);
                        }
                    } else {
                        if (withImages) {
                            imagesModel.tweetWithSelectedImages(enterTweetTextArea.text);
                        } else {
                            twitterApi.tweet(enterTweetTextArea.text);
                        }
                    }
                    pageStack.pop();
                }
            }
        }

        Column {
            id: column
            width: newTweetPage.width
            spacing: Theme.paddingMedium

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

            SlideshowView {
                id: attachedImagesSlideshow
                width: parent.width * 2 / 3
                height: parent.width * 2 / 3
                anchors.horizontalCenter: parent.horizontalCenter
                model: newTweetPage.attachedImages
                delegate: Item {
                    width: parent.width
                    height: parent.height

                    Image {
                        id: attachedImage
                        source: modelData
                        width: parent.width
                        height: parent.height
                        asynchronous: true
                        sourceSize.width: parent.width
                        sourceSize.height: parent.height
                        fillMode: Image.PreserveAspectCrop
                        visible: status === Image.Ready ? true : false
                        opacity: status === Image.Ready ? 1 : 0
                        Behavior on opacity { NumberAnimation {} }
                    }
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
