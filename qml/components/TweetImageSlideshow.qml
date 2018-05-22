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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Item {

    id: tweetImageSlideshowItem

    property variant tweet;
    property bool withArrows : false;
    property variant imageModel: Functions.getTweetImageModel(tweet.retweeted_status ? tweet.retweeted_status : tweet, tweetImageListModel);

    width: parent.width
    height: parent.width * 2 / 3

    ListModel {
        id: tweetImageListModel
    }

    SlideshowView {
        id: tweetImageSlideshow
        width: parent.width
        height: parent.height
        itemWidth: width
        itemHeight: height
        clip: true
        model: imageModel
        Behavior on opacity { NumberAnimation {} }
        delegate: Item {
            width: parent.width
            height: parent.height

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
                        pageStack.push(Qt.resolvedUrl("../pages/ImagePage.qml"), {"imageUrl": media_url_https, "imageHeight": sizes.large.h, "imageWidth": sizes.large.w, "tweet": tweet});
                    }
                }
            }

            Rectangle {
                id: tweetImageErrorShade
                width: parent.width
                height: parent.height
                color: "lightgrey"
                visible: tweetImage.status === Image.Error ? true : false
                opacity: 0.3
            }

            ImageProgressIndicator {
                image: tweetImage
                withPercentage: true
            }
        }

        onModelChanged: {
            if ( tweetImageListModel.count > 1 ) {
                tweetImageSlideshowItem.withArrows = true;
            }
        }

    }


    Rectangle {
        id: tweetArrowLeftBackground
        color: "black"
        opacity: 0.3
        height: parent.height
        width: Theme.fontSizeLarge
        anchors.left: tweetImageSlideshow.left
        visible: tweetImageSlideshowItem.withArrows
        MouseArea {
            anchors.fill: parent
            onClicked: {
                tweetImageSlideshow.currentIndex = tweetImageSlideshow.currentIndex - 1;
            }
        }
    }

    Image {
        id: imageArrowLeft
        width: Theme.fontSizeLarge
        height: Theme.fontSizeLarge
        anchors.left: tweetImageSlideshow.left
        anchors.verticalCenter: tweetImageSlideshow.verticalCenter
        source: "image://theme/icon-m-left"
        visible: tweetImageSlideshowItem.withArrows
    }

    Rectangle {
        id: tweetArrowRightBackground
        color: "black"
        opacity: 0.3
        height: parent.height
        width: Theme.fontSizeLarge
        anchors.right: tweetImageSlideshow.right
        visible: tweetImageSlideshowItem.withArrows
        MouseArea {
            anchors.fill: parent
            onClicked: {
                tweetImageSlideshow.currentIndex = tweetImageSlideshow.currentIndex + 1;
            }
        }
    }

    Image {
        id: imageArrowRight
        width: Theme.fontSizeLarge
        height: Theme.fontSizeLarge
        anchors.right: tweetImageSlideshow.right
        anchors.verticalCenter: tweetImageSlideshow.verticalCenter
        source: "image://theme/icon-m-right"
        visible: tweetImageSlideshowItem.withArrows
    }

    Loader {
        id: imageDescriptionLoader
        active: ( tweetImageSlideshow.currentIndex >= 0 ) ? true : false
        width: parent.width
        anchors.bottom: tweetImageSlideshow.bottom
        anchors.bottomMargin: Theme.fontSizeLarge
        sourceComponent: imageDescriptionComponent
    }

    Component {
        id: imageDescriptionComponent
        Item {
            id: imageDescriptionItem
            height: Theme.fontSizeSmall
            width: parent.width
            property variant imageMetadata: imageModel.get(tweetImageSlideshow.currentIndex)
            visible: imageMetadata && imageMetadata.ext_alt_text ? true : false
            Rectangle {
                id: tweetImageDescriptionBackground
                color: "black"
                opacity: 0.3
                height: parent.height
                width: tweetImageSlideshowItem.withArrows ? tweetImageSlideshowItem.width - ( 2 * Theme.fontSizeLarge ) : tweetImageSlideshowItem.width
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                width: parent.width - ( 2 * Theme.paddingMedium )
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                id: tweetImageDescriptionText
                text: imageDescriptionItem.visible ? imageDescriptionItem.imageMetadata.ext_alt_text : ""
                font.pixelSize: Theme.fontSizeTiny
                color: Theme.primaryColor
                elide: Text.ElideRight
                textFormat: Text.StyledText
                maximumLineCount: 1
            }
        }
    }

    Row {
        id: imagePointRepeaterRow
        anchors.bottom: tweetImageSlideshow.bottom
        anchors.bottomMargin: Theme.fontSizeTiny
        anchors.horizontalCenter: tweetImageSlideshow.horizontalCenter
        spacing: 5
        visible: tweetImageListModel.count > 1
        Repeater {
            id: imagePointRepeater
            model: tweetImageSlideshow.count
            Rectangle {
                id: imagePoint
                width: 12
                height: 12
                radius: width * 0.5
                border.color: "black"
                border.width: 1
                color: index === tweetImageSlideshow.currentIndex ? Theme.highlightColor : Theme.primaryColor
            }
        }
    }

}



