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

    property real pinchCenterX;
    property real pinchCenterY;

    property int previousContentX : 0;
    property int previousContentY : 0;

    SilicaFlickable {
        id: imageFlickable
        anchors.fill: parent
        contentWidth: imageContainer.width
        contentHeight: imageContainer.height
        clip: true

        Item {
            id: imageContainer
            width:  Math.max( singleImage.width * singleImage.scale, imageFlickable.width )
            height: Math.max( singleImage.height * singleImage.scale, imageFlickable.height )

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
                    var offsetFactorWidth = pinchCenterX / ( imagePage.width * singleImage.scale );
                    var offsetFactorHeight = pinchCenterY / ( imagePage.height * singleImage.scale );
                    if (( width * scale )  > imagePage.width) {
                        imageFlickable.contentX = (( width * scale ) - imagePage.width ) * offsetFactorWidth;
                    }
                    if (( height * scale )  > imagePage.height) {
                        imageFlickable.contentY = (( height * scale ) - imagePage.height ) * offsetFactorHeight;
                    }
                }
            }

            ImageProgressIndicator {
                image: singleImage
                withPercentage: true
            }
        }

        PinchArea {
            id: imagePinchArea
            anchors.fill: parent

            enabled: singleImage.visible
            pinch {
                target: singleImage
                minimumScale: 1
                maximumScale: 4
            }

            onPinchUpdated: {
                imagePage.pinchCenterX = pinch.center.x;
                imagePage.pinchCenterY = pinch.center.y;
            }

            onPinchStarted: {
                imagePage.previousContentX = imageFlickable.contentX;
                imagePage.previousContentY = imageFlickable.contentY;
            }

            // Pinch-to-zoom doesn't seem to work without this Rectangle...
            Rectangle {
                anchors.fill: parent
                opacity: 0
            }
        }

    }
}
