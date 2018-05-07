import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Column {
    id: notificationsButton
    property bool isActive: false

    anchors.fill: parent
    IconButton {
        id: notificationsButtonImage
        icon.source: notificationsButton.isActive ? "image://theme/icon-m-alarm?" + Theme.highlightColor : "image://theme/icon-m-alarm?" + Theme.primaryColor
        height: Theme.iconSizeMedium
        width: Theme.iconSizeMedium
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            handleNotificationsClicked();
        }
    }
    Label {
        id: notificationsButtonText
        text: qsTr("Notifications")
        font.pixelSize: Theme.fontSizeTiny * 7 / 8
        color: notificationsButton.isActive ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: handleNotificationsClicked();
        }
    }
}
