/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.2
import Sailfish.Silica 1.0
import com.pipacs.o2 1.0


Page {
    id: welcomePage
    allowedOrientations: Orientation.All

    Column {
        y: ( parent.height - ( errorInfoLabel.height + wunderfitzErrorImage.height + errorOkButton.height + ( 3 * Theme.paddingLarge ) ) ) / 2
        width: parent.width
        id: pinErrorColumn
        spacing: Theme.paddingLarge

        Behavior on opacity { NumberAnimation {} }
        opacity: 0
        visible: false

        Image {
            id: wunderfitzErrorImage
            source: "../../images/piepmatz.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: errorInfoLabel
            text: ""
        }

        Button {
            id: errorOkButton
            text: qsTr("OK")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                pinErrorColumn.opacity = 0;
                welcomeFlickable.opacity = 1;
                pinErrorColumn.visible = false;
                welcomeFlickable.visible = true;
            }
        }
    }

    SilicaFlickable {
        id: welcomeFlickable
        anchors.fill: parent
        contentHeight: column.height
        Behavior on opacity { NumberAnimation {} }

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
        }

        Connections {
            target: accountModel
            onPinRequestSuccessful: {
                console.log("URL: " + url)
                Qt.openUrlExternally(url)
            }
            onPinRequestError: {
                errorInfoLabel.text = errorMessage
                welcomeFlickable.visible = false
                pinErrorColumn.visible = true
                welcomeFlickable.opacity = 0
                pinErrorColumn.opacity = 1
                console.log("Error Message: " + errorMessage)
            }
        }

        Column {
            id: column
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Welcome to Piepmatz!")
            }

            Image {
                id: wunderfitzImage
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            Label {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Please login to Twitter to continue.")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Button {
                text: qsTr("Log in to Twitter")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    accountModel.obtainPinUrl()
                }
            }

            Label {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("If you don't have a Twitter account yet, please sign up first.")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://twitter.com/\">" + qsTr("Sign up for Twitter") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://twitter.com/")
            }

        }
    }
}

