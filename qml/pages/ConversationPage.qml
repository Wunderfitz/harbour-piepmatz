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
import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import "../js/functions.js" as Functions
import "../js/twitter-text.js" as TwitterText

Page {
    id: conversationPage
    allowedOrientations: Orientation.All

    property variant conversationModel;
    property variant configuration;
    property string myUserId;
    property bool loaded : true;

    function getRemainingCharacters(text, configuration) {
        return 10000 - TwitterText.twttr.txt.getTweetLength(text, configuration);
    }

    AppNotification {
        id: conversationNotification
    }

    Connections {
        target: twitterApi

        onDirectMessagesNewSuccessful: {
            var newMessages = conversationListView.model;
            newMessages.push(result.event);
            conversationListView.model = newMessages;

            conversationListView.positionViewAtEnd();
        }

        onDirectMessagesNewError: {
            conversationNotification.show(errorMessage);
        }
    }

    ProfileHeader {
        id: profileHeader
        profileModel: conversationModel.user
        width: parent.width
    }


    SilicaFlickable {
        id: conversationContainer
        width: parent.width
        height: parent.height - profileHeader.height
        anchors.bottom: parent.bottom

        LoadingIndicator {
            id: conversationLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            id: messageListColumn
            width: parent.width
            height: parent.height
            anchors.bottom: parent.bottom

            SilicaListView {
                id: conversationListView
                Component.onCompleted: positionViewAtEnd();

                width: parent.width
                height: parent.height

                clip: true

                model: conversationModel.messages
                delegate: ListItem {

                    id: messageListItem
                    contentHeight: messageTextItem.height + ( 2 * Theme.paddingMedium )
                    contentWidth: parent.width

                    Column {
                        id: messageTextItem

                        spacing: Theme.paddingSmall

                        width: parent.width
                        height: messageText.height + messageDateText.height + ( 2 * Theme.paddingMedium )
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: (modelData.message_create.sender_id === conversationPage.myUserId) ? 4 * Theme.horizontalPageMargin : Theme.horizontalPageMargin
                                right: parent.right
                                rightMargin: (modelData.message_create.sender_id === conversationPage.myUserId) ? Theme.horizontalPageMargin : 4 * Theme.horizontalPageMargin
                            }

                            id: messageText
                            text: Functions.enhanceSimpleText(modelData.message_create.message_data.text, modelData.message_create.message_data.entities)
                            font.pixelSize: Theme.fontSizeSmall
                            color: modelData.message_create.sender_id === conversationPage.myUserId ? Theme.highlightColor : Theme.primaryColor
                            wrapMode: Text.Wrap
                            textFormat: Text.StyledText
                            onLinkActivated: {
                                Functions.handleLink(link);
                            }
                            horizontalAlignment: (modelData.message_create.sender_id === conversationPage.myUserId) ? Text.AlignRight : Text.AlignLeft
                            linkColor: Theme.highlightColor
                        }

                        Timer {
                            id: messageDateUpdater
                            interval: 60000
                            running: true
                            repeat: true
                            onTriggered: {
                                messageDateText.text = Format.formatDate(new Date(parseInt(modelData.created_timestamp)), Formatter.DurationElapsed);
                            }
                        }

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: (modelData.message_create.sender_id === conversationPage.myUserId) ? 4 * Theme.horizontalPageMargin : Theme.horizontalPageMargin
                                right: parent.right
                                rightMargin: (modelData.message_create.sender_id === conversationPage.myUserId) ? Theme.horizontalPageMargin : 4 * Theme.horizontalPageMargin
                            }

                            id: messageDateText
                            text: Format.formatDate(new Date(parseInt(modelData.created_timestamp)), Formatter.DurationElapsed);
                            font.pixelSize: Theme.fontSizeTiny
                            color: modelData.message_create.sender_id === conversationPage.myUserId ? Theme.highlightColor : Theme.primaryColor
                            horizontalAlignment: (modelData.message_create.sender_id === conversationPage.myUserId) ? Text.AlignRight : Text.AlignLeft
                        }

                    }

                }

                footer: footerComponent

                VerticalScrollDecorator { flickable: conversationListView }
            }

            Component {
                id: footerComponent
                Row {
                    id: newMessageRow
                    width: parent.width - Theme.horizontalPageMargin
                    height: sendMessageColumn.height + ( 2 * Theme.paddingLarge )
                    anchors.left: parent.left
                    spacing: Theme.paddingMedium
                    Column {
                        id: sendMessageColumn
                        width: parent.width - Theme.fontSizeMedium - ( 2 * Theme.paddingMedium )
                        anchors.verticalCenter: parent.verticalCenter
                        TextArea {
                            id: newMessageTextField
                            width: parent.width
                            font.pixelSize: Theme.fontSizeSmall
                            placeholderText: qsTr("New message to %1").arg(conversationModel.user.name)
                            labelVisible: false
                            errorHighlight: remainingCharactersText.text < 0
                        }
                        Text {
                            id: remainingCharactersText
                            text: qsTr("%1 characters left").arg(Number(getRemainingCharacters(newMessageTextField.text, conversationPage.configuration)).toLocaleString(Qt.locale(), "f", 0))
                            color: remainingCharactersText.text < 0 ? Theme.highlightColor : Theme.primaryColor
                            font.pixelSize: Theme.fontSizeTiny
                            font.bold: remainingCharactersText.text < 0 ? true : false
                            anchors.left: parent.left
                            anchors.leftMargin: Theme.horizontalPageMargin
                        }
                    }

                    Column {
                        width: Theme.fontSizeMedium
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: Theme.paddingLarge
                        IconButton {
                            id: newMessageSendButton
                            icon.source: "image://theme/icon-m-chat"
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                twitterApi.directMessagesNew(newMessageTextField.text, conversationModel.user.id_str);
                                newMessageTextField.text = "";
                                newMessageTextField.focus = false;
                            }
                        }
                    }
                }

            }


        }

    }

}

