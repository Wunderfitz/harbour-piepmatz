import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Column {
    id: listsButton
    property bool isActive: false

    anchors.fill: parent
    IconButton {
        id: listsButtonImage
        height: Theme.iconSizeMedium
        width: Theme.iconSizeMedium
        icon.source: listsButton.isActive ? "image://theme/icon-m-note?" + Theme.highlightColor : "image://theme/icon-m-note?" + Theme.primaryColor
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            handleListsClicked();
        }
    }
    Label {
        id: listsButtonText
        text: qsTr("Lists")
        font.pixelSize: Theme.fontSizeTiny * 7 / 8
        color: listsButton.isActive ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: handleListsClicked();
        }
    }
}
