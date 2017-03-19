/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        id: aboutContainer
        contentHeight: column.height
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Tweet")
                onClicked: {
                    twitterApi.tweet(enterTweetTextArea.text)
                    pageStack.pop()
                }
            }
        }

        Column {
            id: column
            width: page.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("New Tweet")
            }

            TextArea {
                id: enterTweetTextArea
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width - 2 * Theme.paddingLarge
                focus: true
            }

            VerticalScrollDecorator {}
        }

    }
}
