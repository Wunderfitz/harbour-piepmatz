import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../js/functions.js" as Functions

Item {

    property variant accountModel;

    id: profileComponent
    height: profileItemColumn.height
    width: parent.width

    Column {
        id: profileItemColumn
        width: parent.width
        spacing: 2 * Theme.horizontalPageMargin

        Row {
            id: profileTitleRow
            spacing: Theme.horizontalPageMargin
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            Column {
                id: profilePictureColumn
                width: parent.width / 3
                height: parent.width / 3
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    topMargin: Theme.horizontalPageMargin
                    top: parent.top
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
                    }

                    Image {
                        id: profilePicture
                        source: Functions.findHiResImage(accountModel.profile_image_url_https)
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
                    }

                }
            }

            Column {
                id: profileOverviewColumn
                width: parent.width * 2 / 3
                spacing: Theme.paddingSmall
                anchors {
                    leftMargin: Theme.horizontalPageMargin
                    topMargin: Theme.horizontalPageMargin
                    top: parent.top
                }
                Text {
                    id: profileNameText
                    text: accountModel.name
                    font {
                        pixelSize: Theme.fontSizeLarge
                        bold: true
                    }
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                }
                Text {
                    id: profileScreenNameText
                    text: qsTr("@%1").arg(accountModel.screen_name)
                    font {
                        pixelSize: Theme.fontSizeMedium
                        bold: true
                    }
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                }
                Text {
                    id: profileFriendsText
                    text: qsTr("%1 Following").arg(accountModel.friends_count)
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                }
                Text {
                    id: profileFollowersText
                    text: qsTr("%1 Followers").arg(accountModel.followers_count)
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                }
            }
        }

        Column {
            id: profileDetailsColumn
            spacing: Theme.paddingLarge
            width: parent.width - ( 2 * Theme.horizontalPageMargin )
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: profileDescriptionText
                text: accountModel.description
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.primaryColor
                wrapMode: Text.Wrap
                width: parent.width
            }
            Row {
                id: profileMoreInformationRow
                spacing: Theme.horizontalPageMargin
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Text {
                    id: profileTweetsText
                    text: qsTr("%1 Tweets").arg(accountModel.statuses_count)
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                }
                Text {
                    id: profileMoreInfoSeparatorText
                    text: qsTr("|")
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                }
                Text {
                    id: profileFavoritesText
                    text: qsTr("%1 Favorites").arg(accountModel.favourites_count)
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                }
            }
            Text {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                id: profileJoinedText
                text: qsTr("Joined in %1").arg(Functions.getValidDate(accountModel.created_at).toLocaleDateString(Qt.locale(), "MMMM yyyy"))
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.primaryColor
                wrapMode: Text.Wrap
            }

            Row {
                id: profileLocationRow
                visible: accountModel.location.length === 0 ? false : true
                spacing: Theme.horizontalPageMargin
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Image {
                    id: profileLocationImage
                    source: "image://theme/icon-m-location"
                }
                Text {
                    id: profileLocationText
                    text: accountModel.location
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Row {
                id: profileUrlRow
                spacing: Theme.horizontalPageMargin
                visible: accountModel.entities.url.urls.length === 0 ? false : true
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                Image {
                    id: profileUrlImage
                    source: "image://theme/icon-m-link"
                }
                Text {
                    id: profileUrlText
                    text: "<a href=\"" + accountModel.entities.url.urls[0].url + "\">" + accountModel.entities.url.urls[0].accountModel_url + "</a>"
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    anchors.verticalCenter: parent.verticalCenter
                    onLinkActivated: Qt.openUrlExternally(accountModel.entities.url.urls[0].url)
                    linkColor: Theme.highlightColor
                }
            }

        }
    }

}
