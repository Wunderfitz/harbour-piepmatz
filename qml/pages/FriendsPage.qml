import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: friendsPage
    allowedOrientations: Orientation.All

    property variant friendsModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!friendsModel) {
            console.log("Loading friends for " + screenName);
            twitterApi.friends(screenName);
        } else {
            loaded = true;
        }
    }

    Notification {
        id: friendsNotification
    }

    Connections {
        target: twitterApi
        onFriendsSuccessful: {
            if (!friendsModel) {
                friendsModel = result.users;
                loaded = true;
            }
        }
        onFriendsError: {
            if (!friendsModel) {
                loaded = true;
                friendsNotification.show(errorMessage);
            }
        }
    }

    PageHeader {
        id: friendsHeader
        title: qsTr("%1: Following").arg(userName)
    }

    SilicaFlickable {
        id: friendsContainer
        width: parent.width
        height: parent.height - friendsHeader.height
        anchors.top: friendsHeader.bottom

        LoadingIndicator {
            id: friendsLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        SilicaListView {
            id: friendsListView

            anchors.fill: parent

            clip: true

            model: friendsModel
            delegate: User {
                userModel: modelData
            }
            VerticalScrollDecorator {}
        }
    }
}

