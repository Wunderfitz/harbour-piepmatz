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
import QtQuick 2.5
import Sailfish.Silica 1.0
import "../components"


Page {
    id: attachImagesPage
    allowedOrientations: Orientation.All

    Component.onCompleted: {
        attachImagesPage.searchInProgress = true;
        imagesModel.update();
    }

    AppNotification {
        id: attachImageNotification
    }

    Connections {
        target: imagesModel
        onSearchFinished: {
            attachImagesPage.searchInProgress = false;
        }
    }

    property bool searchInProgress: false;
    property var selectedImages: [];

    function selectImage(fileName) {
        var imageIndex = selectedImages.indexOf(fileName);
        if (imageIndex === -1) {
            if (selectedImages.length < 4) {
                selectedImages.push(fileName);
            } else {
                attachImageNotification.show(qsTr("Maximum number of images selected!"));
            }
        } else {
            selectedImages.splice(imageIndex, 1);
        }
        attachImagesPullDown.visible = attachImagesPage.selectedImages.length > 0 ? true : false;
        attachImageMenuItem.text = attachImagesPage.selectedImages.length > 1 ? qsTr("Attach %1 Images").arg(attachImagesPage.selectedImages.length) : qsTr("Attach Image");
    }

    function isImageSelected(fileName) {
        return selectedImages.indexOf(fileName) === -1 ? false : true;
    }

    LoadingIndicator {
        id: imagesLoadingIndicator
        visible: attachImagesPage.searchInProgress
        Behavior on opacity { NumberAnimation {} }
        opacity: attachImagesPage.searchInProgress ? 1 : 0
        height: parent.height
        width: parent.width
    }

    SilicaFlickable {
        id: attachImagesContainer
        anchors.fill: parent
        contentHeight: attachImagesColumn.height

        PullDownMenu {
            id: attachImagesPullDown
            visible: attachImagesPage.selectedImages.length > 0 ? true : false
            MenuItem {
                id: attachImageMenuItem
                onClicked: {
                    imagesModel.setSelectedImages(attachImagesPage.selectedImages);
                    pageStack.pop();
                }
            }
        }

        Column {
            id: attachImagesColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                id: imagesHeader
                title: qsTr("Select Images")
            }

            SilicaGridView {

                id: imagesGridView

                height: attachImagesPage.height - imagesHeader.height - Theme.paddingLarge
                width: parent.width
                cellWidth: width / 3;
                cellHeight: width / 3;

                anchors.left: parent.left
                anchors.right: parent.right

                Behavior on opacity { NumberAnimation {} }
                opacity: attachImagesPage.searchInProgress ? 0 : 1

                clip: true

                model: imagesModel

                delegate:  Item {
                    width: imagesGridView.cellWidth
                    height: imagesGridView.cellHeight
                    Image {
                        id: singleImage
                        width: imagesGridView.cellWidth
                        height: imagesGridView.cellHeight
                        source: display
                        sourceSize {
                            width: imagesGridView.cellWidth
                            height: imagesGridView.cellHeight
                        }

                        fillMode: Image.PreserveAspectCrop
                        autoTransform: true
                        asynchronous: true
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                selectImage(display);
                                selectionRectangle.visible = isImageSelected(display);
                            }
                        }
                    }
                    ImageProgressIndicator {
                        image: singleImage
                        small: true
                    }
                    Rectangle {
                        id: selectionRectangle
                        border {
                            color: Theme.highlightColor
                            width: Theme.paddingSmall
                        }
                        color: "transparent"
                        width: imagesGridView.cellWidth
                        height: imagesGridView.cellHeight
                        visible: isImageSelected(display)
                    }
                }

                VerticalScrollDecorator {}

            }
        }

    }
}
