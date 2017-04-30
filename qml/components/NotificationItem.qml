import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Item {

    id: notificationItem

    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width
    height: notificationText.height + 2 * Theme.paddingMedium
    visible: false
    opacity: 0
    Behavior on opacity { NumberAnimation {} }

    property string text;

    Rectangle {
        id: notificationRectangleBackground
        anchors.fill: parent
        color: "black"
        opacity: 0.6
        radius: parent.width / 15
    }
    Rectangle {
        id: notificationRectangle
        anchors.fill: parent
        color: Theme.highlightColor
        opacity: 0.6
        radius: parent.width / 15
    }

    Text {
        id: notificationText
        color: Theme.primaryColor
        text: notificationItem.text
        font.pixelSize: Theme.fontSizeSmall
        font.bold: true
        width: parent.width - 2 * Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }
}
