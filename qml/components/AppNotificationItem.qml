/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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

Item {

    id: notificationItem

    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width
    height: notificationText.height + 2 * Theme.paddingMedium
    visible: false
    opacity: 0
    Behavior on opacity { NumberAnimation {} }

    property string text;
    property string additionalInformation;

    Rectangle {
        id: notificationRectangleBackground
        anchors.fill: parent
        color: "black"
        opacity: 0.6
        radius: parent.width / 15
    }
    Rectangle {
        id: notificationRectangle
        anchors.fill: parent
        color: Theme.highlightColor
        opacity: 0.6
        radius: parent.width / 15
    }

    Text {
        id: notificationText
        color: Theme.primaryColor
        text: notificationItem.text
        font.pixelSize: Theme.fontSizeSmall
        font.bold: true
        width: parent.width - 2 * Theme.paddingMedium
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            twitterApi.handleAdditionalInformation(notificationItem.additionalInformation);
        }
        visible: additionalInformation ? true : false
    }
}
