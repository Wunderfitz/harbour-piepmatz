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

            // Pinch-to-zoom doesn't seem to work without this Rectangle...
            Rectangle {
                anchors.fill: parent
                opacity: 0
            }
        }

    }
}
