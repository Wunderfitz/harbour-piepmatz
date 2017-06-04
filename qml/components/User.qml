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
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

ListItem {

    id: singleUser

    property variant userModel;

    contentHeight: userRow.height + userSeparator.height + 2 * Theme.paddingMedium
    contentWidth: parent.width

    onClicked: {
        pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": userModel});
    }

    Column {
        id: userColumn
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        spacing: Theme.paddingSmall
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Row {
            id: userRow
            width: parent.width
            spacing: Theme.paddingMedium

            Column {
                id: userPictureColumn
                width: parent.width / 6
                height: parent.width / 6
                spacing: Theme.paddingSmall

                Item {
                    id: userPictureItem
                    width: parent.width
                    height: parent.width

                    Image {
                        id: userPicture
                        z: 42
                        source: Functions.findBiggerImage(userModel.profile_image_url_https)
                        width: parent.width
                        height: parent.height
                        sourceSize {
                            width: parent.width
                            height: parent.height
                        }
                        visible: false
                    }

                    Rectangle {
                        id: userPictureMask
                        z: 42
                        width: parent.width
                        height: parent.height
                        color: Theme.primaryColor
                        radius: parent.width / 7
                        visible: false
                    }

                    OpacityMask {
                        id: maskedUserPicture
                        z: 42
                        source: userPicture
                        maskSource: userPictureMask
                        anchors.fill: userPicture
                        visible: userPicture.status === Image.Ready ? true : false
                        opacity: userPicture.status === Image.Ready ? 1 : 0
                        Behavior on opacity { NumberAnimation {} }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": userModel});
                            }
                        }
                    }

                    ImageProgressIndicator {
                        image: userPicture
                        withPercentage: false
                    }

                }
            }

            Column {
                id: userContentColumn
                width: parent.width * 5 / 6 - Theme.horizontalPageMargin

                spacing: Theme.paddingSmall

                TweetUser {
                    id: tweetUserRow
                    tweetUser: userModel
                }

                Text {
                    id: userDescriptionText
                    text: Functions.enhanceDescription(userModel.description);
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
        }

    }

    Separator {
        id: userSeparator

        anchors {
            top: userColumn.bottom
            topMargin: Theme.paddingMedium
        }

        width: parent.width
        color: Theme.primaryColor
        horizontalAlignment: Qt.AlignHCenter
    }

}
