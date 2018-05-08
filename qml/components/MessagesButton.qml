import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Column {
    id: messagesButton
    property bool isActive: false

    width: parent.width
    IconButton {
        id: messagesButtonImage
        height: Theme.iconSizeMedium
        width: Theme.iconSizeMedium
        icon.source: messagesButton.isActive ? "image://theme/icon-m-mail?" + Theme.highlightColor : "image://theme/icon-m-mail?" + Theme.primaryColor
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            handleMessagesClicked();
        }
    }
    Label {
        id: messagesButtonText
        text: qsTr("Messages")
        font.pixelSize: Theme.fontSizeTiny * 7 / 8
        color: messagesButton.isActive ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: handleMessagesClicked();
        }
    }
}
