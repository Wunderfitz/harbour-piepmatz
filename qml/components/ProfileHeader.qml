import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Item {
    id: profileHeader

    property variant profileModel;
    width: parent.width
    height: ((profilePictureColumn.y + profilePictureColumn.height) > (profileOverviewColumn.y + profileOverviewColumn.height) ? (profilePictureColumn.y + profilePictureColumn.height) : (profileOverviewColumn.y + profileOverviewColumn.height)) + Theme.paddingSmall

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


}
