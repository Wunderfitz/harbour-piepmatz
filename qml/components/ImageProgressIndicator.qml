import QtQuick 2.0
import Sailfish.Silica 1.0

Item {

    property bool withPercentage : false;
    property bool small : false;
    property Image image;

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    width: small ? parent.width / 2 : parent.width
    height: small ? parent.height / 2 : parent.height

    Behavior on opacity { NumberAnimation {} }
    visible: image.status === ( Image.Loading || Image.Error ) ? true : false
    opacity: image.status === ( Image.Loading || Image.Error ) ? 1 : 0
    ProgressCircle {
        id: imageProgressCircle
        width: withPercentage ? parent.height / 2 : parent.height
        height: withPercentage ? parent.height / 2 : parent.height
        value: image.progress
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

    }
    Text {
        id: imageProgressText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: withPercentage ? qsTr("%1 \%").arg(Math.round(image.progress * 100)) : qsTr("%1").arg(Math.round(image.progress * 100))
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: image.status === Image.Loading ? true : false
    }
    Text {
        id: imageErrorText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: qsTr("Error")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: image.status === Image.Error ? true : false
    }

}
