import QtQuick 2.0
import Sailfish.Silica 1.0

Item {

    id: loadingIndicator

    width: parent.width
    height: parent.height
    Rectangle {
        id: loadingOverlay
        color: "black"
        opacity: 0.8
        width: parent.width
        height: parent.height
        visible: true
    }

    Column {
        width: parent.width
        height: loadingLabel.height + loadingBusyIndicator.height + Theme.paddingMedium
        spacing: Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter

        InfoLabel {
            id: loadingLabel
            text: qsTr("Loading...")
        }

        BusyIndicator {
            id: loadingBusyIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: loadingIndicator.visible
            size: BusyIndicatorSize.Large
        }
    }

}
