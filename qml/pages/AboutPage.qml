/*
    Copyright (C) 2017-20 Sebastian J. Wolf

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
import "../js/functions.js" as Functions

Page {
    id: aboutPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        aboutContainer.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        aboutContainer.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            aboutContainer.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            aboutContainer.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            aboutContainer.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            aboutContainer.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: aboutPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("About Piepmatz")
            }

            Image {
                id: wunderfitzImage
                source: "../../images/" + accountModel.getImagePath() + "piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 2/3 * parent.width

            }

            Label {
                text: "Piepmatz 1.5.1"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("A Twitter client for Sailfish OS")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                text: qsTr("By Sebastian J. Wolf")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"profile://ygriega\">" + qsTr("Find me on Twitter") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Functions.handleLink(link);
            }

            Text {
                text: "<a href=\"mailto:sebastian@ygriega.de\">" + qsTr("Send E-Mail") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("mailto:sebastian@ygriega.de")
            }

            Separator {
                width: parent.width
                color: Theme.primaryColor
                horizontalAlignment: Qt.AlignHCenter
            }

            Label {
                text: qsTr("Licensed under GNU GPLv3")
                font.pixelSize: Theme.fontSizeSmall
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/Wunderfitz/harbour-piepmatz\">" + qsTr("Sources on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/Wunderfitz/harbour-piepmatz")
            }

            SectionHeader {
                text: qsTr("About Twitter")
            }

            Button {
                id: twitterTosButton
                text: qsTr("Terms of Service")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/TextPage.qml"), {"contentId": "tos"});
                }
            }

            Button {
                id: twitterPrivacyButton
                text: qsTr("Privacy Policy")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/TextPage.qml"), {"contentId": "privacy"});
                }
            }

//            SectionHeader {
//                text: "Wagnis"
//            }

//            Label {
//                id: wagnisIdLabel
//                text: qsTr("Wagnis ID: ") + wagnis.getId()
//                font.pixelSize: Theme.fontSizeSmall
//                anchors {
//                    horizontalCenter: parent.horizontalCenter
//                }
//                visible: false
//            }
//            Button {
//                id: showWagnisIdButton
//                text: qsTr("Show Wagnis ID")
//                anchors {
//                    horizontalCenter: parent.horizontalCenter
//                }
//                onClicked: {
//                    showWagnisIdButton.visible = false;
//                    wagnisIdLabel.visible = true;
//                }
//            }

            SectionHeader {
                text: qsTr("Credits")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses OAuth for Qt by Akos Polster. Thanks for making it available under the conditions of the BSD-2-Clause license!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/pipacs/o2\">" + qsTr("Open OAuth for Qt on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/pipacs/o2")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses twitter-text. Copyright 2014 Twitter, Inc. and other contributors. Thanks for making it available under the conditions of the Apache License, Version 2.0!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/twitter/twitter-text\">" + qsTr("Open twitter-text on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/twitter/twitter-text")
            }


            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses twemoji. Copyright 2018 Twitter, Inc. and other contributors. Thanks for making it available under the conditions of the MIT License (coding) and CC-BY 4.0 (graphics)!")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/twitter/twemoji\">" + qsTr("Open twemoji on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/twitter/twemoji")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("This project uses QGumboParser. Copyright 2017 Sergey Lagner. Thanks for making it available under the conditions of the MIT License! QGumboParser uses Gumbo. Copyright 2017 by Google. Licensed under the conditions of the Apache License 2.0.")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Text {
                text: "<a href=\"https://github.com/lagner/QGumboParser\">" + qsTr("Open QGumboParser on GitHub") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://github.com/lagner/QGumboParser")
            }

            Text {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                text: qsTr("Piepmatz uses ipinfo.io and (if available) the GPS receiver of your device to determine your location. Your location is used to retrieve trends and - only if you explicitly agree - as attachment to your tweets. If you want to use your location in tweets, please ensure to have it enabled in your <a href=\"https://twitter.com/settings/safety\">Twitter settings</a>.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                wrapMode: Text.Wrap
                textFormat: Text.StyledText
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                text: "<a href=\"https://ipinfo.io\">" + qsTr("Open ipinfo.io") + "</a>"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor

                onLinkActivated: Qt.openUrlExternally("https://ipinfo.io")
            }

            Label {
                id: separatorLabel
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
