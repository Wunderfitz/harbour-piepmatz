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

    SilicaFlickable {
        id: imageFlickable
        anchors.fill: parent
        contentWidth: imagePinchArea.width
        contentHeight: imagePinchArea.height
        clip: true

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
                var oldContentWidth = imagePinchArea.width;
                var oldContentHeight = imagePinchArea.height;
                var newContentWidth = Math.max( singleImage.width * singleImage.scale, imageFlickable.width )
                var newContentHeight = Math.max( singleImage.width * singleImage.scale, imageFlickable.width )

                var newContentX = imageFlickable.contentX + singleImage.scale * ( pinch.previousCenter.x - pinch.center.x );
                var newContentY = imageFlickable.contentY + singleImage.scale * ( pinch.previousCenter.y - pinch.center.y );

                imageFlickable.contentX = newContentX > 0 ? newContentX : 0;
                imageFlickable.contentY = newContentY > 0 ? newContentY : 0;
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
            }
        }
    }

    ImageProgressIndicator {
        image: singleImage
        withPercentage: true
    }

}
