/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"


Page {
    id: attachImagesPage
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        imagesModel.update();
    }

    SilicaFlickable {
        id: attachImagesContainer
        anchors.fill: parent
        contentHeight: attachImagesColumn.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Attach")
                onClicked: {
                    // Attach stuff...
                    pageStack.pop();
                }
            }
        }

        Column {
            id: attachImagesColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Select Images")
            }

            VerticalScrollDecorator {}
        }

    }
}
