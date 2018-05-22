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
import QtQuick 2.5
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../js/twitter-text.js" as TwitterText
import "../components"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji

Page {
    id: newTweetPage
    allowedOrientations: Orientation.All

    property variant configuration;
    property real progress : 0;
    property bool valid : true;
    property variant attachmentTweet;
    property string replyToStatusId;
    property variant replyToTweet;
    property bool replyToTweetLoaded;
    property bool withImages : false;
    property variant atMentionProposals;
    property variant attachedImages;
    property variant ipInfo;
    property variant place;
    property bool geoEnabled;

    function parseText(text) {
        var parsingResult = TwitterText.parseTweet(text);
        newTweetPage.valid = parsingResult.valid;
        newTweetPage.progress = parsingResult.permillage > 1000 ? 1 : ( parsingResult.permillage / 1000 );
    }

    function getWordBoundaries(text, cursorPosition) {
        var wordBoundaries = { beginIndex : 0, endIndex : text.length};
        var currentIndex = 0;
        for (currentIndex = (cursorPosition - 1); currentIndex > 0; currentIndex--) {
            if (text.charAt(currentIndex) === ' ') {
                wordBoundaries.beginIndex = currentIndex + 1;
                break;
            }
        }
        for (currentIndex = cursorPosition; currentIndex < text.length; currentIndex++) {
            if (text.charAt(currentIndex) === ' ') {
                wordBoundaries.endIndex = currentIndex;
                break;
            }
        }
        return wordBoundaries;
    }

    function handleAtMentioning(text, cursorPosition) {
        var wordBoundaries = getWordBoundaries(text, cursorPosition);

        var currentWord = text.substring(wordBoundaries.beginIndex, wordBoundaries.endIndex);
        if (currentWord.length > 1 && currentWord.charAt(0) === '@') {
            twitterApi.searchUsers(currentWord);
        } else {
            newTweetPage.atMentionProposals = null;
        }
    }

    function replaceAtMentioning(text, cursorPosition, screenName) {
        var wordBoundaries = getWordBoundaries(text, cursorPosition);
        var newText = text.substring(0, wordBoundaries.beginIndex) + screenName + " " + text.substring(wordBoundaries.endIndex);
        var newIndex = wordBoundaries.beginIndex + screenName.length + 1;
        enterTweetTextArea.text = newText;
        enterTweetTextArea.cursorPosition = newIndex;
        lostFocusTimer.start();
    }

    function getRemainingDescriptionCharacters(text) {
        return 420 - TwitterText.getTweetLength(text);
    }

    Timer {
        id: lostFocusTimer
        interval: 200
        running: false
        repeat: false
        onTriggered: {
            enterTweetTextArea.forceActiveFocus();
        }
    }

    Timer {
        id: updateLocationInformationTimer
        interval: 100
        running: false
        repeat: false
        onTriggered: {
            locationInformation.updateInformation();
        }
    }

    Component.onCompleted: {
        if (locationInformation.hasInformation()) {
            updateLocationInformationTimer.start();
            var currentPosition = locationInformation.getCurrentPosition();
            console.log("Latitude: " + currentPosition.latitude + ", longitude: " + currentPosition.longitude);
            twitterApi.searchGeo(currentPosition.latitude, currentPosition.longitude);
        } else {
            twitterApi.getIpInfo();
        }
        twitterApi.accountSettings();
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

    Connections {
        target: twitterApi
        onSearchUsersSuccessful: {
            newTweetPage.atMentionProposals = result;
        }
        onSearchUsersError: {
            newTweetPage.atMentionProposals = null;
        }
        onGetIpInfoSuccessful: {
            newTweetPage.ipInfo = result;
            var locationArray = newTweetPage.ipInfo.loc.split(",");
            if (locationArray.length === 2) {
                twitterApi.searchGeo(locationArray[0], locationArray[1]);
            }
        }
        onSearchGeoSuccessful: {
            newTweetPage.place = result.result.places[0];
            console.log("New tweet will use: " + newTweetPage.place.full_name + " (" + newTweetPage.place.id + ")");
            attachLocationTextSwitch.text = newTweetPage.place.full_name;
        }
        onAccountSettingsSuccessful: {
            console.log("Geo status: " + result.geo_enabled);
            newTweetPage.geoEnabled = result.geo_enabled;
        }
    }

    Connections {
        target: locationInformation
        onNewPosition: {
            console.log("New GPS coordinates: " + coordinate.latitude + " " + coordinate.longitude);
            twitterApi.searchGeo(coordinate.latitude, coordinate.longitude);
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
                visible: attachmentTweet ? false : true
            }
            MenuItem {
                text: replyToStatusId ? qsTr("Send Reply") : ( attachmentTweet ? qsTr("Send Retweet") : qsTr("Send Tweet") )
                onClicked: {
                    if (replyToStatusId) {
                        if (withImages) {
                            if (attachLocationTextSwitch.checked) {
                                imagesModel.replyToTweetWithSelectedImages(enterTweetTextArea.text, newTweetPage.replyToStatusId, newTweetPage.place.id);
                            } else {
                                imagesModel.replyToTweetWithSelectedImages(enterTweetTextArea.text, newTweetPage.replyToStatusId);
                            }
                        } else {
                            if (attachLocationTextSwitch.checked) {
                                twitterApi.replyToTweet(enterTweetTextArea.text, newTweetPage.replyToStatusId, newTweetPage.place.id);
                            } else {
                                twitterApi.replyToTweet(enterTweetTextArea.text, newTweetPage.replyToStatusId);
                            }
                        }
                    } else {
                        if (withImages) {
                            if (attachLocationTextSwitch.checked) {
                                imagesModel.tweetWithSelectedImages(enterTweetTextArea.text, newTweetPage.place.id);
                            } else {
                                imagesModel.tweetWithSelectedImages(enterTweetTextArea.text);
                            }
                        } else {
                            if ( attachmentTweet ) {
                                if (attachLocationTextSwitch.checked) {
                                    twitterApi.retweetWithComment(enterTweetTextArea.text, Functions.getTweetUrl(attachmentTweet), newTweetPage.place.id);
                                } else {
                                    twitterApi.retweetWithComment(enterTweetTextArea.text, Functions.getTweetUrl(attachmentTweet));
                                }
                            } else {
                                if (attachLocationTextSwitch.checked) {
                                    twitterApi.tweet(enterTweetTextArea.text, newTweetPage.place.id);
                                } else {
                                    twitterApi.tweet(enterTweetTextArea.text);
                                }
                            }
                        }
                    }
                    pageStack.pop();
                }
            }
        }

        Column {
            id: column
            width: newTweetPage.width
            spacing: Theme.paddingSmall

            PageHeader {
                title: replyToStatusId ? qsTr("Reply") : ( attachmentTweet ? qsTr("Retweet") : qsTr("New Tweet") )
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

            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width - 2 * Theme.paddingLarge
                TextArea {
                    id: enterTweetTextArea
                    width: parent.width - remainingCircle.width
                    focus: true
                    font.pixelSize: Theme.fontSizeSmall
                    labelVisible: false;
                    onTextChanged: {
                        parseText(enterTweetTextArea.text);
                        atMentioningTimer.stop();
                        atMentioningTimer.start();
                    }

                    errorHighlight: !newTweetPage.valid
                }

                ProgressCircle {
                    id: remainingCircle
                    progressColor: Theme.highlightColor
                    backgroundColor: Theme.highlightDimmerColor
                    width: Theme.itemSizeExtraSmall
                    height: Theme.itemSizeExtraSmall
                    value: newTweetPage.progress
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                }
            }


            Timer {
                id: atMentioningTimer
                interval: 600
                running: false
                repeat: false
                onTriggered: {
                    handleAtMentioning(enterTweetTextArea.text, enterTweetTextArea.cursorPosition);
                }
            }

            Column {
                id: atMentioningColumn
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
                visible: atMentionProposals ? ( atMentionProposals.length > 0 ? true : false ) : false
                opacity: atMentionProposals ? ( atMentionProposals.length > 0 ? 1 : 0 ) : 0
                Behavior on opacity { NumberAnimation {} }
                spacing: Theme.paddingMedium

                Flickable {
                    width: parent.width
                    height: atMentioningResultRow.height + Theme.paddingSmall
                    anchors.horizontalCenter: parent.horizontalCenter
                    contentWidth: atMentioningResultRow.width
                    clip: true
                    Row {
                        id: atMentioningResultRow
                        spacing: Theme.paddingMedium
                        Repeater {
                            model: atMentionProposals

                            Item {
                                height: singleUserRow.height
                                width: singleUserRow.width

                                Row {
                                    id: singleUserRow
                                    spacing: Theme.paddingSmall

                                    Item {
                                        width: Theme.fontSizeHuge
                                        height: Theme.fontSizeHuge
                                        anchors.verticalCenter: parent.verticalCenter
                                        Image {
                                            id: userPicture
                                            z: 42
                                            source: modelData.profile_image_url_https
                                            width: parent.width
                                            height: parent.height
                                            visible: false
                                        }

                                        Rectangle {
                                            id: userPictureErrorShade
                                            z: 42
                                            width: parent.width
                                            height: parent.height
                                            color: "lightgrey"
                                            visible: false
                                        }

                                        Rectangle {
                                            id: userPictureMask
                                            z: 42
                                            width: parent.width
                                            height: parent.height
                                            color: Theme.primaryColor
                                            radius: parent.width / 7
                                            visible: false
                                        }

                                        OpacityMask {
                                            id: maskedUserPicture
                                            z: 42
                                            source: userPicture.status === Image.Error ? userPictureErrorShade : userPicture
                                            maskSource: userPictureMask
                                            anchors.fill: userPicture
                                            visible: ( userPicture.status === Image.Ready || userPicture.status === Image.Error ) ? true : false
                                            opacity: userPicture.status === Image.Ready ? 1 : ( userPicture.status === Image.Error ? 0.3 : 0 )
                                            Behavior on opacity { NumberAnimation {} }
                                        }
                                    }

                                    Column {
                                        Row {
                                            spacing: Theme.paddingSmall
                                            Text {
                                                text: Emoji.emojify(modelData.name, Theme.fontSizeExtraSmall)
                                                textFormat: Text.StyledText
                                                color: Theme.primaryColor
                                                font.pixelSize: Theme.fontSizeExtraSmall
                                                font.bold: true
                                                elide: Text.ElideRight
                                                maximumLineCount: 1
                                                width: if (modelData.name.length > 20) { atMentioningColumn.width / 2 }
                                                onTruncatedChanged: {
                                                    // There is obviously a bug in QML in truncating text with images.
                                                    // We simply remove Emojis then...
                                                    if (truncated) {
                                                        text = text.replace(/\<img [^>]+\/\>/g, "");
                                                    }
                                                }
                                            }
                                            Image {
                                                source: "image://theme/icon-s-installed"
                                                visible: modelData.verified
                                                width: Theme.fontSizeSmall
                                                height: Theme.fontSizeSmall
                                            }
                                        }
                                        Text {
                                            text: qsTr("@%1").arg(modelData.screen_name)
                                            textFormat: Text.StyledText
                                            color: Theme.primaryColor
                                            font.pixelSize: Theme.fontSizeExtraSmall
                                        }
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        replaceAtMentioning(enterTweetTextArea.text, enterTweetTextArea.cursorPosition, qsTr("@%1").arg(modelData.screen_name));
                                        atMentionProposals = null;
                                    }
                                }
                            }

                        }
                    }
                }

            }

            IconTextSwitch {
                id: attachLocationTextSwitch
                icon.source: "image://theme/icon-m-location"
                visible: ( newTweetPage.place && newTweetPage.geoEnabled ) ? true : false
                description: qsTr("Attach current location to this tweet")
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Component {
                id: attachmentTweetComponent
                EmbeddedTweet {
                    id: attachmentTweetItem
                    tweetModel: attachmentTweet
                    visible: attachmentTweet ? true : false
                    withReferenceUrl: false
                }
            }

            Loader {
                id: attachmentTweetLoader
                active: attachmentTweet ? true : false
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                sourceComponent: attachmentTweetComponent
            }

            Item {
                id: attachedImagesItem
                width: parent.width * 2 / 3
                height: parent.width * 2 / 3
                anchors.horizontalCenter: parent.horizontalCenter
                visible: newTweetPage.attachedImages ? true : false

                SlideshowView {
                    id: attachedImagesSlideshow
                    anchors.fill: parent
                    model: newTweetPage.attachedImages
                    clip: true
                    delegate: Item {
                        width: parent.width
                        height: parent.height

                        Image {
                            id: attachedImage
                            source: modelData
                            width: parent.width
                            height: parent.height
                            autoTransform: true
                            asynchronous: true
                            sourceSize.width: parent.width
                            sourceSize.height: parent.height
                            fillMode: Image.PreserveAspectCrop
                            visible: status === Image.Ready ? true : false
                            opacity: status === Image.Ready ? 1 : 0
                            Behavior on opacity { NumberAnimation {} }
                        }
                    }
                    onCurrentIndexChanged: {
                        imageDescriptionTextField.text = attachedImagesSlideshow.currentIndex >= 0 ? imagesModel.getImageDescription(attachedImages[attachedImagesSlideshow.currentIndex]) : "";
                    }
                }

                Rectangle {
                    id: tweetArrowLeftBackground
                    color: "black"
                    opacity: 0.3
                    height: parent.height
                    width: Theme.fontSizeLarge
                    anchors.left: attachedImagesSlideshow.left
                    visible: attachedImagesSlideshow.count > 1
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            attachedImagesSlideshow.currentIndex = attachedImagesSlideshow.currentIndex - 1;
                        }
                    }
                }

                Image {
                    id: imageArrowLeft
                    width: Theme.fontSizeLarge
                    height: Theme.fontSizeLarge
                    anchors.left: attachedImagesSlideshow.left
                    anchors.verticalCenter: attachedImagesSlideshow.verticalCenter
                    source: "image://theme/icon-m-left"
                    visible: attachedImagesSlideshow.count > 1
                }

                Rectangle {
                    id: tweetArrowRightBackground
                    color: "black"
                    opacity: 0.3
                    height: parent.height
                    width: Theme.fontSizeLarge
                    anchors.right: attachedImagesSlideshow.right
                    visible: attachedImagesSlideshow.count > 1
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            attachedImagesSlideshow.currentIndex = attachedImagesSlideshow.currentIndex + 1;
                        }
                    }
                }

                Image {
                    id: imageArrowRight
                    width: Theme.fontSizeLarge
                    height: Theme.fontSizeLarge
                    anchors.right: attachedImagesSlideshow.right
                    anchors.verticalCenter: attachedImagesSlideshow.verticalCenter
                    source: "image://theme/icon-m-right"
                    visible: attachedImagesSlideshow.count > 1
                }

                Row {
                    id: imagePointRepeaterRow
                    anchors.bottom: attachedImagesSlideshow.bottom
                    anchors.bottomMargin: Theme.fontSizeTiny
                    anchors.horizontalCenter: attachedImagesSlideshow.horizontalCenter
                    spacing: 5
                    visible: attachedImagesSlideshow.count > 1
                    Repeater {
                        id: imagePointRepeater
                        model: attachedImagesSlideshow.count
                        Rectangle {
                            id: imagePoint
                            width: 12
                            height: 12
                            radius: width * 0.5
                            border.color: "black"
                            border.width: 1
                            color: index === attachedImagesSlideshow.currentIndex ? Theme.highlightColor : Theme.primaryColor
                        }
                    }
                }

            }

            Column {
                id: imageDescriptionColumn
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                anchors.horizontalCenter: parent.horizontalCenter
                visible: attachedImagesItem.visible
                TextArea {
                    id: imageDescriptionTextField
                    width: parent.width
                    font.pixelSize: Theme.fontSizeSmall
                    placeholderText: qsTr("Image Description")
                    labelVisible: false
                    errorHighlight: getRemainingDescriptionCharacters(imageDescriptionTextField.text) < 0
                    text: attachedImagesSlideshow.currentIndex >= 0 ? imagesModel.getImageDescription(attachedImages[attachedImagesSlideshow.currentIndex]) : ""
                    onTextChanged: {
                        imagesModel.setImageDescription(attachedImages[attachedImagesSlideshow.currentIndex], text);
                    }
                }
                Text {
                    id: descriptionRemainingCharactersText
                    text: qsTr("%1 characters left").arg(Number(getRemainingDescriptionCharacters(imageDescriptionTextField.text)).toLocaleString(Qt.locale(), "f", 0))
                    color: descriptionRemainingCharactersText.text < 0 ? Theme.highlightColor : Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    font.bold: descriptionRemainingCharactersText.text < 0 ? true : false
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
