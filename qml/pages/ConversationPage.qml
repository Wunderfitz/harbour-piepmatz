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

    Notification {
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
        anchors.top: profileHeader.bottom

        LoadingIndicator {
            id: conversationLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        SilicaListView {
            id: conversationListView
            Component.onCompleted: positionViewAtEnd();

            width: parent.width
            height: parent.height - newMessageRow.height

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

            VerticalScrollDecorator {}
        }


        Row {
            id: newMessageRow
            width: parent.width - Theme.horizontalPageMargin
            height: sendMessageColumn.height + ( 2 * Theme.paddingLarge )
            anchors.top: conversationListView.bottom
            anchors.left: parent.left
            spacing: Theme.paddingMedium
            Column {
                width: parent.width - Theme.fontSizeMedium - ( 2 * Theme.paddingMedium )
                anchors.verticalCenter: parent.verticalCenter
                TextField {
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
                id: sendMessageColumn
                width: Theme.fontSizeMedium
                anchors.verticalCenter: parent.verticalCenter
                IconButton {
                    id: newMessageSendButton
                    icon.source: "image://theme/icon-m-bubble-universal"
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

