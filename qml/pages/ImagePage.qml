import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: imagePage
    allowedOrientations: Orientation.All

    property string imageUrl;
    property int imageWidth;
    property int imageHeight;

    SilicaFlickable {
        id: aboutContainer
        anchors.fill: parent
        contentWidth: imageWidth
        contentHeight: imageHeight

        Image {
            width: imageWidth
            height: imageHeight
            source: imageUrl
        }

    }
}
