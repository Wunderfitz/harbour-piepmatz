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
                    id: profilePicureItem
                    anchors.fill: parent

                    Rectangle {
                        id: profilePictureBackground
                        width: parent.width / 3 + parent.width / 30
                        height: parent.width / 3 + parent.width / 30
                        color: "white"
                        radius: parent.width / 17
                        anchors.margins: parent.width / 20 - parent.width / 60
                        anchors.left: profilePicureItem.left
                        anchors.top: profilePicureItem.top
                    }

                    Image {
                        id: profilePicture
                        source: display.profile_image_url_https
                        width: parent.width / 3
                        height: parent.width / 3
                        anchors.margins: parent.width / 20
                        anchors.left: profilePicureItem.left
                        anchors.top: profilePicureItem.top
                        visible: false
                    }

                    Rectangle {
                        id: profilePictureMask
                        width: parent.width / 3
                        height: parent.width / 3
                        color: "white"
                        radius: parent.width / 20
                        anchors.margins: parent.width / 20
                        anchors.left: profilePicureItem.left
                        anchors.top: profilePicureItem.top
                        visible: false
                    }

                    OpacityMask {
                        id: maskedProfilePicture
                        anchors.fill: profilePicture
                        source: profilePicture
                        maskSource: profilePictureMask
                    }

                }
            }
        }

    }
}
