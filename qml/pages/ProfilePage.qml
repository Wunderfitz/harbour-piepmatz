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
            console.log("Loading profile for " + profileName);
            twitterApi.showUser(profileName);
        } else {
            loaded = true;
        }
    }

    Notification {
        id: profileNotification
    }

    Connections {
        target: twitterApi
        onShowUserSuccessful: {
            profileModel = result;
            loaded = true;
        }
        onShowUserError: {
            loaded = true;
            profileNotification.show(errorMessage);
        }
        onFollowUserSuccessful: {
            // TODO: Find out, why result shows following: false
            profileNotification.show(qsTr("You follow %1 now.").arg(result.name));
        }
        onFollowUserError: {
            profileNotification.show(errorMessage);
        }
        onUnfollowUserSuccessful: {
            // TODO: Find out, why result shows following: true
            profileNotification.show(qsTr("You don't follow %1 anymore.").arg(result.name));
        }
        onUnfollowUserError: {
            profileNotification.show(errorMessage);
        }
    }

    SilicaFlickable {
        id: profileContainer
        anchors.fill: parent

        Loader {
            id: profilePullDownLoader
            active: profilePage.loaded
            anchors.fill: parent
            sourceComponent: profilePullDownComponent
        }

        Component {
            id: profilePullDownComponent
            Item {
                id: profilePullDownContent
                PullDownMenu {
                    MenuItem {
                        text: profilePage.profileModel.following ? qsTr("Unfollow %1").arg(profilePage.profileModel.name) : qsTr("Follow %1").arg(profilePage.profileModel.name)
                        onClicked: {
                            if (profilePage.accountModel.following) {
                                console.log("Unfollowing user: " + profilePage.accountModel.screen_name);
                                twitterApi.unfollowUser(profilePage.accountModel.screen_name);
                            } else {
                                console.log("Following user: " + profilePage.accountModel.screen_name);
                                twitterApi.followUser(profilePage.accountModel.screen_name);
                            }
                        }
                    }
                }
            }
        }

        LoadingIndicator {
            id: profileLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Loader {
            id: profileLoader
            active: profilePage.loaded
            anchors.fill: parent
            sourceComponent: profileComponent
        }

        Component {
            id: profileComponent
            Item {
                id: profileContent
                Profile {
                    id: otherProfile
                    profileModel: profilePage.profileModel
                }
            }
        }
    }
}
