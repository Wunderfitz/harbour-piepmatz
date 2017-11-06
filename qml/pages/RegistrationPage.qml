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
import QtQuick 2.2
import Sailfish.Silica 1.0

Page {
    id: registrationPage
    allowedOrientations: Orientation.All

    SilicaFlickable {
        id: registrationFlickable
        anchors.fill: parent
        contentHeight: termsOfUseColumn.height
        Behavior on opacity { NumberAnimation {} }

        Connections {
            target: wagnis
        }

        Column {
            id: termsOfUseColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Welcome to Piepmatz!")
            }

            Image {
                id: piepmatzImage
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            SectionHeader {
                text: qsTr("Committed to Free Software")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Welcome to Piepmatz - a Twitter client for SailfishOS! Piepmatz is <a href=\"https://www.gnu.org/philosophy/free-sw\">free software</a> and licensed under the conditions of the <a href=\"https://www.gnu.org/licenses/gpl.html\">General Public License in version 3 (GPLv3)</a>. By using this software, you accept the terms and conditions of this license.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            SectionHeader {
                text: qsTr("Anonymous Registration")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Moreover, Piepmatz will register this installation automatically on my server if you press 'Accept'. I respect your privacy, therefore only anonymous and a very limited set of information will be transmitted: a unique identifer for this installation and your country. I simply would like to know how many users Piepmatz has and where they come from. If this is not OK for you, simply close the application. No information has been transmitted so far.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Have fun with Piepmatz! Sebastian J. Wolf")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Accept")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    pageStack.clear();
                    accountModel.isLinked() ? pageStack.push(overviewPage) : pageStack.push(welcomePage)
                }
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

        }

    }

}
