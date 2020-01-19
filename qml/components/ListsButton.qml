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

Column {
    id: listsButton
    property bool isActive: false

    width: parent.width
    IconButton {
        id: listsButtonImage
        height: Theme.iconSizeMedium
        width: Theme.iconSizeMedium
        icon.source: listsButton.isActive ? "image://theme/icon-m-note?" + Theme.highlightColor : "image://theme/icon-m-note?" + Theme.primaryColor
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        onClicked: {
            handleListsClicked();
        }
    }
    Label {
        id: listsButtonText
        text: qsTr("Lists")
        font.pixelSize: Theme.fontSizeTiny * 7 / 8
        color: listsButton.isActive ? Theme.highlightColor : Theme.primaryColor
        truncationMode: TruncationMode.Fade
        anchors {
            horizontalCenter: parent.horizontalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked: handleListsClicked();
        }
    }
}
