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
import "../js/functions.js" as Functions

CoverBackground {

    id: coverPage

    property bool loading : false;
    property bool initialized : false;

    Connections {
        target: timelineModel
        onHomeTimelineStartUpdate: {
            coverPage.loading = true;
        }
        onHomeTimelineUpdated: {
            coverPage.loading = false;
        }
        onHomeTimelineError: {
            coverPage.loading = false;
        }
    }

    Connections {
        target: coverModel
        onCoverInitialized: {
            coverPage.initialized = true;
        }
    }

    Column {
        id: loadingColumn
        width: parent.width - 2 * Theme.horizontalPageMargin
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.paddingMedium
        visible: coverPage.loading
        Behavior on opacity { NumberAnimation {} }
        opacity: coverPage.loading ? 1 : 0
        InfoLabel {
            id: loadingLabel
            text: qsTr("Loading...")
            font.pixelSize: Theme.fontSizeMedium
        }
    }

    CoverActionList {
        enabled: coverPage.initialized
        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: {
                Functions.updatePiepmatz()
            }
        }
    }

    Image {
        source: "../../images/background.png"
        anchors {
            verticalCenter: parent.verticalCenter

            bottom: parent.bottom
            bottomMargin: Theme.paddingMedium

            right: parent.right
            rightMargin: Theme.paddingMedium
        }

        fillMode: Image.PreserveAspectFit
        opacity: 0.15
    }

    SilicaListView {
        id: coverListView
        visible: !coverPage.loading
        Behavior on opacity { NumberAnimation {} }
        opacity: coverPage.loading ? 0 : 1
        anchors {
            top: parent.top
            topMargin: Theme.paddingMedium
            left: parent.left
            leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.paddingMedium
            bottom: parent.bottom
        }
        model: coverModel
        delegate: ListItem {
            anchors {
                topMargin:  Theme.paddingMedium
            }
            height: resultLabelTitle.height + resultLabelContent.height + Theme.paddingSmall
            opacity: index < 4 ? 1.0 - index * 0.2 : 0.0

            Column {
                width: parent.width
                Row {
                    id: resultTitleRow
                    spacing: Theme.paddingSmall
                    width: parent.width
                    Image {
                        id: resultTitlePicture
                        source: display.image
                        width: Theme.fontSizeExtraSmall
                        height: Theme.fontSizeExtraSmall
                        sourceSize {
                            width: Theme.fontSizeExtraSmall
                            height: Theme.fontSizeExtraSmall
                        }
                    }
                    Text {
                        id: resultLabelTitle
                        width: parent.width - Theme.paddingSmall - resultTitlePicture.width
                        maximumLineCount: 1
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeTiny
                        font.bold: true
                        text: display.name
                        elide: Text.ElideRight
                    }
                }
                Text {
                    id: resultLabelContent
                    maximumLineCount: 2
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    text: display.text
                    textFormat: Text.StyledText
                    width: parent.width
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                }
            }
        }
    }

}

