import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0
import "../components"

Page {
    id: imagePage
    allowedOrientations: Orientation.All

    property string imageUrl;
    property int imageWidth;
    property int imageHeight;

    property real imageSizeFactor : imageWidth / imageHeight;
    property real screenSizeFactor: imagePage.width / imagePage.height;
    property real sizingFactor    : imageSizeFactor >= screenSizeFactor ? imagePage.width / imageWidth : imagePage.height / imageHeight;

    property real previousScale : 1;
    property real centerX;
    property real centerY;
    property real oldCenterX;
    property real oldCenterY;

    SilicaFlickable {
        id: imageFlickable
        anchors.fill: parent
        contentWidth: imagePinchArea.width
        contentHeight: imagePinchArea.height
        clip: true

        transitions: Transition {
            NumberAnimation { properties: "contentX, contentY"; easing.type: Easing.Linear }
        }

        PinchArea {
            id: imagePinchArea
            width:  Math.max( singleImage.width * singleImage.scale, imageFlickable.width )
            height: Math.max( singleImage.height * singleImage.scale, imageFlickable.height )

            enabled: singleImage.visible
            pinch {
                target: singleImage
                minimumScale: 1
                maximumScale: 4
            }

            onPinchUpdated: {
                imagePage.centerX = pinch.center.x;
                imagePage.centerY = pinch.center.y;
            }

            Image {
                id: singleImage
                source: imageUrl
                width: imagePage.imageWidth * imagePage.sizingFactor
                height: imagePage.imageHeight * imagePage.sizingFactor
                anchors.centerIn: parent

                fillMode: Image.PreserveAspectFit

                visible: status === Image.Ready ? true : false
                opacity: status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
                onScaleChanged: {
                    var newWidth = singleImage.width * singleImage.scale;
                    var newHeight = singleImage.height * singleImage.scale;
                    var oldWidth = singleImage.width * imagePage.previousScale;
                    var oldHeight = singleImage.height * imagePage.previousScale;
                    var widthDifference = newWidth - oldWidth;
                    var heightDifference = newHeight - oldHeight;

                    if (oldWidth > imageFlickable.width || newWidth > imageFlickable.width) {
                        var xRatioNew = imagePage.centerX / newWidth;
                        var xRatioOld = imagePage.centerX / oldHeight;
                        imageFlickable.contentX = imageFlickable.contentX + ( xRatioNew * widthDifference );
                    }
                    if (oldHeight > imageFlickable.height || newHeight > imageFlickable.height) {
                        var yRatioNew = imagePage.centerY / newHeight;
                        var yRatioOld = imagePage.centerY / oldHeight;
                        imageFlickable.contentY = imageFlickable.contentY + ( yRatioNew * heightDifference );
                    }


                    imagePage.previousScale = singleImage.scale;
                    imagePage.oldCenterX = imagePage.centerX;
                    imagePage.oldCenterY = imagePage.centerY;
                }
            }
        }
    }

    ImageProgressIndicator {
        image: singleImage
        withPercentage: true
    }

}
