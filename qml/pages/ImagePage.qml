import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: imagePage
    allowedOrientations: Orientation.All

    property string imageUrl;
    property int imageWidth;
    property int imageHeight;

    SilicaFlickable {
        id: imageContainer
        anchors.fill: parent

        Image {
            id: singleImage
            source: imageUrl
            width: parent.width
            height: Math.round( parent.width / imageWidth * imageHeight )
            anchors.verticalCenter: parent.verticalCenter
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
}
