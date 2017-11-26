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
import QtMultimedia 5.0
import "../components"
import "../js/functions.js" as Functions

Page {
    id: videoPage
    allowedOrientations: Orientation.All

    property variant tweetModel;

    property int videoWidth : Functions.getVideoOriginalWidth(videoPage.tweetModel)
    property int videoHeight : Functions.getVideoOriginalHeight(videoPage.tweetModel)

    property real imageSizeFactor : videoWidth / videoHeight;
    property real screenSizeFactor: videoPage.width / videoPage.height;
    property real sizingFactor    : imageSizeFactor >= screenSizeFactor ? videoPage.width / videoWidth : videoPage.height / videoHeight;

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
