import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"
import "../js/functions.js" as Functions

Page {
    id: conversationPage
    allowedOrientations: Orientation.All

    property variant conversationModel;
    property string myUserId;
    property bool loaded : true;

    Notification {
        id: conversationNotification
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

            anchors.fill: parent

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
    }
}

