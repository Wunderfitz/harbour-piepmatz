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
import Sailfish.Silica 1.0
import "../js/functions.js" as Functions


Page {
    id: settingsPage
    allowedOrientations: Orientation.All

    property variant allAccounts;

    Component.onCompleted: {
        allAccounts = [ accountModel.getCurrentAccount().screen_name ];
        allAccounts = allAccounts.concat(accountModel.getOtherAccounts());
    }

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        Column {
            id: column
            width: settingsPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Settings")
            }

            SectionHeader {
                text: qsTr("Location")
            }

            TextSwitch {
                checked: locationInformation.isEnabled()
                text: qsTr("Enable Positioning")
                description: qsTr("Allow Piepmatz to use GPS/GLONASS/Galileo etc. for tweet geotagging")
                onCheckedChanged: {
                    locationInformation.setEnabled(checked);
                }
            }

            SectionHeader {
                text: qsTr("Images")
            }

            TextSwitch {
                checked: accountModel.getUseEmoji()
                text: qsTr("Use Twitter Emoji")
                description: qsTr("Use Twitter Emoji instead of system font")
                onCheckedChanged: {
                    accountModel.setUseEmoji(checked);
                }
            }

            ComboBox {
                id: imagesComboBox
                label: qsTr("Image Style")
                currentIndex: (accountModel.getImagePath() === "") ? 0 : 1
                description: qsTr("Choose the active image style here")
                menu: ContextMenu {
                     MenuItem {
                        text: qsTr("Standard")
                     }
                     MenuItem {
                        text: qsTr("Piepfish by @ferlanero")
                     }
                    onActivated: {
                        var imagePath = ( index === 0 ? "" : "ferlanero/" );
                        accountModel.setImagePath(imagePath);
                    }
                }
            }

            SectionHeader {
                text: qsTr("Accounts")
            }

            ComboBox {
                id: accountsComboBox
                label: qsTr("Account")
                currentIndex: 0
                description: qsTr("Choose the active account here")
                menu: ContextMenu {
                    Repeater {
                        model: allAccounts
                        delegate: MenuItem {
                            text: qsTr("@%1").arg(modelData)
                        }
                    }
                    onActivated: {
                        console.log("Account " + allAccounts[index] + " was selected");
                        accountModel.switchAccount(allAccounts[index]);
                        pageStack.clear();
                        pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ? (accountModel.isLinked() ? overviewPage : welcomePage) : registrationPage);
                    }
                }
            }

            Button {
                id: registerNewAccountButton
                text: qsTr("Login with other Account")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    accountModel.registerNewAccount();
                    pageStack.clear();
                    pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ? (accountModel.isLinked() ? overviewPage : welcomePage) : registrationPage);
                }
            }

            VerticalScrollDecorator {}
        }

    }
}
