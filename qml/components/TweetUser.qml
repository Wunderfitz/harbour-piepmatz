import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Row {

    property variant tweet;

    id: tweetUserRow
    width: parent.width
    spacing: Theme.paddingSmall

    Text {
        id: tweetUserNameText
        font.pixelSize: Theme.fontSizeExtraSmall
        font.bold: true
        color: Theme.primaryColor
        text: tweet.retweeted_status ? tweet.retweeted_status.user.name : tweet.user.name
        elide: Text.ElideRight
        maximumLineCount: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweet.retweeted_status ? tweet.retweeted_status.user : tweet.user});
            }
        }
    }

    Image {
        id: tweetUserVerifiedImage
        source: "image://theme/icon-s-installed"
        visible: tweet.retweeted_status ? tweet.retweeted_status.user.verified : tweet.user.verified
        width: Theme.fontSizeSmall
        height: Theme.fontSizeSmall
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push( tweetUserPageComponent );
            }
        }
    }

    Text {
        id: tweetUserHandleText
        font.pixelSize: Theme.fontSizeExtraSmall
        width: parent.width - tweetUserVerifiedImage.width - tweetUserNameText.width - ( 2 * Theme.paddingSmall )
        color: Theme.secondaryColor
        anchors.bottom: tweetUserNameText.bottom
        text: qsTr("@%1").arg(tweet.retweeted_status ? tweet.retweeted_status.user.screen_name : tweet.user.screen_name)
        elide: Text.ElideRight
        maximumLineCount: 1
        MouseArea {
            anchors.fill: parent
            onClicked: {
                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": tweet.retweeted_status ? tweet.retweeted_status.user : tweet.user});
            }
        }
    }
}

