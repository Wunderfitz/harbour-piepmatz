import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Item {

    id: profileItem
    height: parent.height
    width: parent.width
    visible: profileModel ? true : false
    opacity: profileModel ? 1 : 0
    Behavior on opacity { NumberAnimation {} }

    property variant profileModel;
    property variant profileTimeline;
    property bool loadingError : false;

    Component.onCompleted: {
        console.log("Profile Model called for " + profileModel.screen_name);
        twitterApi.userTimeline(profileModel.screen_name);
    }

    onProfileModelChanged: {
        profileTimeline = null;
        twitterApi.userTimeline(profileModel.screen_name);
    }

    Notification {
        id: notification
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

    Item {
        id: profileBackgroundItem
        width: parent.width
        height: appWindow.height / 5
        Rectangle {
            id: profileBackgroundColor
            color: "#" + profileModel.profile_link_color
            anchors.fill: parent
        }
        Component {
            id: profileBannerComponent
            Image {
                id: profileBackgroundImage
                source: profileModel.profile_banner_url
            }
        }

        Loader {
            id: profileBannerLoader
            active: profileModel.profile_banner_url ? true : false
            sourceComponent: profileBannerComponent
            anchors.fill: parent
        }
    }

    Column {
        id: profilePictureColumn
        width: appWindow.width > appWindow.height ? ( appWindow.height / 3 ) : ( appWindow.width / 3 )
        height: appWindow.width > appWindow.height ? ( appWindow.height / 3 ) : ( appWindow.width / 3 )
        x: Theme.horizontalPageMargin
        anchors {
            verticalCenter: profileBackgroundItem.bottom
        }
        Item {
            id: profilePictureItem
            width: parent.width
            height: parent.height
            Rectangle {
                id: profilePictureBackground
                width: parent.width
                height: parent.height
                color: Theme.primaryColor
                radius: parent.width / 6
                anchors {
                    margins: Theme.horizontalPageMargin
                }
                visible: profilePicture.status === Image.Ready ? true : false
                opacity: profilePicture.status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
            }

            Component {
                id: singleImageComponent
                ImagePage {
                    imageUrl: Functions.findHiResImage(profileModel.profile_image_url_https)
                    imageHeight: appWindow.width > appWindow.height ? appWindow.height : appWindow.width
                    imageWidth: appWindow.width > appWindow.height ? appWindow.height : appWindow.width
                }
            }

            Image {
                id: profilePicture
                source: Functions.findHiResImage(profileModel.profile_image_url_https)
                width: parent.width - parent.width / 10
                height: parent.height - parent.height / 10
                anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                anchors.centerIn: profilePictureBackground
                visible: false
            }

            Rectangle {
                id: profilePictureMask
                width: parent.width - parent.width / 10
                height: parent.height - parent.height / 10
                color: Theme.primaryColor
                radius: parent.width / 7
                anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                anchors.centerIn: profilePictureBackground
                visible: false
            }

            OpacityMask {
                id: maskedProfilePicture
                source: profilePicture
                maskSource: profilePictureMask
                anchors.fill: profilePicture
                visible: profilePicture.status === Image.Ready ? true : false
                opacity: profilePicture.status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation {} }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageStack.push( singleImageComponent );
                    }
                }
            }

            ImageProgressIndicator {
                image: profilePicture
                withPercentage: false
            }

        }
    }

    Column {
        id: profileOverviewColumn
        width: appWindow.width > appWindow.height ? ( appWindow.height * 2 / 3 ) : ( appWindow.width * 2 / 3 )
        height: profileNameText.height + profileScreenNameText.height + ( Theme.paddingMedium )
        spacing: Theme.paddingSmall
        anchors {
            top: profileBackgroundItem.bottom
            topMargin: Theme.paddingMedium
            left: profilePictureColumn.right
            leftMargin: Theme.horizontalPageMargin
        }
        Text {
            id: profileNameText
            text: profileModel.name
            font {
                pixelSize: Theme.fontSizeMedium
                bold: true
            }
            color: Theme.primaryColor
            elide: Text.ElideRight
            width: parent.width
        }
        Text {
            id: profileScreenNameText
            text: qsTr("@%1").arg(profileModel.screen_name)
            font {
                pixelSize: Theme.fontSizeSmall
                bold: true
            }
            color: Theme.primaryColor
            elide: Text.ElideRight
            width: parent.width
        }
    }

    Row {
        id: profileFollowingRow
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        spacing: Theme.paddingMedium
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: (profilePictureColumn.y + profilePictureColumn.height) > (profileOverviewColumn.y + profileOverviewColumn.height) ? profilePictureColumn.bottom : profileOverviewColumn.bottom
            topMargin: Theme.paddingMedium
        }
        Text {
            id: profileFriendsText
            text: qsTr("%1 Following").arg(Number(profileModel.friends_count).toLocaleString(Qt.locale(), "f", 0))
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
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
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
        }
        Text {
            id: profileFollowersText
            text: qsTr("%1 Followers").arg(Number(profileModel.followers_count).toLocaleString(Qt.locale(), "f", 0))
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
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
            top: profileFollowingRow.bottom
            topMargin: Theme.paddingMedium
        }
        Text {
            id: profileTweetsText
            text: qsTr("%1 Tweets").arg(Number(profileModel.statuses_count).toLocaleString(Qt.locale(), "f", 0))
            font.pixelSize: Theme.fontSizeExtraSmall
            font.underline: !profileItem.loadingError
            color: Theme.primaryColor
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
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
        }
        Text {
            id: profileJoinedText
            text: qsTr("Joined in %1").arg(Functions.getValidDate(profileModel.created_at).toLocaleDateString(Qt.locale(), "MMMM yyyy"))
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            wrapMode: Text.Wrap
        }
    }

    Column {
        id: profileItemColumn
        width: parent.width
        spacing: Theme.paddingMedium

        anchors {
            top: profileActivityRow.bottom
            topMargin: Theme.paddingMedium
        }

        Row {
            id: profileDetailsRow
            spacing: Theme.paddingMedium
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: profileDescriptionText
                text: Functions.enhanceDescription(profileModel.description);
                font.pixelSize: Theme.fontSizeExtraSmall
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
            id: profileLocationRow
            spacing: Theme.paddingMedium
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            Row {
                visible: profileModel.location.length === 0 ? false : true
                Image {
                    id: profileLocationImage
                    source: "image://theme/icon-m-location"
                    width: Theme.fontSizeSmall
                    height: Theme.fontSizeSmall
                }
                Text {
                    id: profileLocationText
                    text: profileModel.location
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Row {
            id: profileUrlRow
            spacing: Theme.paddingMedium
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            Row {
                visible: profileModel.entities.url ? true : false
                Image {
                    id: profileUrlImage
                    source: "image://theme/icon-m-link"
                    width: Theme.fontSizeSmall
                    height: Theme.fontSizeSmall
                }
                Text {
                    id: profileUrlText
                    text: profileModel.entities.url ? ("<a href=\"" + profileModel.entities.url.urls[0].url + "\">" + profileModel.entities.url.urls[0].display_url + "</a>") : ""
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    anchors.verticalCenter: parent.verticalCenter
                    onLinkActivated: Qt.openUrlExternally(profileModel.entities.url.urls[0].url)
                    linkColor: Theme.highlightColor
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

    Item {
        id: profileTimelineLoadingIndicator
        visible: profileTimeline || profileItem.loadingError ? false : true
        Behavior on opacity { NumberAnimation {} }
        opacity: profileTimeline ? 0 : 1

        anchors {
            top: profileItemColumn.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
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

        anchors {
            top: profileItemColumn.bottom
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
