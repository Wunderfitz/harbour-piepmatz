/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
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

    function updatePiepmatz() {
        homeView.reloading = true;
        timelineModel.update();
    }

    property string activeTabId: "home";

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
        accountModel.verifyCredentials()
    }

    Connections {
        target: accountModel
        onCredentialsVerified: {
            hideAccountVerificationColumn();
            profileLoader.active = true;
            overviewContainer.visible = true;
            overviewColumn.visible = true;
            overviewColumn.opacity = 1;
            openTab("home");
            timelineModel.update();
        }
        onVerificationError: {
            hideAccountVerificationColumn();
            verificationFailedColumn.visible = true;
            verificationFailedColumn.opacity = 1;
        }
    }

    Connections {
        target: twitterApi
        onTweetError: {
            overviewNotification.show(errorMessage);
        }
        onTweetSuccessful: {
            overviewNotification.show(qsTr("Tweet sent successfully!"));
        }
    }

    Notification {
        id: overviewNotification
    }

    Column {
        y: ( parent.height - ( accountVerificationImage.height + accountVerificationIndicator.height + accountVerificationLabel.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: accountVerificationColumn
        spacing: Theme.paddingSmall
        Behavior on opacity { NumberAnimation {} }

        Image {
            id: accountVerificationImage
            source: "../../images/piepmatz.svg"
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
            source: "../../images/piepmatz.svg"
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
        visible: false

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
            MenuItem {
                text: qsTr("New Tweet")
                onClicked: pageStack.push(newTweetPage)
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: updatePiepmatz()
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: updatePiepmatz()
            }
            MenuItem {
                text: qsTr("New Tweet")
                onClicked: pageStack.push(newTweetPage)
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
                property bool error : false;
                property bool reloading: false;

                Connections {
                    target: timelineModel
                    onHomeTimelineStartUpdate: {
                        homeView.error = false;
                    }

                    onHomeTimelineUpdated: {
                        homeView.loaded = true;
                        homeView.reloading = false;
                    }
                    onHomeTimelineError: {
                        homeView.loaded = true;
                        homeView.error = true;
                        homeView.reloading = false;
                        homeErrorLabel.text = errorMessage;
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

                Column {
                    width: parent.width
                    height: homeProgressLabel.height + homeProgressIndicator.height + Theme.paddingSmall
                    visible: homeView.loaded && homeView.error
                    opacity: homeView.loaded && homeView.error ? 0 : 1
                    id: homeErrorColumn
                    spacing: Theme.paddingSmall
                    Behavior on opacity { NumberAnimation {} }
                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: homeErrorLabel
                        text: ""
                    }
                }

                SilicaListView {
                    id: homeListView
                    opacity: homeView.loaded && !homeView.error ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
                    visible: homeView.loaded && !homeView.error
                    width: parent.width
                    height: parent.height
                    clip: true

                    model: timelineModel

                    delegate: Tweet {
                        tweetModel: display
                    }

                    VerticalScrollDecorator {}
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


            Column {
                id: notificationsColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Notifications not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Column {
                id: messagesColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Messages not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Column {
                id: searchColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Search not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
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
                    Column {
                        id: homeButtonColumn
                        width: parent.width / 5
                        height: parent.height - Theme.paddingMedium

                        IconButton {
                            id: homeButtonImage
                            icon.source: "image://theme/icon-m-home"
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: {
                                if (overviewPage.activeTabId === "home") {
                                    homeListView.scrollToTop();
                                } else {
                                    openTab("home");
                                }
                            }
                        }
                        Label {
                            id: homeButtonText
                            text: qsTr("Timeline")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Column {
                        id: notificationsButtonColumn
                        width: parent.width / 5
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: notificationsButtonImage
                            icon.source: "image://theme/icon-m-alarm"
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: openTab("notifications")
                        }
                        Label {
                            id: notificationsButtonText
                            text: qsTr("Notifications")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Column {
                        id: messagesButtonColumn
                        width: parent.width / 5
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: messagesButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-mail"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: openTab("messages")
                        }
                        Label {
                            id: messagesButtonText
                            text: qsTr("Messages")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Column {
                        id: searchButtonColumn
                        width: parent.width / 5
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: searchButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-search"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: openTab("search")
                        }
                        Label {
                            id: searchButtonText
                            text: qsTr("Search")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    Column {
                        id: profileButtonColumn
                        width: parent.width / 5
                        height: parent.height - navigationRowSeparator.height
                        IconButton {
                            id: profileButtonImage
                            height: Theme.iconSizeMedium
                            width: Theme.iconSizeMedium
                            icon.source: "image://theme/icon-m-person"
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            onClicked: openTab("profile")
                        }
                        Label {
                            id: profileButtonText
                            text: qsTr("Profile")
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.primaryColor
                            truncationMode: TruncationMode.Fade
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                        }
                    }
                }
            }

        }

    }
}
