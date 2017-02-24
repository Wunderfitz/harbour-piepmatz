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

    Component.onCompleted: {
        accountModel.verifyCredentials()
    }

    Connections {
        target: accountModel
        onCredentialsVerified: {
            hideAccountVerificationColumn()
            overviewContainer.opacity = 1;
            overviewContainer.visible = true;
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
        contentHeight: profileRepeater.height
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

        Repeater {
            id: profileRepeater
            model: accountModel

            delegate: Item {
                id: profileComponent
                anchors.fill: parent

                Item {
                    id: profileTitleItem
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.right: parent.right

                    Rectangle {
                        id: profilePictureBackground
                        width: parent.width / 3 + parent.width / 30
                        height: parent.width / 3 + parent.width / 30
                        color: Theme.primaryColor
                        border.color: "black"
                        border.width: 1
                        radius: parent.width / 17
                        anchors.margins: Theme.horizontalPageMargin
                        anchors.left: profileTitleItem.left
                        anchors.top: profileTitleItem.top
                    }

                    Image {
                        id: profilePicture
                        source: display.profile_image_url_https
                        width: parent.width / 3
                        height: parent.width / 3
                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                        anchors.left: profileTitleItem.left
                        anchors.top: profileTitleItem.top
                        visible: false
                    }

                    Rectangle {
                        id: profilePictureMask
                        width: parent.width / 3
                        height: parent.width / 3
                        color: Theme.primaryColor
                        radius: parent.width / 20
                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                        anchors.left: profileTitleItem.left
                        anchors.top: profileTitleItem.top
                        visible: false
                    }

                    OpacityMask {
                        id: maskedProfilePicture
                        anchors.fill: profilePicture
                        source: profilePicture
                        maskSource: profilePictureMask
                    }

                    Text {
                        id: profileNameText
                        text: display.name
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.horizontalPageMargin
                        anchors.left: profilePictureBackground.right
                        anchors.right: profileTitleItem.right
                        anchors.top: profileTitleItem.top
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileScreenNameText
                        text: "@" + display.screen_name
                        font.pixelSize: Theme.fontSizeMedium
                        font.bold: true
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.left: profilePictureBackground.right
                        anchors.right: profileTitleItem.right
                        anchors.top: profileNameText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileFriendsText
                        text: qsTr("%1 Following").arg(display.friends_count)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: profilePictureBackground.right
                        anchors.right: profileTitleItem.right
                        anchors.top: profileScreenNameText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileFollowersText
                        text: qsTr("%1 Followers").arg(display.followers_count)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingSmall
                        anchors.left: profilePictureBackground.right
                        anchors.right: profileTitleItem.right
                        anchors.top: profileFriendsText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileDescriptionText
                        text: display.description
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: ( profileFriendsText.y + profileFriendsText.height ) > ( profilePictureBackground.y + profilePictureBackground.height ) ? profileFriendsText.bottom : profilePictureBackground.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileTweetsText
                        text: qsTr("%1 Tweets").arg(display.statuses_count)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: profileDescriptionText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileFavoritesText
                        text: qsTr("%1 Favorites").arg(display.favourites_count)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: profileTweetsText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileLocationText
                        text: qsTr("Location: %1").arg(display.location)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: profileFavoritesText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileJoinedText
                        text: qsTr("Joined on %1").arg((new Date(display.created_at)).toLocaleDateString())
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: profileLocationText.bottom
                        wrapMode: Text.Wrap
                    }

                    Text {
                        id: profileUrlText
                        text: qsTr("See more on the <a href=\"https://twitter.com/%1\">Twitter Profile</a>").arg(display.screen_name)
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        anchors.leftMargin: Theme.horizontalPageMargin
                        anchors.rightMargin: Theme.horizontalPageMargin
                        anchors.topMargin: Theme.paddingLarge
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: profileJoinedText.bottom
                        wrapMode: Text.Wrap
                        linkColor: Theme.highlightColor
                        onLinkActivated: Qt.openUrlExternally("https://twitter.com/" + display.screen_name)
                    }

                }


            }
        }

    }
}
