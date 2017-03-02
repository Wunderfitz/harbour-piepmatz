/*
  Copyright (C) 2017 Sebastian J. Wolf
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    CoverActionList {
        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: timelineModel.update()
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
        anchors {
            top: parent.top
            topMargin: Theme.paddingMedium
            left: parent.left
            leftMargin: Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.paddingMedium
            bottom: parent.bottom
        }
        model: timelineModel
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
                    Image {
                        id: resultTitlePicture
                        source: display.retweeted_status ? display.retweeted_status.user.profile_image_url_https : display.user.profile_image_url_https
                        width: Theme.fontSizeExtraSmall
                        height: Theme.fontSizeExtraSmall
                        sourceSize {
                            width: Theme.fontSizeExtraSmall
                            height: Theme.fontSizeExtraSmall
                        }
                    }
                    Text {
                        id: resultLabelTitle
                        maximumLineCount: 1
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeTiny
                        font.bold: true
                        text: display.retweeted_status ? display.retweeted_status.user.name : display.user.name
                        width: parent.width
                    }
                }
                Text {
                    id: resultLabelContent
                    maximumLineCount: 2
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    text: display.retweeted_status ? display.retweeted_status.full_text : display.full_text
                    width: parent.width
                    wrapMode: Text.Wrap
                    elide: Text.ElideRight
                }
            }
        }
    }

}

