import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: profilePage
    allowedOrientations: Orientation.All

    property variant profileModel;
    property string profileName;

    Connections {
        target: twitterApi
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

    Notification {
        id: profileNotification
    }

    SilicaFlickable {
        id: profileContainer
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: otherProfile.accountModel.following ? qsTr("Unfollow %1").arg(otherProfile.accountModel.name) : qsTr("Follow %1").arg(otherProfile.accountModel.name)
                onClicked: {
                    if (otherProfile.accountModel.following) {
                        console.log("Unfollowing user: " + otherProfile.accountModel.screen_name);
                        twitterApi.unfollowUser(otherProfile.accountModel.screen_name);
                    } else {
                        console.log("Following user: " + otherProfile.accountModel.screen_name);
                        twitterApi.followUser(otherProfile.accountModel.screen_name);
                    }
                }
            }
        }

        Profile {
            id: otherProfile
            accountModel: profilePage.profileModel
            accountName: profilePage.profileName
        }

    }
}
