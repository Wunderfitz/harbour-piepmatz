/*
    Copyright (C) 2017-18 Sebastian J. Wolf

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
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import Nemo.Notifications 1.0


import "../components"
import "../js/functions.js" as Functions

Page {

    id: overviewPage
    allowedOrientations: Orientation.All

    function hideAccountVerificationColumn() {
        accountVerificationColumn.opacity = 0;
        accountVerificationIndicator.running = false;
        accountVerificationColumn.visible = false;
    }

    function showAccountVerificationColumn() {
        accountVerificationColumn.opacity = 1;
        accountVerificationIndicator.running = true;
        accountVerificationColumn.visible = true;
    }

    function hideBackgroundColumn() {
        backgroundColumn.opacity = 0;
        backgroundColumn.visible = false;
    }

    function showBackgroundColumn() {
        backgroundColumn.opacity = 1;
        backgroundColumn.visible = true;
    }

    function getNavigationRowSize() {
        return Theme.iconSizeMedium + Theme.fontSizeMedium + Theme.paddingMedium;
    }

    function handleHomeClicked() {
        if (overviewPage.activeTabId === "home") {
            homeListView.scrollToTop();
        } else {
            openTab("home");
        }
    }

    function handleNotificationsClicked() {
        if (overviewPage.activeTabId === "notifications") {
            mentionsListView.scrollToTop();
        } else {
            openTab("notifications");
        }
    }

    function handleMessagesClicked() {
        openTab("messages");
    }

    function handleSearchClicked() {
        if (overviewPage.activeTabId === "search") {
            searchResultsListView.scrollToTop();
        } else {
            openTab("search");
        }
    }

    function handleListsClicked() {
        openTab("lists");
    }

    function handleProfileClicked() {
        openTab("profile");
    }

    function getLastUserOfConversation(otherUser, conversation) {
        var lastMessage = conversation[conversation.length - 1].message_create;
        if (lastMessage.sender_id === overviewPage.myUser.id_str) {
            return qsTr("You");
        } else {
            return otherUser.name;
        }
    }

    function getLastMessageOfConversation(conversation) {
        var lastMessage = conversation[conversation.length - 1].message_create;
        return lastMessage.message_data.text;
    }

    function getConversationTimeElapsed(conversation) {
        return Format.formatDate(new Date(parseInt(conversation[conversation.length - 1].created_timestamp)), Formatter.DurationElapsed);
    }

    function updateIpInfo() {
        twitterApi.getIpInfo();
    }

    property string activeTabId: "home";
    property variant myUser;
    property bool initializationCompleted : false;
    property variant configuration;
    property bool tweetInProgress : false;
    property variant ipInfo;

    function openTab(tabId) {

        activeTabId = tabId;

        switch (tabId) {
        case "home":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.highlightColor
            homeButtonText.color = Theme.highlightColor
            homeView.visible = true
            homeView.opacity = 1
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.primaryColor
            listsButtonText.color = Theme.primaryColor
            listsColumn.opacity = 0
            listsColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "notifications":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.highlightColor
            notificationsButtonText.color = Theme.highlightColor
            notificationsColumn.visible = true
            notificationsColumn.opacity = 1
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.primaryColor
            listsButtonText.color = Theme.primaryColor
            listsColumn.opacity = 0
            listsColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "messages":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.highlightColor
            messagesButtonText.color = Theme.highlightColor
            messagesColumn.visible = true
            messagesColumn.opacity = 1
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.primaryColor
            listsButtonText.color = Theme.primaryColor
            listsColumn.opacity = 0
            listsColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "search":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.highlightColor
            searchButtonText.color = Theme.highlightColor
            searchColumn.visible = true
            searchColumn.opacity = 1
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.primaryColor
            listsButtonText.color = Theme.primaryColor
            listsColumn.opacity = 0
            listsColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "lists":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.highlightColor
            listsButtonText.color = Theme.highlightColor
            listsColumn.visible = true
            listsColumn.opacity = 1
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "profile":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            listsButtonImage.icon.source = "image://theme/icon-m-note?" + Theme.primaryColor
            listsButtonText.color = Theme.primaryColor
            listsColumn.opacity = 0
            listsColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.highlightColor
            profileButtonText.color = Theme.highlightColor
            profileView.visible = true
            profileView.opacity = 1
            break;
        default:
            console.log("Some strange navigation happened!")
        }
    }

    Component.onCompleted: {
        accountModel.verifyCredentials();
    }

    Timer {
        id: ipInfoUpdater
        repeat: true
        interval: 1800000
        onTriggered: {
            updateIpInfo();
        }
    }

    Connections {
        target: accountModel
        onCredentialsVerified: {
            if (!overviewPage.initializationCompleted) {
                hideAccountVerificationColumn();
                profileLoader.active = true;
                overviewContainer.visible = true;
                overviewColumn.visible = true;
                overviewColumn.opacity = 1;
                openTab("home");
                twitterApi.helpConfiguration();
                timelineModel.update();
                mentionsModel.update();
                notificationsColumn.updateInProgress = true;
                overviewPage.myUser = accountModel.getCurrentAccount();
                directMessagesModel.setUserId(overviewPage.myUser.id_str);
                directMessagesModel.update();
                ownListsModel.update();
                membershipListsModel.update();
                overviewPage.initializationCompleted = true;
                updateIpInfo();
                ipInfoUpdater.start();
            }
        }
        onVerificationError: {
            if (!overviewPage.initializationCompleted) {
                hideAccountVerificationColumn();
                verificationFailedColumn.visible = true;
                verificationFailedColumn.opacity = 1;
            }
        }
    }

    Connections {
        target: twitterApi
        onTweetError: {
            overviewNotification.show(errorMessage);
            overviewPage.tweetInProgress = false;
        }
        onTweetSuccessful: {
            overviewNotification.show(qsTr("Tweet sent successfully!"));
            accountModel.verifyCredentials();
            overviewPage.tweetInProgress = false;
        }
        onHelpConfigurationSuccessful: {
            overviewPage.configuration = result;
            console.log(overviewPage.configuration.short_url_length_https);
        }
        onHelpConfigurationError: {
            overviewNotification.show(errorMessage);
        }
        onImageUploadStatus: {
            console.log(fileName + " - sent " + bytesSent + " bytes out of " + bytesTotal);
        }
        onGetIpInfoSuccessful: {
            overviewPage.ipInfo = result;
            console.log("Piepmatz knows where he currently is: " + result.city + ", " + result.region + ", " + result.country);
        }
        onDestroySuccessful: {
            Functions.updatePiepmatz();
        }
    }

    Connections {
        target: imagesModel
        onUploadStarted: {
            overviewPage.tweetInProgress = true;
            persistentNotificationItem.enabled = true;
            persistentNotification.text = qsTr("Sending tweet...");
        }

        onUploadCompleted: {
            persistentNotificationItem.enabled = false;
        }

        onUploadFailed: {
            persistentNotificationItem.enabled = false;
            overviewPage.tweetInProgress = false;
            overviewNotification.show(errorMessage);
        }

        onUploadProgress: {
            persistentNotification.text = qsTr("Uploading, %1\% completed...").arg(percentCompleted);
        }
    }

    AppNotification {
        id: overviewNotification
    }

    Item {
        id: persistentNotificationItem
        enabled: false
        width: parent.width
        height: persistentNotification.height
        y: parent.height - getNavigationRowSize() - persistentNotification.height - Theme.paddingSmall
        z: 42

        AppNotificationItem {
            id: persistentNotification
            visible: persistentNotificationItem.enabled
            opacity: persistentNotificationItem.enabled ? 1 : 0
        }
    }



    Column {
        y: ( parent.height - ( accountVerificationImage.height + accountVerificationIndicator.height + accountVerificationLabel.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: accountVerificationColumn
        spacing: Theme.paddingSmall
        Behavior on opacity { NumberAnimation {} }

        Image {
            id: accountVerificationImage
            source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: accountVerificationLabel
            text: qsTr("Saying hello to Twitter...")
        }

        BusyIndicator {
            id: accountVerificationIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: true
            size: BusyIndicatorSize.Large
        }

    }

    Column {
        y: ( parent.height - ( verificationFailedImage.height + verificationFailedInfoLabel.height + verifyAgainButton.height + reauthenticateButton.height + ( 4 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: verificationFailedColumn
        spacing: Theme.paddingSmall
        Behavior on opacity { NumberAnimation {} }
        opacity: 0
        visible: false

        Image {
            id: verificationFailedImage
            source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: verificationFailedInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Piepmatz could not log you in!")
        }

        Button {
            id: verifyAgainButton
            text: qsTr("Try again")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                verificationFailedColumn.opacity = 0;
                verificationFailedColumn.visible = false;
                showAccountVerificationColumn()
                accountModel.verifyCredentials()
            }
        }

        Button {
            id: reauthenticateButton
            text: qsTr("Authenticate")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                verificationFailedColumn.opacity = 0;
                verificationFailedColumn.visible = false;
                pageStack.clear()
                pageStack.push(welcomePage)
            }
        }

    }

    SilicaFlickable {
        id: overviewContainer
        anchors.fill: parent
        contentHeight: parent.height
        visible: false

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(settingsPage)
            }
            MenuItem {
                text: qsTr("New Tweet")
                enabled: overviewPage.tweetInProgress ? false : true
                onClicked: pageStack.push(newTweetPage, {"configuration": overviewPage.configuration})
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: Functions.updatePiepmatz()
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: Functions.updatePiepmatz()
            }
            MenuItem {
                text: qsTr("New Tweet")
                enabled: overviewPage.tweetInProgress ? false : true
                onClicked: pageStack.push(newTweetPage, {"configuration": overviewPage.configuration})
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(settingsPage)
            }
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
        }

        Column {
            id: overviewColumn
            opacity: 0
            visible: false
            Behavior on opacity { NumberAnimation {} }
            width: parent.width
            height: parent.height

            Item {
                id: profileView
                opacity: 0
                Behavior on opacity { NumberAnimation {} }
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()

                Loader {
                    id: profileLoader
                    active: false
                    width: parent.width
                    height: parent.height
                    sourceComponent: profileComponent
                }

                Component {
                    id: profileComponent
                    Profile {
                        id: ownProfile
                        profileModel: accountModel.getCurrentAccount()

                        Connections {
                            target: accountModel
                            onCredentialsVerified: {
                                console.log("Updating profile page...");
                                ownProfile.profileModel = accountModel.getCurrentAccount();
                            }
                        }
                    }
                }
            }

            Item {
                id: homeView
                width: parent.width
                height: parent.height - getNavigationRowSize()
                visible: false
                opacity: 0
                Behavior on opacity { NumberAnimation {} }

                property bool loaded : false;
                property bool reloading: false;

                Connections {
                    target: timelineModel
                    onHomeTimelineStartUpdate: {
                        homeListView.currentIndex = -1;
                        homeListView.footer = homeTimelineFooterComponent;
                    }

                    onHomeTimelineUpdated: {
                        homeListView.currentIndex = modelIndex;
                        homeView.loaded = true;
                        homeView.reloading = false;
                    }
                    onHomeTimelineError: {
                        homeView.loaded = true;
                        homeView.reloading = false;
                        overviewNotification.show(errorMessage);
                    }
                    onHomeTimelineEndReached: {
                        homeListView.footer = null;
                        overviewNotification.show(qsTr("No tweets found. Follow more people to get their tweets in your timeline!"));
                    }
                }

                Column {
                    width: parent.width
                    height: homeProgressLabel.height + homeProgressIndicator.height + Theme.paddingSmall
                    visible: !homeView.loaded
                    opacity: homeView.loaded ? 0 : 1
                    id: homeProgressColumn
                    spacing: Theme.paddingSmall
                    Behavior on opacity { NumberAnimation {} }
                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: homeProgressLabel
                        text: qsTr("Loading timeline...")
                    }

                    BusyIndicator {
                        id: homeProgressIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: !homeView.loaded
                        size: BusyIndicatorSize.Large
                    }
                }

                SilicaListView {
                    id: homeListView
                    opacity: homeView.loaded ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
                    visible: homeView.loaded
                    width: parent.width
                    height: parent.height
                    clip: true

                    model: timelineModel

                    delegate: Tweet {
                        tweetModel: display
                        userId: overviewPage.myUser.id_str
                    }

                    onMovementEnded: {
                        timelineModel.setCurrentTweetId(homeListView.itemAt(homeListView.contentX, ( homeListView.contentY + Math.round(overviewPage.height / 2))).tweetModel.id_str);
                    }

                    onQuickScrollAnimatingChanged: {
                        if (!quickScrollAnimating) {
                            timelineModel.setCurrentTweetId(homeListView.itemAt(homeListView.contentX, ( homeListView.contentY + Math.round(overviewPage.height / 2))).tweetModel.id_str);
                        }
                    }

                    footer: homeTimelineFooterComponent;

                    VerticalScrollDecorator {}
                }

                Component {
                    id: homeTimelineFooterComponent
                    Item {
                        id: homeTimelineLoadMoreRow
                        width: overviewPage.width
                        height: homeTimelineLoadMoreButton.height + ( 2 * Theme.paddingLarge )
                        Button {
                            id: homeTimelineLoadMoreButton
                            Behavior on opacity { NumberAnimation {} }
                            text: qsTr("Load more tweets")
                            preferredWidth: Theme.buttonWidthLarge
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            opacity: visible ? 1 : 0
                            onClicked: {
                                console.log("Loading more tweets for timeline");
                                timelineModel.loadMore();
                                homeTimelineLoadMoreBusyIndicator.visible = true;
                                homeTimelineLoadMoreButton.visible = false;
                            }
                        }
                        BusyIndicator {
                            id: homeTimelineLoadMoreBusyIndicator
                            Behavior on opacity { NumberAnimation {} }
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                            visible: false
                            opacity: visible ? 1 : 0
                            running: visible
                            size: BusyIndicatorSize.Medium
                        }
                        Connections {
                            target: timelineModel
                            onHomeTimelineUpdated: {
                                homeTimelineLoadMoreBusyIndicator.visible = false;
                                homeTimelineLoadMoreButton.visible = true;
                            }
                        }
                    }
                }


                LoadingIndicator {
                    id: homeLoadingIndicator
                    visible: homeView.reloading
                    Behavior on opacity { NumberAnimation {} }
                    opacity: homeView.reloading ? 1 : 0
                    height: parent.height
                    width: parent.width
                }
            }


            Item {
                id: notificationsColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }

                property bool updateInProgress : false;

                Notification {
                    id: mentionsNotification
                    appName: "Piepmatz"
                    appIcon: "/usr/share/icons/hicolor/256x256/apps/harbour-piepmatz.png"
                }
                Notification {
                    id: retweetsNotification
                    appName: "Piepmatz"
                    appIcon: "/usr/share/icons/hicolor/256x256/apps/harbour-piepmatz.png"
                }
                Notification {
                    id: followersNotification
                    appName: "Piepmatz"
                    appIcon: "/usr/share/icons/hicolor/256x256/apps/harbour-piepmatz.png"
                }

                Connections {

                    target: mentionsModel
                    onUpdateMentionsFinished: {
                        notificationsColumn.updateInProgress = false;
                    }
                    onUpdateMentionsError: {
                        notificationsColumn.updateInProgress = false;
                        overviewNotification.show(errorMessage);
                    }
                    onNewMentionsFound: {
                        if (newMentions > 1) {
                            mentionsNotification.summary = qsTr("New Mentions");
                            mentionsNotification.body = qsTr("You have been mentioned %1 times!").arg(newMentions);
                            mentionsNotification.previewSummary = qsTr("New Mentions");
                            mentionsNotification.previewBody = qsTr("You have been mentioned %1 times!").arg(newMentions);
                        } else {
                            mentionsNotification.summary = qsTr("New Mention");
                            mentionsNotification.body = qsTr("You have been mentioned!");
                            mentionsNotification.previewSummary = qsTr("New Mention");
                            mentionsNotification.previewBody = qsTr("You have been mentioned!");
                        }
                        mentionsNotification.replacesId = 0;
                        mentionsNotification.publish();
                    }
                    onNewRetweetsFound: {
                        if (newRetweets > 1) {
                            retweetsNotification.summary = qsTr("New Retweets");
                            retweetsNotification.body = qsTr("You have been retweeted %1 times!").arg(newRetweets);
                            retweetsNotification.previewSummary = qsTr("New Retweets");
                            retweetsNotification.previewBody = qsTr("You have been retweeted %1 times!").arg(newRetweets);
                        } else {
                            retweetsNotification.summary = qsTr("New Retweets");
                            retweetsNotification.body = qsTr("You have been retweeted!");
                            retweetsNotification.previewSummary = qsTr("New Retweets");
                            retweetsNotification.previewBody = qsTr("You have been retweeted!");
                        }
                        retweetsNotification.replacesId = 0;
                        retweetsNotification.publish();
                    }
                    onNewFollowersFound: {
                        if (newFollowers > 1) {
                            followersNotification.summary = qsTr("New Followers");
                            followersNotification.body = qsTr("You have %1 new followers!").arg(newFollowers);
                            followersNotification.previewSummary = qsTr("New Followers");
                            followersNotification.previewBody = qsTr("You have %1 new followers!").arg(newFollowers);
                        } else if (newFollowers === 1) {
                            followersNotification.summary = qsTr("New Follower");
                            followersNotification.body = qsTr("You have a new follower!");
                            followersNotification.previewSummary = qsTr("New Follower");
                            followersNotification.previewBody = qsTr("You have a new follower!");
                        }
                        followersNotification.replacesId = 0;
                        followersNotification.publish();
                    }
                }

                Component {
                    id: componentMentionsTweet

                    Tweet {
                        tweetModel: mentionsData
                        userId: overviewPage.myUser.id_str
                        isRetweetMention: isRetweet
                    }
                }

                Component {
                    id: componentMentionsUser

                    User {
                        userModel: mentionsData
                    }
                }

                SilicaListView {
                    anchors {
                        fill: parent
                    }
                    id: mentionsListView

                    clip: true

                    model: mentionsModel
                    delegate: Component {
                        Loader {
                            width: mentionsListView.width
                            property variant mentionsData: display
                            property bool isRetweet : display.retweeted_status ? (( display.retweeted_status.user.id_str === overviewPage.myUser.id_str ) ? true : false ) : false

                            sourceComponent: if (display.followed_at) {
                                                 mentionsData.description = qsTr("follows you now!");
                                                 return componentMentionsUser;
                                             } else {
                                                 return componentMentionsTweet;
                                             }
                        }
                    }

                    VerticalScrollDecorator {}
                }

                Column {
                    anchors {
                        fill: parent
                    }

                    id: mentionsUpdateInProgressColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: notificationsColumn.updateInProgress ? 1 : 0
                    visible: notificationsColumn.updateInProgress ? true : false

                    LoadingIndicator {
                        id: mentionsLoadingIndicator
                        visible: notificationsColumn.updateInProgress
                        Behavior on opacity { NumberAnimation {} }
                        opacity: notificationsColumn.updateInProgress ? 1 : 0
                        height: parent.height
                        width: parent.width
                    }
                }

            }

            Item {
                id: messagesColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }

                property bool updateInProgress : false;

                Notification {
                    id: messagesNotification
                    appName: "Piepmatz"
                    appIcon: "/usr/share/icons/hicolor/256x256/apps/harbour-piepmatz.png"
                }

                Connections {

                    target: directMessagesModel

                    onUpdateMessagesStarted: {
                        messagesColumn.updateInProgress = true;
                    }

                    onUpdateMessagesFinished: {
                        messagesColumn.updateInProgress = false;
                    }

                    onUpdateMessagesError: {
                        messagesColumn.updateInProgress = false;
                        overviewNotification.show(errorMessage);
                    }
                    onNewMessagesFound: {
                        messagesNotification.summary = qsTr("New Messages");
                        messagesNotification.body = qsTr("You have new direct messages!");
                        messagesNotification.previewSummary = qsTr("New Messages");
                        messagesNotification.previewBody = qsTr("You have new messages!");
                        messagesNotification.replacesId = 0;
                        messagesNotification.publish();
                    }
                }

                SilicaListView {
                    anchors {
                        fill: parent
                    }
                    id: messagesListView

                    clip: true

                    model: directMessagesModel
                    delegate: ListItem {

                        id: messageContactItem

                        contentHeight: messageContactRow.height + messageContactSeparator.height + 2 * Theme.paddingMedium
                        contentWidth: parent.width

                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/ConversationPage.qml"), { "conversationModel" : display, "myUserId": overviewPage.myUser.id_str, "configuration": overviewPage.configuration });
                        }

                        Column {
                            id: messageContactColumn
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            spacing: Theme.paddingSmall
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                verticalCenter: parent.verticalCenter
                            }

                            Row {
                                id: messageContactRow
                                width: parent.width
                                spacing: Theme.paddingMedium

                                Column {
                                    id: messageContactPictureColumn
                                    width: parent.width / 6
                                    height: parent.width / 6
                                    spacing: Theme.paddingSmall

                                    Item {
                                        id: messageContactPictureItem
                                        width: parent.width
                                        height: parent.width

                                        Image {
                                            id: messageContactPicture
                                            z: 42
                                            source: Functions.findBiggerImage(display.user.profile_image_url_https)
                                            width: parent.width
                                            height: parent.height
                                            sourceSize {
                                                width: parent.width
                                                height: parent.height
                                            }
                                            visible: false
                                        }

                                        Rectangle {
                                            id: messageContactPictureMask
                                            z: 42
                                            width: parent.width
                                            height: parent.height
                                            color: Theme.primaryColor
                                            radius: parent.width / 7
                                            visible: false
                                        }

                                        OpacityMask {
                                            id: maskedMessageContactPicture
                                            z: 42
                                            source: messageContactPicture
                                            maskSource: messageContactPictureMask
                                            anchors.fill: messageContactPicture
                                            visible: messageContactPicture.status === Image.Ready ? true : false
                                            opacity: messageContactPicture.status === Image.Ready ? 1 : 0
                                            Behavior on opacity { NumberAnimation {} }
                                        }

                                        ImageProgressIndicator {
                                            image: messageContactPicture
                                            withPercentage: false
                                        }

                                    }
                                }

                                Column {
                                    id: messageContactContentColumn
                                    width: parent.width * 5 / 6 - Theme.horizontalPageMargin

                                    spacing: Theme.paddingSmall

                                    Text {
                                        id: messageContactNameText
                                        text: display.user.name
                                        font.pixelSize: Theme.fontSizeMedium
                                        color: Theme.primaryColor
                                        elide: Text.ElideRight
                                        width: parent.width
                                    }

                                    Row {
                                        id: messageContactLastMessageRow
                                        width: parent.width
                                        spacing: Theme.paddingMedium
                                        Text {
                                            id: messageContactLastUserText
                                            text: getLastUserOfConversation(display.user, display.messages)
                                            font.pixelSize: Theme.fontSizeExtraSmall
                                            color: Theme.highlightColor
                                        }
                                        Text {
                                            id: messageContactLastMessageText
                                            text: getLastMessageOfConversation(display.messages)
                                            font.pixelSize: Theme.fontSizeExtraSmall
                                            color: Theme.primaryColor
                                            width: parent.width - Theme.paddingMedium - messageContactLastUserText.width
                                            elide: Text.ElideRight
                                            textFormat: Text.PlainText
                                        }
                                    }

                                    Timer {
                                        id: messageContactTimeUpdater
                                        interval: 60000
                                        running: true
                                        repeat: true
                                        onTriggered: {
                                            messageContactTimeElapsedText.text = getConversationTimeElapsed(display.messages);
                                        }
                                    }

                                    Text {
                                        id: messageContactTimeElapsedText
                                        text: getConversationTimeElapsed(display.messages)
                                        font.pixelSize: Theme.fontSizeTiny
                                        color: Theme.primaryColor
                                    }
                                }
                            }

                        }

                        Separator {
                            id: messageContactSeparator

                            anchors {
                                top: messageContactColumn.bottom
                                topMargin: Theme.paddingMedium
                            }

                            width: parent.width
                            color: Theme.primaryColor
                            horizontalAlignment: Qt.AlignHCenter
                        }

                    }


                    VerticalScrollDecorator {}
                }

                Column {
                    anchors {
                        fill: parent
                    }

                    id: messagesUpdateInProgressColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: messagesColumn.updateInProgress ? 1 : 0
                    visible: messagesColumn.updateInProgress ? true : false

                    LoadingIndicator {
                        id: messagesLoadingIndicator
                        visible: messagesColumn.updateInProgress
                        Behavior on opacity { NumberAnimation {} }
                        opacity: messagesColumn.updateInProgress ? 1 : 0
                        height: parent.height
                        width: parent.width
                    }
                }

                Column {
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width

                    id: messagesNoResultsColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: messagesListView.count === 0 ? 1 : 0
                    visible: messagesListView.count === 0 ? true : false

                    Connections {
                        target: accountModel
                        onImageStyleChanged: {
                            messagesNoResultsImage.source = "../../images/" + accountModel.getImagePath() + "piepmatz.svg";
                        }
                    }

                    Image {
                        id: messagesNoResultsImage
                        source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        fillMode: Image.PreserveAspectFit
                        width: 1/3 * parent.width
                    }

                    InfoLabel {
                        id: messagesNoResultsText
                        text: qsTr("No direct messages in the last 30 days")
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeLarge
                        width: parent.width - 2 * Theme.horizontalPageMargin
                    }
                }


            }

            Item {
                id: searchColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }

                property bool tweetSearchInProgress : false;
                property bool usersSearchInProgress : false;
                property bool tweetSearchInTransition : false;
                property bool usersSearchInTransition : false;

                property bool usersSearchSelected : false;

                Connections {
                    target: searchModel
                    onSearchFinished: {
                        searchColumn.tweetSearchInProgress = false;
                        searchColumn.tweetSearchInTransition = false;
                    }
                    onSearchError: {
                        searchColumn.tweetSearchInProgress = false;
                        searchColumn.tweetSearchInTransition = false;
                        overviewNotification.show(errorMessage);
                    }
                }

                Connections {
                    target: searchUsersModel
                    onSearchFinished: {
                        searchColumn.usersSearchInProgress = false;
                        searchColumn.usersSearchInTransition = false;
                    }
                    onSearchError: {
                        searchColumn.usersSearchInProgress = false;
                        searchColumn.usersSearchInTransition = false;
                        overviewNotification.show(errorMessage);
                    }
                }

                Connections {
                    target: trendsModel
                    onTrendsRetrieved: {
                        trendsPlaceText.text = qsTr("Trends for %1").arg(place);
                    }
                }

                Timer {
                    id: searchTimer
                    interval: 1000
                    running: false
                    repeat: false
                    onTriggered: {
                        searchColumn.tweetSearchInProgress = true;
                        searchColumn.usersSearchInProgress = true;
                        searchModel.search(searchField.text);
                        searchUsersModel.search(searchField.text);
                    }
                }

                SearchField {
                    id: searchField
                    width: parent.width
                    placeholderText: qsTr("Search on Twitter...")
                    anchors {
                        top: parent.top
                    }

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: focus = false

                    onTextChanged: {
                        searchColumn.tweetSearchInTransition = true;
                        searchColumn.usersSearchInTransition = true;
                        searchTimer.stop();
                        searchTimer.start();
                    }
                }

                Row {
                    id: searchTypeRow
                    width: parent.width
                    height: Theme.fontSizeLarge + Theme.paddingMedium
                    anchors.top: searchField.bottom
                    anchors.topMargin: Theme.paddingMedium
                    opacity: ( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress || (searchResultsListView.count === 0 && usersSearchResultsListView.count === 0)) ? 0 : 1
                    visible: ( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress || (searchResultsListView.count === 0 && usersSearchResultsListView.count === 0)) ? false : true
                    Behavior on opacity { NumberAnimation {} }
                    Text {
                        id: searchTypeTweets
                        width: ( parent.width / 2 )
                        font.pixelSize: Theme.fontSizeMedium
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        color: searchColumn.usersSearchSelected ? Theme.primaryColor : Theme.highlightColor
                        textFormat: Text.PlainText
                        anchors.top: parent.top
                        text: qsTr("Tweets")
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (searchColumn.usersSearchSelected) {
                                    searchColumn.usersSearchSelected = false;
                                }
                            }
                        }
                    }
                    Separator {
                        width: Theme.fontSizeMedium
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                        anchors.top: parent.top
                        anchors.topMargin: Theme.paddingSmall
                        transform: Rotation { angle: 90 }
                    }
                    Text {
                        id: searchTypeUsers
                        width: ( parent.width / 2 ) - ( 2 * Theme.fontSizeMedium )
                        font.pixelSize: Theme.fontSizeMedium
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        color: searchColumn.usersSearchSelected ? Theme.highlightColor : Theme.primaryColor
                        textFormat: Text.PlainText
                        anchors.top: parent.top
                        text: qsTr("Users")
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!searchColumn.usersSearchSelected) {
                                    searchColumn.usersSearchSelected = true;
                                }
                            }
                        }
                    }
                }

                Row {
                    id: trendsPlaceRow
                    width: parent.width
                    height: Theme.fontSizeLarge + Theme.paddingMedium
                    anchors.top: searchField.bottom
                    anchors.topMargin: Theme.paddingMedium
                    opacity: ( searchField.text === "" && trendsListView.count !== 0 && !( searchColumn.tweetSearchInTransition || searchColumn.usersSearchInTransition ) ) ? 1 : 0
                    visible: ( searchField.text === "" && trendsListView.count !== 0 && !( searchColumn.tweetSearchInTransition || searchColumn.usersSearchInTransition ) ) ? true : false
                    Behavior on opacity { NumberAnimation {} }
                    Text {
                        id: trendsPlaceText
                        width: parent.width
                        font.pixelSize: Theme.fontSizeMedium
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        color: Theme.highlightColor
                        textFormat: Text.PlainText
                        anchors.top: parent.top
                    }
                }

                SilicaListView {
                    anchors {
                        top: searchTypeRow.bottom
                    }
                    id: searchResultsListView
                    width: parent.width
                    height: parent.height - searchField.height - searchTypeRow.height - Theme.paddingMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: ( !searchColumn.tweetSearchInProgress && !searchColumn.usersSearchSelected ) ? 1 : 0
                    visible: ( !searchColumn.tweetSearchInProgress && !searchColumn.usersSearchSelected ) ? true : false
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: searchModel
                    delegate: Tweet {
                        tweetModel: display
                        userId: overviewPage.myUser.id_str
                    }
                    VerticalScrollDecorator {}
                }

                SilicaListView {
                    anchors {
                        top: searchTypeRow.bottom
                    }
                    id: usersSearchResultsListView
                    width: parent.width
                    height: parent.height - searchField.height - searchTypeRow.height - Theme.paddingMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: ( !searchColumn.usersSearchInProgress && searchColumn.usersSearchSelected ) ? 1 : 0
                    visible: ( !searchColumn.usersSearchInProgress && searchColumn.usersSearchSelected ) ? true : false
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: searchUsersModel
                    delegate: User {
                        userModel: display
                    }
                    VerticalScrollDecorator {}
                }

                Column {
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width

                    id: searchInProgressColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: ( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress ) ? 1 : 0
                    visible: ( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress ) ? true : false

                    BusyIndicator {
                        id: searchInProgressIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: ( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress )
                        size: BusyIndicatorSize.Medium
                    }

                    InfoLabel {
                        id: searchInProgressIndicatorLabel
                        text: qsTr("Searching...")
                        font.pixelSize: Theme.fontSizeLarge
                        width: parent.width - 2 * Theme.horizontalPageMargin
                    }
                }

                Column {
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width

                    id: searchNoResultsColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: ( ((!searchColumn.usersSearchSelected && searchResultsListView.count === 0) || (searchColumn.usersSearchSelected && usersSearchResultsListView.count === 0)) && !( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress ) && !( trendsListView.count !== 0 && searchField.text === "" ) ) ? 1 : 0
                    visible: ( ((!searchColumn.usersSearchSelected && searchResultsListView.count === 0) || (searchColumn.usersSearchSelected && usersSearchResultsListView.count === 0)) && !( searchColumn.usersSearchInProgress || searchColumn.tweetSearchInProgress ) && !( trendsListView.count !== 0 && searchField.text === "" ) ) ? true : false

                    Connections {
                        target: accountModel
                        onImageStyleChanged: {
                            searchNoResultsImage.source = "../../images/" + accountModel.getImagePath() + "piepmatz.svg";
                        }
                    }

                    Image {
                        id: searchNoResultsImage
                        source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        fillMode: Image.PreserveAspectFit
                        width: 1/3 * parent.width
                    }

                    InfoLabel {
                        id: searchNoResultsText
                        text: ( searchField.text === "" || searchColumn.tweetSearchInTransition || searchColumn.usersSearchInTransition ) ? qsTr("What are you looking for?") : qsTr("No results found")
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeLarge
                        width: parent.width - 2 * Theme.horizontalPageMargin
                    }
                }

                SilicaListView {
                    anchors {
                        top: searchTypeRow.bottom
                    }
                    id: trendsListView
                    width: parent.width
                    height: parent.height - searchField.height - searchTypeRow.height - Theme.paddingMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: ( searchField.text === "" && trendsListView.count !== 0 && !( searchColumn.tweetSearchInTransition || searchColumn.usersSearchInTransition ) ) ? 1 : 0
                    visible: ( searchField.text === "" && trendsListView.count !== 0 && !( searchColumn.tweetSearchInTransition || searchColumn.usersSearchInTransition ) ) ? true : false
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: trendsModel
                    delegate: ListItem {
                        contentHeight: trendRow.height
                        contentWidth: parent.width
                        Row {
                            id: trendRow
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            height: Theme.fontSizeHuge
                            spacing: Theme.paddingMedium
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                verticalCenter: parent.verticalCenter
                            }
                            Text {
                                id: trendsNameText
                                anchors.verticalCenter: parent.verticalCenter
                                text: display.name
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                elide: Text.ElideRight
                                maximumLineCount: 1
                                width: parent.width * 3 / 4
                            }
                            Text {
                                id: trendsNameCount
                                anchors.verticalCenter: parent.verticalCenter
                                text: display.tweet_volume ? Number(display.tweet_volume).toLocaleString(Qt.locale(), "f", 0) : ""
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                horizontalAlignment: Text.AlignRight
                                width: parent.width - trendsNameText.width - Theme.paddingMedium
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                        }
                        onClicked: {
                            searchField.text = display.name;
                        }
                    }

                    VerticalScrollDecorator {}
                }

            }

            Item {
                id: listsColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }

                property bool myListsInProgress : false;
                property bool memberListsInProgress : false;

                property bool memberListsSelected : false;

                Connections {
                    target: ownListsModel
                    onUpdateStarted: {
                        console.log("MY LISTS STARTED");
                        listsColumn.myListsInProgress = true;
                    }
                    onOwnListsRetrieved: {
                        console.log("MY LISTS RECEIVED");
                        listsColumn.myListsInProgress = false;
                    }
                    onOwnListsError: {
                        listsColumn.myListsInProgress = false;
                    }
                }

                Connections {
                    target: membershipListsModel
                    onUpdateStarted: {
                        console.log("MEMBER LISTS STARTED");
                        listsColumn.memberListsInProgress = true;
                    }
                    onMembershipListsRetrieved: {
                        console.log("MEMBER LISTS RECEIVED");
                        listsColumn.memberListsInProgress = false;
                    }
                    onMembershipListsError: {
                        listsColumn.memberListsInProgress = false;
                    }
                }

                Row {
                    id: listsTypeRow
                    width: parent.width
                    height: Theme.fontSizeLarge + Theme.paddingMedium
                    anchors.top: listsColumn.top
                    anchors.topMargin: Theme.paddingMedium
                    opacity: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress || (myListsListView.count === 0 && memberListsListView.count === 0)) ? 0 : 1
                    visible: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress || (myListsListView.count === 0 && memberListsListView.count === 0)) ? false : true
                    Behavior on opacity { NumberAnimation {} }
                    Text {
                        id: listsTypeMy
                        width: ( parent.width / 2 )
                        font.pixelSize: Theme.fontSizeMedium
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        color: listsColumn.memberListsSelected ? Theme.primaryColor : Theme.highlightColor
                        textFormat: Text.PlainText
                        anchors.top: parent.top
                        text: qsTr("Subscribed")
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (listsColumn.memberListsSelected) {
                                    listsColumn.memberListsSelected = false;
                                }
                            }
                        }
                    }
                    Separator {
                        width: Theme.fontSizeMedium
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                        anchors.top: parent.top
                        anchors.topMargin: Theme.paddingSmall
                        transform: Rotation { angle: 90 }
                    }
                    Text {
                        id: listsTypeMember
                        width: ( parent.width / 2 ) - ( 2 * Theme.fontSizeMedium )
                        font.pixelSize: Theme.fontSizeMedium
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        color: listsColumn.memberListsSelected ? Theme.highlightColor : Theme.primaryColor
                        textFormat: Text.PlainText
                        anchors.top: parent.top
                        text: qsTr("Member of")
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!listsColumn.memberListsSelected) {
                                    listsColumn.memberListsSelected = true;
                                }
                            }
                        }
                    }
                }

                SilicaListView {
                    anchors {
                        top: listsTypeRow.bottom
                    }
                    id: myListsListView
                    width: parent.width
                    height: parent.height - listsTypeRow.height - Theme.paddingMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: ( !listsColumn.myListsInProgress && !listsColumn.memberListsSelected ) ? 1 : 0
                    visible: ( !listsColumn.myListsInProgress && !listsColumn.memberListsSelected ) ? true : false
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: ownListsModel
                    delegate: TwitterList {
                        listModel: display
                    }

                    VerticalScrollDecorator {}
                }

                SilicaListView {
                    anchors {
                        top: listsTypeRow.bottom
                    }
                    id: memberListsListView
                    width: parent.width
                    height: parent.height - listsTypeRow.height - Theme.paddingMedium
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: ( !listsColumn.memberListsInProgress && listsColumn.memberListsSelected ) ? 1 : 0
                    visible: ( !listsColumn.memberListsInProgress && listsColumn.memberListsSelected ) ? true : false
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: membershipListsModel
                    delegate: TwitterList {
                        listModel: display
                    }

                    VerticalScrollDecorator {}
                }

                Column {
                    anchors {
                        fill: parent
                    }

                    id: listsUpdateInProgressColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress ) ? 1 : 0
                    visible: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress ) ? true : false

                    LoadingIndicator {
                        id: listsLoadingIndicator
                        visible: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress )
                        Behavior on opacity { NumberAnimation {} }
                        opacity: ( listsColumn.myListsInProgress || listsColumn.memberListsInProgress ) ? 1 : 0
                        height: parent.height
                        width: parent.width
                    }
                }

                Column {
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    width: parent.width

                    id: listsNoResultsColumn
                    Behavior on opacity { NumberAnimation {} }
                    opacity: ( ((!listsColumn.memberListsSelected && myListsListView.count === 0) || (listsColumn.memberListsSelected && memberListsListView.count === 0)) && !( listsColumn.myListsInProgress || listsColumn.memberListsInProgress ) ) ? 1 : 0
                    visible: ( ((!listsColumn.memberListsSelected && myListsListView.count === 0) || (listsColumn.memberListsSelected && memberListsListView.count === 0)) && !( listsColumn.myListsInProgress || listsColumn.memberListsInProgress ) ) ? true : false

                    Connections {
                        target: accountModel
                        onImageStyleChanged: {
                            listsNoResultsImage.source = "../../images/" + accountModel.getImagePath() + "piepmatz.svg";
                        }
                    }

                    Image {
                        id: listsNoResultsImage
                        source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        fillMode: Image.PreserveAspectFit
                        width: 1/3 * parent.width
                    }

                    InfoLabel {
                        id: listsNoResultsText
                        text: qsTr("No lists found")
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeLarge
                        width: parent.width - 2 * Theme.horizontalPageMargin
                    }
                }

            }

            Column {
                id: navigationRow
                width: parent.width
                height: getNavigationRowSize()
                Column {
                    id: navigationRowSeparatorColumn
                    width: parent.width
                    height: Theme.paddingMedium
                    Separator {
                        id: navigationRowSeparator
                        width: parent.width
                        color: Theme.primaryColor
                        horizontalAlignment: Qt.AlignHCenter
                    }
                }

                Row {
                    y: Theme.paddingSmall
                    width: parent.width
                    Item {
                        id: homeButtonColumn
                        width: parent.width / 6
                        height: parent.height - Theme.paddingMedium
                        Column {
                            anchors.fill: parent
                            IconButton {
                                id: homeButtonImage
                                icon.source: "image://theme/icon-m-home"
                                height: Theme.iconSizeMedium
                                width: Theme.iconSizeMedium
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                onClicked: {
                                    handleHomeClicked();
                                }

                            }
                            Label {
                                id: homeButtonText
                                text: qsTr("Timeline")
                                font.pixelSize: Theme.fontSizeTiny * 7 / 8
                                color: Theme.primaryColor
                                truncationMode: TruncationMode.Fade
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: handleHomeClicked();
                                }
                            }
                        }
                    }

                    Column {
                        id: notificationsButtonColumn
                        width: parent.width / 6
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: notificationsButtonImage
                            icon.source: "image://theme/icon-m-alarm"
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                handleNotificationsClicked();
                            }
                        }
                        Label {
                            id: notificationsButtonText
                            text: qsTr("Notifications")
                            font.pixelSize: Theme.fontSizeTiny * 7 / 8
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleNotificationsClicked();
                            }
                        }
                    }
                    Column {
                        id: messagesButtonColumn
                        width: parent.width / 6
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: messagesButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-mail"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                handleMessagesClicked();
                            }
                        }
                        Label {
                            id: messagesButtonText
                            text: qsTr("Messages")
                            font.pixelSize: Theme.fontSizeTiny * 7 / 8
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleMessagesClicked();
                            }
                        }
                    }
                    Column {
                        id: searchButtonColumn
                        width: parent.width / 6
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: searchButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-search"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                handleSearchClicked();
                            }
                        }
                        Label {
                            id: searchButtonText
                            text: qsTr("Search")
                            font.pixelSize: Theme.fontSizeTiny * 7 / 8
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleSearchClicked();
                            }
                        }
                    }

                    Column {
                        id: listsButtonColumn
                        width: parent.width / 6
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: listsButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-note"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                handleListsClicked();
                            }
                        }
                        Label {
                            id: listsButtonText
                            text: qsTr("Lists")
                            font.pixelSize: Theme.fontSizeTiny * 7 / 8
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleListsClicked();
                            }
                        }
                    }

                    Column {
                        id: profileButtonColumn
                        width: parent.width / 6
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: profileButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-person"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                handleProfileClicked();
                            }
                        }
                        Label {
                            id: profileButtonText
                            text: qsTr("Profile")
                            font.pixelSize: Theme.fontSizeTiny * 7 / 8
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleProfileClicked();
                            }
                        }
                    }
                }
            }

        }

    }
}
