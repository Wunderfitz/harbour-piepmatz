/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: titlePage
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
        }

        contentHeight: column.height

        Column {
            id: column

            width: titlePage.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("Piepmatz")
            }
        }
    }
}

