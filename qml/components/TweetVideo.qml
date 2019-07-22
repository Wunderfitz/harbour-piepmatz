/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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
import QtMultimedia 5.0
import "../js/functions.js" as Functions

Item {
    id: tweetVideoComponent

    property variant tweet;
    property bool fullscreen : false;

    width: parent.width
    height: parent.height

    Timer {
        id: screensaverTimer
        interval: 30000
        running: false
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            twitterApi.controlScreenSaver(false);
        }
    }

    function getTweetVideoPlaceholderImageUrl(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                    return tweet.extended_entities.media[i].media_url_https;
                }
            }
        }
        return "";
    }

    function getTimeString(rawSeconds) {
        var minutes = Math.floor( rawSeconds / 60 );
        var seconds = rawSeconds - ( minutes * 60 );

        if ( minutes < 10 ) {
            minutes = "0" + minutes;
        }
        if ( seconds < 10 ) {
            seconds = "0" + seconds;
        }
        return minutes + ":" + seconds;
    }

    function disableScreensaver() {
        screensaverTimer.start();
    }

    function enableScreensaver() {
        screensaverTimer.stop();
        twitterApi.controlScreenSaver(true);
    }

    Image {
        id: placeholderImage
        source: getTweetVideoPlaceholderImageUrl(tweet.retweeted_status ? tweet.retweeted_status : tweet)
        width: parent.width
        height: parent.height
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        fillMode: Image.PreserveAspectCrop
        visible: status === Image.Ready ? true : false
    }

    Rectangle {
        id: placeholderBackground
        color: "black"
        opacity: 0.3
        height: parent.height
        width: parent.width
        visible: playButton.visible
    }

    Row {
        width: parent.width
        height: parent.height
        Item {
            height: parent.height
            width: tweetVideoComponent.fullscreen ? parent.width : ( parent.width / 2 )
            Image {
                id: playButton
                anchors.centerIn: parent
                width: Theme.iconSizeLarge
                height: Theme.iconSizeLarge
                source: "image://theme/icon-l-play"
                visible: placeholderImage.status === Image.Ready ? true : false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        fullscreenItem.visible = false;
                        videoComponentLoader.active = true;
                    }
                }
            }
        }
        Item {
            id: fullscreenItem
            height: parent.height
            width: parent.width / 2
            visible: !tweetVideoComponent.fullscreen
            Image {
                id: fullscreenButton
                anchors.centerIn: parent
                width: Theme.iconSizeLarge
                height: Theme.iconSizeLarge
                source: "../../images/icon-l-fullscreen.png"
                visible: ( placeholderImage.status === Image.Ready && !tweetVideoComponent.fullscreen ) ? true : false
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageStack.push(Qt.resolvedUrl("../pages/VideoPage.qml"), {"tweetModel": tweet.retweeted_status ? tweet.retweeted_status : tweet});
                    }
                }
            }
        }
    }

    Rectangle {
        id: videoErrorShade
        width: parent.width
        height: parent.height
        color: "lightgrey"
        visible: placeholderImage.status === Image.Error ? true : false
        opacity: 0.3
    }

    ImageProgressIndicator {
        image: placeholderImage
        withPercentage: true
    }

    Rectangle {
        id: errorTextOverlay
        color: "black"
        opacity: 0.8
        width: parent.width
        height: parent.height
        visible: false
    }

    Text {
        id: errorText
        visible: false
        width: parent.width
        color: Theme.primaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: Text.AlignHCenter
        anchors {
            verticalCenter: parent.verticalCenter
        }
        wrapMode: Text.Wrap
        text: ""
    }

    Loader {
        id: videoComponentLoader
        active: false
        width: parent.width
        height: Functions.getVideoHeight(parent.width, tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
        sourceComponent: videoComponent
    }

    Component {
        id: videoComponent

        Item {
            width: parent ? parent.width : 0
            height: parent ? parent.height : 0

            Connections {
                target: tweetVideo
                onPlaying: {
                    playButton.visible = false;
                    placeholderImage.visible = false;
                    tweetVideo.visible = true;
                }
            }

            Video {
                id: tweetVideo

                Component.onCompleted: {
                    if (tweetVideo.error === MediaPlayer.NoError) {
                        tweetVideo.play();
                        timeLeftTimer.start();
                    } else {
                        errorText.text = qsTr("Error loading video! " + tweetVideo.errorString)
                        errorTextOverlay.visible = true;
                        errorText.visible = true;
                    }
                }

                onStatusChanged: {
                    if (status == MediaPlayer.NoMedia) {
                        console.log("No Media");
                        videoBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Loading) {
                        console.log("Loading");
                        videoBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Loaded) {
                        console.log("Loaded");
                        videoBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.Buffering) {
                        console.log("Buffering");
                        videoBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Stalled) {
                        console.log("Stalled");
                        videoBusyIndicator.visible = true;
                    }
                    if (status == MediaPlayer.Buffered) {
                        console.log("Buffered");
                        videoBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.EndOfMedia) {
                        console.log("End of Media");
                        videoBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.InvalidMedia) {
                        console.log("Invalid Media");
                        videoBusyIndicator.visible = false;
                    }
                    if (status == MediaPlayer.UnknownStatus) {
                        console.log("Unknown Status");
                        videoBusyIndicator.visible = false;
                    }
                }

                visible: false
                width: parent.width
                height: parent.height
                source: Functions.getTweetVideoUrl(tweet.retweeted_status ? tweet.retweeted_status : tweet)
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (tweetVideo.playbackState === MediaPlayer.PlayingState) {
                            enableScreensaver();
                            tweetVideo.pause();
                            timeLeftItem.visible = true;
                        } else {
                            disableScreensaver();
                            tweetVideo.play();
                            timeLeftTimer.start();
                        }
                    }
                }
                onStopped: {
                    enableScreensaver();
                    tweetVideo.visible = false;
                    placeholderImage.visible = true;
                    playButton.visible = true;
                    videoComponentLoader.active = false;
                    fullscreenItem.visible = !tweetVideoComponent.fullscreen;
                }
            }

            BusyIndicator {
                id: videoBusyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                visible: false
                running: visible
                size: BusyIndicatorSize.Medium
                onVisibleChanged: {
                    if (visible) {
                        enableScreensaver();
                    } else {
                        disableScreensaver();
                    }
                }
            }

            Timer {
                id: timeLeftTimer
                repeat: false
                interval: 2000
                onTriggered: {
                    timeLeftItem.visible = false;
                }
            }

            Item {
                id: timeLeftItem
                width: parent.width
                height: parent.height
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: tweetVideo.visible
                opacity: visible ? 1 : 0
                Behavior on opacity { NumberAnimation {} }

                Rectangle {
                    id: positionTextOverlay
                    color: "black"
                    opacity: 0.3
                    width: parent.width
                    height: parent.height
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: pausedRow.visible
                }

                Row {
                    id: pausedRow
                    width: parent.width
                    height: parent.height - ( tweetVideoSlider.visible ? tweetVideoSlider.height : 0 ) - ( positionText.visible ? positionText.height : 0 )
                    visible: videoComponentLoader.active && tweetVideo.playbackState === MediaPlayer.PausedState
                    Item {
                        height: parent.height
                        width: tweetVideoComponent.fullscreen ? parent.width : ( parent.width / 2 )
                        Image {
                            id: pausedPlayButton
                            anchors.centerIn: parent
                            width: Theme.iconSizeLarge
                            height: Theme.iconSizeLarge
                            source: "image://theme/icon-l-play"
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    disableScreensaver();
                                    tweetVideo.play();
                                    timeLeftTimer.start();
                                }
                            }
                        }
                    }
                    Item {
                        id: pausedFullscreenItem
                        height: parent.height
                        width: parent.width / 2
                        visible: !tweetVideoComponent.fullscreen
                        Image {
                            id: pausedFullscreenButton
                            anchors.centerIn: parent
                            width: Theme.iconSizeLarge
                            height: Theme.iconSizeLarge
                            source: "../../images/icon-l-fullscreen.png"
                            visible: ( videoComponentLoader.active && tweetVideo.playbackState === MediaPlayer.PausedState ) ? true : false
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    pageStack.push(Qt.resolvedUrl("../pages/VideoPage.qml"), {"tweetModel": tweetVideoComponent.tweet});
                                }
                            }
                        }
                    }
                }

                Slider {
                    id: tweetVideoSlider
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: positionText.top
                    minimumValue: 0
                    maximumValue: tweetVideo.duration
                    stepSize: 1
                    value: tweetVideo.position
                    enabled: tweetVideo.seekable
                    visible: (tweetVideo.duration > 0)
                    onReleased: {
                        tweetVideo.seek(Math.floor(value));
                        tweetVideo.play();
                        timeLeftTimer.start();
                    }
                    valueText: getTimeString(Math.round((tweetVideo.duration - tweetVideoSlider.value) / 1000))
                }

                Text {
                    id: positionText
                    visible: tweetVideo.visible && tweetVideo.duration === 0
                    color: Theme.primaryColor
                    font.pixelSize: tweetVideoComponent.fullscreen ? Theme.fontSizeSmall : Theme.fontSizeTiny
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: Theme.paddingSmall
                        horizontalCenter: positionTextOverlay.horizontalCenter
                    }
                    wrapMode: Text.Wrap
                    text: ( tweetVideo.duration - tweetVideo.position ) > 0 ? getTimeString(Math.round((tweetVideo.duration - tweetVideo.position) / 1000)) : "-:-"
                }
            }

        }


    }

}
