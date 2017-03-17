import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: profilePage
    allowedOrientations: Orientation.All

    property variant profileModel;
    property string profileName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!profileModel) {
            twitterApi.showUser(profilePage.profileName);
        } else {
            loaded = true;
        }
    }

    Connections {
        target: twitterApi
        onShowUserSuccessful: {
            profileModel = result;
            loaded = true;
        }
        onShowUserError: {
            loaded = true;
            // TODO: Finally implement a nice error/notification popup
        }
    }

    LoadingIndicator {
        id: profileLoadingIndicator
        visible: !profilePage.loaded
        Behavior on opacity { NumberAnimation {} }
        opacity: profilePage.loaded ? 0 : 1
        height: parent.height
        width: parent.width
    }

    SilicaFlickable {
        visible: profilePage.loaded
        opacity: profilePage.loaded ? 1 : 0
        Behavior on opacity { NumberAnimation {} }
        id: profileContainer
        anchors.fill: parent

        Loader {
            id: profileLoader
            active: profilePage.loaded
            anchors.fill: parent
            sourceComponent: profileComponent
        }

        Component {
            id: profileComponent
            Profile {
                accountModel: profilePage.profileModel
            }
        }

    }
}
