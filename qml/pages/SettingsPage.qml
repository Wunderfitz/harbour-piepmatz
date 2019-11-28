/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        settingsContainer.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        settingsContainer.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            settingsContainer.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            settingsContainer.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            settingsContainer.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            settingsContainer.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    property variant allAccounts;

    Component.onCompleted: {
        allAccounts = [ accountModel.getCurrentAccount().screen_name ];
        allAccounts = allAccounts.concat(accountModel.getOtherAccounts());
    }

    RemorsePopup {
        id: removeAccountRemorsePopup
    }

    SilicaFlickable {
        id: settingsContainer
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
                id: removeAccountButton
                text: qsTr("Remove current Account")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    removeAccountRemorsePopup.execute(qsTr("Removing account"), function() {
                        accountModel.removeCurrentAccount();
                        pageStack.clear();
                        pageStack.push(( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ? (accountModel.isLinked() ? overviewPage : welcomePage) : registrationPage);
                    } );
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

            SectionHeader {
                text: qsTr("Style")
            }

            TextSwitch {
                checked: accountModel.getUseEmoji()
                text: qsTr("Use Twitter Emoji")
                description: qsTr("Use Twitter Emoji instead of system font")
                onCheckedChanged: {
                    accountModel.setUseEmoji(checked);
                }
            }

            TextSwitch {
                checked: accountModel.getUseLoadingAnimations()
                text: qsTr("Enable loading animations")
                description: qsTr("Use animations when additional content is loaded")
                onCheckedChanged: {
                    accountModel.setUseLoadingAnimations(checked);
                }
            }

            TextSwitch {
                checked: accountModel.getUseSwipeNavigation()
                text: qsTr("Enable swipe navigation")
                description: qsTr("Use swipe navigation to switch categories (e.g. from timeline to notifications)")
                onCheckedChanged: {
                    accountModel.setUseSwipeNavigation(checked);
                }
            }

            TextSwitch {
                checked: accountModel.getDisplayImageDescriptions()
                text: qsTr("Display image descriptions")
                description: qsTr("Display additional descriptions of tweet images (if available)")
                onCheckedChanged: {
                    accountModel.setDisplayImageDescriptions(checked);
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

            ComboBox {
                id: fontSizeComboBox
                label: qsTr("Font Size")
                currentIndex: (accountModel.getFontSize() === "piepmatz") ? 0 : 1
                description: qsTr("Choose the font size here")
                menu: ContextMenu {
                     MenuItem {
                        text: qsTr("Normal (Piepmatz default)")
                     }
                     MenuItem {
                        text: qsTr("Large (Sailfish OS default)")
                     }
                    onActivated: {
                        var fontSize = ( index === 0 ? "piepmatz" : "sailfish" );
                        accountModel.setFontSize(fontSize);
                    }
                }
            }

            SectionHeader {
                text: qsTr("Behavior")
            }

            ComboBox {
                id: linkPreviewComboBox
                label: qsTr("Link Preview")
                currentIndex: (accountModel.getLinkPreviewMode() === "always") ? 0 : ( (accountModel.getLinkPreviewMode() === "wifiOnly" ? 1 : 2 ) )
                description: qsTr("Choose the link preview mode here")
                menu: ContextMenu {
                     MenuItem {
                        text: qsTr("Always")
                     }
                     MenuItem {
                        text: qsTr("Only on WiFi")
                     }
                     MenuItem {
                        text: qsTr("Never")
                     }
                    onActivated: {
                        var linkPreviewMode = ( index === 0 ? "always" : ( index === 1 ? "wifiOnly" : "never" ) );
                        accountModel.setLinkPreviewMode(linkPreviewMode);
                    }
                }
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
