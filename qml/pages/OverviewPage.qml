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
                width: overviewContainer.width

                Rectangle {
                    id: profilePictureBackground
                    width: parent.width / 3 + parent.width / 30
                    height: parent.width / 3 + parent.width / 30
                    color: Theme.primaryColor
                    border.color: "black"
                    border.width: 1
                    radius: parent.width / 17
                    anchors.margins: Theme.horizontalPageMargin
                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                Image {
                    id: profilePicture
                    source: display.profile_image_url_https
                    width: parent.width / 3
                    height: parent.width / 3
                    anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                    anchors.left: parent.left
                    anchors.top: parent.top
                    visible: false
                }

                Rectangle {
                    id: profilePictureMask
                    width: parent.width / 3
                    height: parent.width / 3
                    color: Theme.primaryColor
                    radius: parent.width / 20
                    anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                    anchors.left: parent.left
                    anchors.top: parent.top
                    visible: false
                }

                OpacityMask {
                    id: maskedProfilePicture
                    anchors.fill: profilePicture
                    source: profilePicture
                    maskSource: profilePictureMask
                }

                Column {
                    id: provideOverviewColumn
                    spacing: Theme.paddingMedium
                    anchors {
                        leftMargin: Theme.horizontalPageMargin
                        topMargin: Theme.horizontalPageMargin
                        left: profilePictureBackground.right
                        right: parent.right
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

                Column {
                    id: profileDetailsColumn
                    spacing: Theme.paddingLarge
                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: Theme.horizontalPageMargin
                        rightMargin: Theme.horizontalPageMargin
                        topMargin: Theme.paddingLarge
                        top: provideOverviewColumn.height > profilePictureBackground.height ? provideOverviewColumn.bottom : profilePictureBackground.bottom
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
                            source: "image://theme/icon-m-computer"
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


                    Text {
                        text: display.description
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        width: parent.width
                    }

                    Text {
                        text: display.description
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        width: parent.width
                    }

                    Text {
                        text: display.description
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        width: parent.width
                    }

                }
            }
        }
    }
}
