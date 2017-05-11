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

    function handleProfileClicked() {
        openTab("profile");
    }

    property string activeTabId: "home";
    property bool initializationCompleted : false;
    property variant configuration;
    property bool tweetInProgress : false;

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
        accountModel.verifyCredentials();
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
                directMessagesModel.setUserId(accountModel.getCurrentAccount().id_str);
                directMessagesModel.update();
                overviewPage.initializationCompleted = true;
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

    Notification {
        id: overviewNotification
    }

    Item {
        id: persistentNotificationItem
        enabled: false
        width: parent.width
        height: persistentNotification.height
        y: parent.height - getNavigationRowSize() - persistentNotification.height - Theme.paddingSmall
        z: 42

        NotificationItem {
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
                    }

                    onMovementEnded: {
                        timelineModel.setCurrentTweetId(homeListView.itemAt(homeListView.contentX, ( homeListView.contentY + Math.round(overviewPage.height / 2))).tweetModel.id_str);
                    }

                    onQuickScrollAnimatingChanged: {
                        if (!quickScrollAnimating) {
                            timelineModel.setCurrentTweetId(homeListView.itemAt(homeListView.contentX, ( homeListView.contentY + Math.round(overviewPage.height / 2))).tweetModel.id_str);
                        }
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


            Item {
                id: notificationsColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }

                property bool updateInProgress : false;

                Connections {
                    target: mentionsModel
                    onUpdateMentionsFinished: {
                        notificationsColumn.updateInProgress = false;
                    }
                    onUpdateMentionsError: {
                        notificationsColumn.updateInProgress = false;
                        overviewNotification.show(errorMessage);
                    }
                }

                SilicaListView {
                    anchors {
                        fill: parent
                    }
                    id: mentionsListView

                    clip: true

                    model: mentionsModel
                    delegate: Tweet {
                        tweetModel: display
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

                Column {
                    width: parent.width
                    height: messagesNotImplementedImage.height + messagesNotImplementedLabel.height
                    anchors.verticalCenter: parent.verticalCenter
                    Image {
                        id: messagesNotImplementedImage
                        source: "../../images/piepmatz.svg"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        fillMode: Image.PreserveAspectFit
                        width: 1/2 * parent.width
                    }

                    InfoLabel {
                        id: messagesNotImplementedLabel
                        text: qsTr("Messages are not yet implemented")
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
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

                property bool searchInProgress : false;
                property bool inTransition : false;

                Connections {
                    target: searchModel
                    onSearchFinished: {
                        searchColumn.searchInProgress = false;
                        searchColumn.inTransition = false;
                    }
                    onSearchError: {
                        searchColumn.searchInProgress = false;
                        searchColumn.inTransition = false;
                        overviewNotification.show(errorMessage);
                    }
                }

                Timer {
                    id: searchTimer
                    interval: 1000
                    running: false
                    repeat: false
                    onTriggered: {
                        searchColumn.searchInProgress = true;
                        searchModel.search(searchField.text)
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
                        searchColumn.inTransition = true;
                        searchTimer.stop()
                        searchTimer.start()
                    }
                }

                SilicaListView {
                    anchors {
                        top: searchField.bottom
                    }
                    id: searchResultsListView
                    width: parent.width
                    height: parent.height - searchField.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    opacity: searchColumn.searchInProgress ? 0 : 1
                    visible: searchColumn.searchInProgress ? false : true
                    Behavior on opacity { NumberAnimation {} }

                    clip: true

                    model: searchModel
                    delegate: Tweet {
                        tweetModel: display
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
                    opacity: searchColumn.searchInProgress ? 1 : 0
                    visible: searchColumn.searchInProgress ? true : false

                    BusyIndicator {
                        id: searchInProgressIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: searchColumn.searchInProgress
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
                    opacity: ( searchResultsListView.count === 0 && !searchColumn.searchInProgress ) ? 1 : 0
                    visible: ( searchResultsListView.count === 0 && !searchColumn.searchInProgress ) ? true : false

                    Image {
                        id: searchNoResultsImage
                        source: "../../images/piepmatz.svg"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                        fillMode: Image.PreserveAspectFit
                        width: 1/3 * parent.width
                    }

                    InfoLabel {
                        id: searchNoResultsText
                        text: ( searchField.text === "" || searchColumn.inTransition ) ? qsTr("What are you looking for?") : qsTr("No results found")
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
                        width: parent.width / 5
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
                                font.pixelSize: Theme.fontSizeTiny
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
                            onClicked: {
                                handleNotificationsClicked();
                            }
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
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleNotificationsClicked();
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
                            onClicked: {
                                handleMessagesClicked();
                            }
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
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleMessagesClicked();
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
                            onClicked: {
                                handleSearchClicked();
                            }
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
                            MouseArea {
                                anchors.fill: parent
                                onClicked: handleSearchClicked();
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
                            onClicked: {
                                handleProfileClicked();
                            }
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
