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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji
import "../js/twitter-text.js" as TwitterText

Item {

    id: profileItem
    height: parent.height
    width: parent.width
    visible: profileModel ? true : false
    opacity: profileModel ? 1 : 0
    Behavior on opacity { NumberAnimation {} }

    function scrollDown() {
        profileTimelineListView.flick(0, - parent.height);
    }
    function scrollUp() {
        profileTimelineListView.flick(0, parent.height);
    }
    function pageDown() {
        profileTimelineListView.flick(0, - parent.height * 2);
    }
    function pageUp() {
        profileTimelineListView.flick(0, parent.height * 2);
    }
    function scrollToTop() {
        profileTimelineListView.scrollToTop();
    }
    function scrollToBottom() {
        profileTimelineListView.scrollToBottom();
    }

    property variant profileModel;
    property variant profileTimeline;
    property bool loadingError : false;
    property string componentFontSize: ( accountModel.getFontSize() === "piepmatz" ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall) ;
    property string iconFontSize: ( accountModel.getFontSize() === "piepmatz" ? Theme.fontSizeSmall : Theme.fontSizeMedium) ;

    Component.onCompleted: {
        console.log("Profile Model called for " + profileModel.id_str + ": " + profileModel.screen_name);
        twitterApi.userTimeline(profileModel.screen_name);
    }

    onProfileModelChanged: {
        //profileHeader.profileModel = profileItem.profileModel;
        profileTimeline = null;
        twitterApi.userTimeline(profileModel.screen_name);
    }

    AppNotification {
        id: notification
    }

    Connections {
        target: accountModel
        onFontSizeChanged: {
            if (fontSize === "piepmatz") {
                componentFontSize = Theme.fontSizeExtraSmall;
                iconFontSize = Theme.fontSizeSmall;
            } else {
                componentFontSize = Theme.fontSizeSmall;
                iconFontSize = Theme.fontSizeMedium;
            }
        }
    }

    Connections {
        target: twitterApi
        onUserTimelineSuccessful: {
            if (!profileTimeline) {
                console.log("Timeline updated for user " + profileModel.screen_name)
                profileTimeline = result;
            }
        }
        onUserTimelineError: {
            if (!profileTimeline) {
                loadingError = true;
                notification.show(errorMessage);
            }
        }
    }

    Component {
        id: profileListHeaderComponent
        Column {
            id: profileElementsColumn

            height: profileFollowingRow.height + profileHeader.height + profileActivityRow.height + profileItemColumn.height + ( 3 * Theme.paddingMedium )
            width: parent.width
            spacing: Theme.paddingMedium

            ProfileHeader {
                id: profileHeader
                profileModel: profileItem.profileModel
                width: parent.width
            }

            Row {
                id: profileFollowingRow
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                spacing: Theme.paddingMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Text {
                    id: profileFriendsText
                    text: qsTr("%1 Following").arg(Number(profileModel.friends_count).toLocaleString(Qt.locale(), "f", 0))
                    font.pixelSize: componentFontSize
                    color: Theme.highlightColor
                    font.underline: !profileItem.loadingError
                    wrapMode: Text.Wrap
                    MouseArea {
                        enabled: !profileItem.loadingError
                        anchors.fill: parent
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/FriendsPage.qml"), { "screenName" : profileModel.screen_name, "userName" : profileModel.name });
                        }
                    }
                }
                Text {
                    id: profileFollowingSeparatorText
                    text: "|"
                    font.pixelSize: componentFontSize
                    color: Theme.primaryColor
                }
                Text {
                    id: profileFollowersText
                    text: qsTr("%1 Followers").arg(Number(profileModel.followers_count).toLocaleString(Qt.locale(), "f", 0))
                    font.pixelSize: componentFontSize
                    color: Theme.highlightColor
                    font.underline: !profileItem.loadingError
                    wrapMode: Text.Wrap
                    MouseArea {
                        enabled: !profileItem.loadingError
                        anchors.fill: parent
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/FollowersPage.qml"), { "screenName" : profileModel.screen_name, "userName" : profileModel.name });
                        }
                    }
                }
            }

            Row {
                id: profileActivityRow
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                spacing: Theme.paddingMedium
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Text {
                    id: profileTweetsText
                    text: qsTr("%1 Tweets").arg(Number(profileModel.statuses_count).toLocaleString(Qt.locale(), "f", 0))
                    font.pixelSize: componentFontSize
                    font.underline: !profileItem.loadingError
                    color: Theme.highlightColor
                    MouseArea {
                        enabled: !profileItem.loadingError
                        anchors.fill: parent
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/UserTimelinePage.qml"), { "screenName" : profileModel.screen_name, "userName" : profileModel.name, "userTimelineModel": profileTimeline });
                        }
                    }
                }
                Text {
                    id: profileActivitySeparatorText
                    text: "|"
                    font.pixelSize: componentFontSize
                    color: Theme.primaryColor
                }
                Text {
                    id: profileFavoritesText
                    text: qsTr("%1 Favorites").arg(Number(profileModel.favourites_count).toLocaleString(Qt.locale(), "f", 0))
                    font.pixelSize: componentFontSize
                    font.underline: !profileItem.loadingError
                    color: Theme.highlightColor
                    MouseArea {
                        enabled: !profileItem.loadingError
                        anchors.fill: parent
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("../pages/FavoritesPage.qml"), { "screenName" : profileModel.screen_name, "userName" : profileModel.name });
                        }
                    }
                }
            }

            Column {
                id: profileItemColumn
                width: parent.width
                spacing: Theme.paddingMedium

                Row {
                    id: profileDetailsRow
                    spacing: Theme.paddingMedium
                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                    visible: profileModel.description ? true : false
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        id: profileDescriptionText
                        text: Emoji.emojify(TwitterText.autoLink(profileModel.description, { usernameIncludeSymbol : true }), componentFontSize)
                        font.pixelSize: componentFontSize
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        width: parent.width
                        textFormat: Text.StyledText
                        onLinkActivated: {
                            Functions.handleLink(link);
                        }
                        linkColor: Theme.highlightColor
                    }
                }

                Row {
                    id: profileJoinedRow
                    spacing: Theme.paddingMedium
                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        id: profileJoinedText
                        text: qsTr("Joined in %1").arg(Functions.getValidDate(profileModel.created_at).toLocaleDateString(Qt.locale(), "MMMM yyyy"))
                        font.pixelSize: componentFontSize
                        color: Theme.primaryColor
                        wrapMode: Text.NoWrap
                        elide: Text.ElideRight
                    }
                }


                Row {
                    id: profileLocationRow
                    spacing: Theme.paddingMedium
                    width: parent.width - ( 2 * Theme.horizontalPageMargin )
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                    Row {
                        visible: profileModel.location.length === 0 ? false : true
                        width: profileModel.location.length === 0 ? 0 : ( profileModel.entities.url ? parent.width / 2 : parent.width )
                        Image {
                            id: profileLocationImage
                            source: "image://theme/icon-m-location"
                            width: iconFontSize
                            height: iconFontSize
                        }
                        Text {
                            id: profileLocationText
                            text: Emoji.emojify(profileModel.location, componentFontSize)
                            font.pixelSize: componentFontSize
                            color: Theme.primaryColor
                            wrapMode: Text.NoWrap
                            anchors.verticalCenter: parent.verticalCenter
                            elide: Text.ElideRight
                            width: parent.width - profileLocationImage.width
                        }
                    }

                    Row {
                        visible: profileModel.entities.url ? true : false
                        width: profileModel.entities.url ? ( profileModel.location.length === 0 ? parent.width : parent.width / 2 ) : 0
                        Image {
                            id: profileUrlImage
                            source: "image://theme/icon-m-link"
                            width: iconFontSize
                            height: iconFontSize
                        }
                        Text {
                            id: profileUrlText
                            text: profileModel.entities.url ? ("<a href=\"" + profileModel.entities.url.urls[0].url + "\">" + profileModel.entities.url.urls[0].display_url + "</a>") : ""
                            font.pixelSize: componentFontSize
                            color: Theme.primaryColor
                            wrapMode: Text.NoWrap
                            anchors.verticalCenter: parent.verticalCenter
                            onLinkActivated: Qt.openUrlExternally(profileModel.entities.url.urls[0].url)
                            linkColor: Theme.highlightColor
                            elide: Text.ElideRight
                            width: parent.width - profileUrlImage.width
                        }
                    }
                }

                Separator {
                    id: profileSeparator
                    width: parent.width
                    color: Theme.primaryColor
                    horizontalAlignment: Qt.AlignHCenter
                }
            }

        }

    }


    Item {
        id: profileTimelineLoadingIndicator
        visible: profileTimeline || profileItem.loadingError ? false : true
        Behavior on opacity { NumberAnimation {} }
        opacity: profileTimeline ? 0 : 1

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Rectangle {
            id: profileTimelineLoadingOverlay
            color: "black"
            opacity: 0.4
            width: parent.width
            height: parent.height
            visible: profileTimelineLoadingIndicator.visible
        }

        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            running: profileTimelineLoadingIndicator.visible
            size: BusyIndicatorSize.Large
        }
    }

    SilicaListView {
        id: profileTimelineListView

        header: profileListHeaderComponent

        anchors {
            top: parent.top
            topMargin: Theme.paddingSmall
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        clip: true

        model: profileTimeline
        delegate: Tweet {
            tweetModel: modelData
        }
        VerticalScrollDecorator {}
    }



}
