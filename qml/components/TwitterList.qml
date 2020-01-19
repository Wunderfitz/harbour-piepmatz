/*
    Copyright (C) 2017-20 Sebastian J. Wolf

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

ListItem {

    id: singleList

    property variant listModel;

    contentHeight: listRow.height + listSeparator.height + 2 * Theme.paddingMedium
    contentWidth: parent.width

    onClicked: {
        pageStack.push(Qt.resolvedUrl("../pages/ListTimelinePage.qml"), {"listId": listModel.id_str, "listName": listModel.name});
    }

    Column {
        id: listColumn
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        spacing: Theme.paddingSmall
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Row {
            id: listRow
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
                        source: Functions.findBiggerImage(listModel.user.profile_image_url_https)
                        width: parent.width
                        height: parent.height
                        visible: false
                    }

                    Rectangle {
                        id: userPictureErrorShade
                        z: 42
                        width: parent.width
                        height: parent.height
                        color: "lightgrey"
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
                        source: userPicture.status === Image.Error ? userPictureErrorShade : userPicture
                        maskSource: userPictureMask
                        anchors.fill: userPicture
                        visible: ( userPicture.status === Image.Ready || userPicture.status === Image.Error ) ? true : false
                        opacity: userPicture.status === Image.Ready ? 1 : ( userPicture.status === Image.Error ? 0.3 : 0 )
                        Behavior on opacity { NumberAnimation {} }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileModel": listModel.user});
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

                Text {
                    id: listNameText
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                    text: qsTr("<b>%1</b> by %2").arg(Emoji.emojify(listModel.name, Theme.fontSizeExtraSmall)).arg(Emoji.emojify(listModel.user.name, Theme.fontSizeExtraSmall))
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    width: parent.width
                    textFormat: Text.StyledText
                    onTruncatedChanged: {
                        // There is obviously a bug in QML in truncating text with images.
                        // We simply remove Emojis then...
                        if (truncated) {
                            text = text.replace(/\<img [^>]+\/\>/g, "");
                        }
                    }
                }

                Text {
                    id: listsMembersText
                    text: qsTr("%1 members").arg(listModel.member_count)
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
        id: listSeparator

        anchors {
            top: listColumn.bottom
            topMargin: Theme.paddingMedium
        }

        width: parent.width
        color: Theme.primaryColor
        horizontalAlignment: Qt.AlignHCenter
    }

}
