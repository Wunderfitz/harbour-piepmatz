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
import QtMultimedia 5.0
import "../components"
import "../js/functions.js" as Functions

Page {
    id: videoPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }

    property variant tweetModel;

    property int videoWidth : Functions.getVideoOriginalWidth(videoPage.tweetModel)
    property int videoHeight : Functions.getVideoOriginalHeight(videoPage.tweetModel)

    property real imageSizeFactor : videoWidth / videoHeight;
    property real screenSizeFactor: videoPage.width / videoPage.height;
    property real sizingFactor    : imageSizeFactor >= screenSizeFactor ? videoPage.width / videoWidth : videoPage.height / videoHeight;

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Download")
                onClicked: {
                    var mediaUrl = Functions.getTweetVideoUrl(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel);
                    twitterApi.downloadFile(mediaUrl, Functions.getMediaFileName(tweetModel, mediaUrl));
                    persistentNotificationItem.enabled = true;
                    persistentNotification.text = qsTr("Downloading...");
                }
            }
        }

        Connections {
            target: twitterApi

            onDownloadSuccessful: {
                persistentNotificationItem.enabled = false;
                videoNotification.show(qsTr("Download of %1 successful.").arg(fileName), filePath);
            }

            onDownloadError: {
                persistentNotificationItem.enabled = false;
                imageNotification.show(errorMessage);
            }

            onDownloadStatus: {
                persistentNotification.text = qsTr("Downloading, %1\% completed...").arg(percentCompleted);
            }
        }

        AppNotification {
            id: videoNotification
        }

        Item {
            id: persistentNotificationItem
            enabled: false
            width: parent.width
            height: persistentNotification.height
            y: parent.height - persistentNotification.height - Theme.paddingSmall
            z: 42

            AppNotificationItem {
                id: persistentNotification
                visible: persistentNotificationItem.enabled
                opacity: persistentNotificationItem.enabled ? 1 : 0
            }
        }

        Item {
            width: videoPage.videoWidth * videoPage.sizingFactor
            height: videoPage.videoHeight * videoPage.sizingFactor
            anchors.centerIn: parent

            TweetVideo {
                tweet: videoPage.tweetModel
                fullscreen: true
            }
        }

    }
}
