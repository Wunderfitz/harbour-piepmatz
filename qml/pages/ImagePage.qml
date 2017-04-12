import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: imagePage
    allowedOrientations: Orientation.All

    property string imageUrl;
    property int imageWidth;
    property int imageHeight;
    property real imageSizeFactor : imageWidth / imageHeight;
    property real screenSizeFactor: imagePage.width / imagePage.height;

    SilicaFlickable {
        id: imageContainer
        anchors.fill: parent

        Image {
            id: singleImage
            source: imageUrl
            width: ( imagePage.imageSizeFactor >= imagePage.screenSizeFactor || imagePage.imageSizeFactor >= 1 ) ? imagePage.width : Math.round(imagePage.height / imagePage.imageSizeFactor )
            height: ( imagePage.imageSizeFactor <= imagePage.screenSizeFactor || imagePage.imageSizeFactor <= 1 ) ? imagePage.height : Math.round( imagePage.width / imagePage.imageSizeFactor )
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            visible: status === Image.Ready ? true : false
            opacity: status === Image.Ready ? 1 : 0
            Behavior on opacity { NumberAnimation {} }
        }

        PinchArea {
            anchors.fill: parent
            enabled: singleImage.visible
            pinch {
                target: singleImage
                minimumScale: 1
                maximumScale: 4
            }
            // Pinch-to-zoom doesn't seem to work without this Rectangle...
            Rectangle {
                opacity: 0
                anchors.fill: parent
            }
        }

        ImageProgressIndicator {
            image: singleImage
            withPercentage: true
        }

    }
}
