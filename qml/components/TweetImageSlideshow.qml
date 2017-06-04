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

Column {

    property variant tweet;

    width: parent.width
    height: parent.width * 2 / 3

    ListModel {
        id: tweetImageListModel
    }

    SlideshowView {
        id: tweetImageSlideshow
        width: parent.width
        height: parent.height
        model: Functions.getTweetImageModel(tweet.retweeted_status ? tweet.retweeted_status : tweet, tweetImageListModel)
        delegate: Item {
            width: parent.width
            height: parent.height

            Component {
                id: singleImageComponent
                ImagePage {
                    imageUrl: media_url_https
                    imageHeight: sizes.large.h
                    imageWidth: sizes.large.w
                }
            }

            Image {
                id: tweetImage
                source: media_url_https
                width: parent.width
                height: parent.height
                sourceSize.width: parent.width
                sourceSize.height: parent.height
                fillMode: Image.PreserveAspectCrop
                visible: status === Image.Ready ? true : false
                opacity: status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageStack.push( singleImageComponent );
                    }
                }
            }
            ImageProgressIndicator {
                image: tweetImage
                withPercentage: true
            }
        }
    }

}
