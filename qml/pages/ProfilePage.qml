/*
    Copyright (C) 2017 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import "../js/functions.js" as Functions

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
            profilePage.profileName = profilePage.profileModel.screen_name;
            loaded = true;
        }
    }

    AppNotification {
        id: profileNotification
    }

    Connections {
        target: twitterApi
        onShowUserSuccessful: {
            if (profileModel) {
                if (profileName === result.screen_name) {
                    profileModel = result;
                    profileName = result.screen_name;
                    loaded = true;
                }
            } else {
                profileModel = result;
                profileName = result.screen_name;
                loaded = true;
            }
        }
        onShowUserError: {
            if (!profileModel) {
                loaded = true;
                profileNotification.show(errorMessage);
            }
        }
        onFollowUserSuccessful: {
            profileNotification.show(qsTr("You follow %1 now.").arg(result.name));
            profileModel = result;
        }
        onFollowUserError: {
            profileNotification.show(errorMessage);
        }
        onUnfollowUserSuccessful: {
            profileNotification.show(qsTr("You don't follow %1 anymore.").arg(result.name));
            profileModel = result;
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
                        onClicked: {
                            Clipboard.text = Functions.getUserUrl(profilePage.profileModel);
                        }
                        text: qsTr("Copy URL to Clipboard")
                    }
                    MenuItem {
                        text: profilePage.profileModel.following ? qsTr("Unfollow %1").arg(profilePage.profileModel.name) : qsTr("Follow %1").arg(profilePage.profileModel.name)
                        onClicked: {
                            if (profilePage.profileModel.following) {
                                console.log("Unfollowing user: " + profilePage.profileModel.screen_name);
                                twitterApi.unfollowUser(profilePage.profileModel.screen_name);
                            } else {
                                console.log("Following user: " + profilePage.profileModel.screen_name);
                                twitterApi.followUser(profilePage.profileModel.screen_name);
                            }
                        }
                    }
                    MenuItem {
                        text: qsTr("Send Direct Message")
                        onClicked: {
                            var myConversationModel = { user : profileModel, messages: directMessagesModel.getMessagesForUserId(profileModel.id_str) };
                            pageStack.push(Qt.resolvedUrl("../pages/ConversationPage.qml"), { "conversationModel" : myConversationModel, "myUserId": accountModel.getCurrentAccount().id_str });
                        }
                    }
                    MenuItem {
                        text: qsTr("Refresh")
                        onClicked: {
                            profilePage.loaded = false;
                            twitterApi.showUser(profilePage.profileName);
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
                anchors.fill: parent
                Profile {
                    id: otherProfile
                    profileModel: profilePage.profileModel
                }
            }
        }
    }
}
