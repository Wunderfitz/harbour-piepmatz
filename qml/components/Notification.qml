import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0

Item {
    id: notificationItem
    z: 42
    y: -100
    Behavior on y { PropertyAnimation { easing.type: Easing.OutCubic } }
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width - 2 * Theme.horizontalPageMargin

    function show(message) {
        notificationTextItem.text = message;
        notificationTimer.start();
        notificationTextItem.visible = true;
        notificationTextItem.opacity = 1;
        notificationItem.y = notificationItem.parent.height / 2;
    }

    Connections {
        target: notificationTimer
        onTriggered: {
            notificationItem.y = notificationItem.parent.height + 100;
            notificationTextItem.opacity = 0;
            notificationResetter.start();
        }
    }

    Connections {
        target: notificationResetter
        onTriggered: {
            notificationTextItem.visible = false;
            notificationItem.y = -100
        }
    }

    Connections {
        target: notificationItem

    }

    Timer {
        id: notificationTimer
        repeat: false
        interval: 2500
    }

    Timer {
        id: notificationResetter
        repeat: false
        interval: 1000
    }

    NotificationItem {
        id: notificationTextItem
    }

}
