import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: followersPage
    allowedOrientations: Orientation.All

    property variant followersModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!followersModel) {
            console.log("Loading followers for " + screenName);
            twitterApi.followers(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: followersNotification
    }

    Connections {
        target: twitterApi
        onFollowersSuccessful: {
            if (!followersModel) {
                followersModel = result.users;
                loaded = true;
            }
        }
        onFollowersError: {
            if (!followersModel) {
                loaded = true;
                followersNotification.show(errorMessage);
            }
        }
    }

    PageHeader {
        id: followersHeader
        title: qsTr("%1: Followers").arg(userName)
    }

    SilicaFlickable {
        id: followersContainer
        width: parent.width
        height: parent.height - followersHeader.height
        anchors.top: followersHeader.bottom

        LoadingIndicator {
            id: followersLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        SilicaListView {
            id: followersListView

            anchors.fill: parent

            clip: true

            model: followersModel
            delegate: User {
                userModel: modelData
            }
            VerticalScrollDecorator {}
        }
    }
}

