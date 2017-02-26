/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0


Page {
    id: overviewPage
    allowedOrientations: Orientation.All

    function hideAccountVerificationColumn() {
        accountVerificationColumn.opacity = 0
        accountVerificationIndicator.running = false
        accountVerificationColumn.visible = false;
    }

    function showAccountVerificationColumn() {
        accountVerificationColumn.opacity = 1
        accountVerificationIndicator.running = true
        accountVerificationColumn.visible = true;
    }

    function hideBackgroundColumn() {
        backgroundColumn.opacity = 0
        backgroundColumn.visible = false;
    }

    function showBackgroundColumn() {
        backgroundColumn.opacity = 1
        backgroundColumn.visible = true;
    }

    function getNavigationRowSize() {
        return Theme.iconSizeMedium + Theme.fontSizeMedium + Theme.paddingLarge
    }

    property string activeTabId: "profile"

    function toggleNavigationButton(tabId) {
        if (tabId === activeTabId) {
            return;
        }

        var newColor = Theme.primaryColor;
        switch (tabId) {
        case "home":
            newColor = homeButtonText.color === Theme.primaryColor ? Theme.highlightColor : Theme.primaryColor
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + newColor
            homeButtonText.color = newColor
            break;
        case "notifications":
            newColor = notificationsButtonText.color === Theme.primaryColor ? Theme.highlightColor : Theme.primaryColor
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + newColor
            notificationsButtonText.color = newColor
            break;
        case "messages":
            newColor = messagesButtonText.color === Theme.primaryColor ? Theme.highlightColor : Theme.primaryColor
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + newColor
            messagesButtonText.color = newColor
            break;
        case "search":
            newColor = searchButtonText.color === Theme.primaryColor ? Theme.highlightColor : Theme.primaryColor
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + newColor
            searchButtonText.color = newColor
            break;
        case "profile":
            newColor = profileButtonText.color === Theme.primaryColor ? Theme.highlightColor : Theme.primaryColor
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + newColor
            profileButtonText.color = newColor
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
            hideAccountVerificationColumn()
            overviewContainer.opacity = 1;
            overviewContainer.visible = true;
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.highlightColor
            profileButtonText.color = Theme.highlightColor
        }
        onVerificationError: {
            hideAccountVerificationColumn()
            verificationFailedColumn.visible = true;
            verificationFailedColumn.opacity = 1
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
            text: qsTr("Verifying your account...")
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
            text: qsTr("Piepmatz could not verify your account!")
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
        opacity: 0

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
            MenuItem {
                text: qsTr("New Tweet")
                onClicked: pageStack.push(newTweetPage)
            }
        }

        Column {
            id: overviewColumn
            anchors {
                fill: parent
            }

            SilicaListView {
                id: profileView
                width: parent.width
                height: parent.height - getNavigationRowSize()
                clip: true

                model: accountModel

                delegate: Item {
                    id: profileComponent
                    height: profileColumn.height
                    width: parent.width

                    Column {
                        id: profileColumn
                        width: parent.width
                        spacing: Theme.paddingLarge

                        Row {
                            id: profileTitleRow
                            spacing: Theme.horizontalPageMargin
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            height: profilePictureColumn.height > profileOverviewColumn.height ? profilePictureColumn.height + Theme.paddingLarge : profileOverviewColumn.height + Theme.paddingLarge
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                            Column {
                                id: profilePictureColumn
                                width: parent.width / 3
                                anchors {
                                    leftMargin: Theme.horizontalPageMargin
                                    topMargin: Theme.horizontalPageMargin
                                    top: parent.top
                                }
                                Item {
                                    id: profilePictureItem
                                    width: profilePictureColumn.width
                                    height: profilePictureItem.width
                                    Rectangle {
                                        id: profilePictureBackground
                                        width: parent.width
                                        height: parent.height
                                        color: Theme.primaryColor
                                        border.color: "black"
                                        border.width: 1
                                        radius: parent.width / 6
                                        anchors {
                                            margins: Theme.horizontalPageMargin
                                        }
                                    }

                                    Image {
                                        id: profilePicture
                                        source: display.profile_image_url_https
                                        width: parent.width - parent.width / 10
                                        height: parent.height - parent.height / 10
                                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                                        anchors.centerIn: profilePictureBackground
                                        visible: false
                                    }

                                    Rectangle {
                                        id: profilePictureMask
                                        width: parent.width - parent.width / 10
                                        height: parent.height - parent.height / 10
                                        color: Theme.primaryColor
                                        radius: parent.width / 7
                                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                                        anchors.centerIn: profilePictureBackground
                                        visible: false
                                    }

                                    OpacityMask {
                                        id: maskedProfilePicture
                                        source: profilePicture
                                        maskSource: profilePictureMask
                                        anchors.fill: profilePicture
                                    }
                                }
                            }

                            Column {
                                id: profileOverviewColumn
                                width: parent.width * 2 / 3
                                spacing: Theme.paddingSmall
                                anchors {
                                    leftMargin: Theme.horizontalPageMargin
                                    topMargin: Theme.horizontalPageMargin
                                    top: parent.top
                                }
                                Text {
                                    id: profileNameText
                                    text: display.name
                                    font {
                                        pixelSize: Theme.fontSizeLarge
                                        bold: true
                                    }
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileScreenNameText
                                    text: qsTr("@%1").arg(display.screen_name)
                                    font {
                                        pixelSize: Theme.fontSizeMedium
                                        bold: true
                                    }
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileFriendsText
                                    text: qsTr("%1 Following").arg(display.friends_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileFollowersText
                                    text: qsTr("%1 Followers").arg(display.followers_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                            }
                        }

                        Column {
                            id: profileDetailsColumn
                            spacing: Theme.paddingLarge
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }
                            Text {
                                id: profileDescriptionText
                                text: display.description
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                wrapMode: Text.Wrap
                                width: parent.width
                            }
                            Row {
                                id: profileMoreInformationRow
                                spacing: Theme.horizontalPageMargin
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Text {
                                    id: profileTweetsText
                                    text: qsTr("%1 Tweets").arg(display.statuses_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                                Text {
                                    id: profileMoreInfoSeparatorText
                                    text: qsTr("|")
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                                Text {
                                    id: profileFavoritesText
                                    text: qsTr("%1 Favorites").arg(display.favourites_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                            }
                            Text {
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                id: profileJoinedText
                                text: qsTr("Joined in %1").arg((new Date(display.created_at)).toLocaleDateString(Qt.locale(), "MMMM yyyy"))
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                wrapMode: Text.Wrap
                            }

                            Row {
                                id: profileLocationRow
                                visible: display.location.length === 0 ? false : true
                                spacing: Theme.horizontalPageMargin
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Image {
                                    id: profileLocationImage
                                    source: "image://theme/icon-m-location"
                                }
                                Text {
                                    id: profileLocationText
                                    text: display.location
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            Row {
                                id: profileUrlRow
                                spacing: Theme.horizontalPageMargin
                                visible: display.entities.url.urls.length === 0 ? false : true
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Image {
                                    id: profileUrlImage
                                    source: "image://theme/icon-m-link"
                                }
                                Text {
                                    id: profileUrlText
                                    text: "<a href=\"" + display.entities.url.urls[0].url + "\">" + display.entities.url.urls[0].display_url + "</a>"
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                    anchors.verticalCenter: parent.verticalCenter
                                    onLinkActivated: Qt.openUrlExternally(display.entities.url.urls[0].url)
                                    linkColor: Theme.highlightColor
                                }
                            }

                        }
                    }
                }
                VerticalScrollDecorator {}
            }

            Row {
                id: navigationRow
                width: parent.width
                height: getNavigationRowSize()

                Column {
                    id: homeButtonColumn
                    width: parent.width / 5
                    height: parent.height

                    IconButton {
                        id: homeButtonImage
                        icon.source: "image://theme/icon-m-home"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onPressed: {
                            toggleNavigationButton("home")
                        }
                        onReleased: {
                            toggleNavigationButton("home")
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
                    height: parent.height
                    IconButton {
                        id: notificationsButtonImage
                        icon.source: "image://theme/icon-m-alarm"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onPressed: {
                            toggleNavigationButton("notifications")
                        }
                        onReleased: {
                            toggleNavigationButton("notifications")
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
                    }
                }
                Column {
                    id: messagesButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: messagesButtonImage
                        icon.source: "image://theme/icon-m-mail"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onPressed: {
                            toggleNavigationButton("messages")
                        }
                        onReleased: {
                            toggleNavigationButton("messages")
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
                    }
                }
                Column {
                    id: searchButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: searchButtonImage
                        icon.source: "image://theme/icon-m-search"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onPressed: {
                            toggleNavigationButton("search")
                        }
                        onReleased: {
                            toggleNavigationButton("search")
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
                    }
                }
                Column {
                    id: profileButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: profileButtonImage
                        icon.source: "image://theme/icon-m-person"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onPressed: {
                            toggleNavigationButton("profile")
                        }
                        onReleased: {
                            toggleNavigationButton("profile")
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

                    }
                }
            }

        }

    }
}
