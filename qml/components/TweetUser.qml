import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Row {

    property variant tweetUser;

    id: tweetUserRow
    width: parent.width
    spacing: Theme.paddingSmall

    Text {
        id: tweetUserNameText
        font.pixelSize: Theme.fontSizeExtraSmall
        font.bold: true
        color: Theme.primaryColor
        text: tweetUser.name
        elide: Text.ElideRight
        maximumLineCount: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweetUser});
            }
        }
    }

    Image {
        id: tweetUserVerifiedImage
        source: "image://theme/icon-s-installed"
        visible: tweetUser.verified
        width: Theme.fontSizeSmall
        height: Theme.fontSizeSmall
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweetUser});
            }
        }
    }

    Image {
        id: tweetUserProtectedImage
        source: "image://theme/icon-s-secure"
        visible: tweetUser.protected
        width: Theme.fontSizeSmall
        height: Theme.fontSizeSmall
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweetUser});
            }
        }
    }

    Text {
        id: tweetUserHandleText
        font.pixelSize: Theme.fontSizeExtraSmall
        width: parent.width - ( tweetUserVerifiedImage.visible ? tweetUserVerifiedImage.width : 0 ) - ( tweetUserProtectedImage.visible ? tweetUserProtectedImage.width : 0 ) - tweetUserNameText.width - ( 2 * Theme.paddingSmall )
        color: Theme.secondaryColor
        anchors.bottom: tweetUserNameText.bottom
        text: qsTr("@%1").arg(tweetUser.screen_name)
        elide: Text.ElideRight
        maximumLineCount: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweetUser});
            }
        }
    }
}

