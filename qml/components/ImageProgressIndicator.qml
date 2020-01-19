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
import Sailfish.Silica 1.0

Item {

    property bool withPercentage : false;
    property bool small : false;
    property Image image;

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    width: small ? parent.width / 2 : parent.width
    height: small ? parent.height / 2 : parent.height

    Behavior on opacity { NumberAnimation {} }
    visible: image.status === ( Image.Loading || Image.Error ) ? true : false
    opacity: image.status === ( Image.Loading || Image.Error ) ? 1 : 0
    ProgressCircle {
        id: imageProgressCircle
        width: withPercentage ? parent.height / 2 : parent.height
        height: withPercentage ? parent.height / 2 : parent.height
        value: image.progress
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

    }
    Text {
        id: imageProgressText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: withPercentage ? qsTr("%1 \%").arg(Math.round(image.progress * 100)) : qsTr("%1").arg(Math.round(image.progress * 100))
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: image.status === Image.Loading ? true : false
    }
    Text {
        id: imageErrorText
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
        text: qsTr("Error")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        visible: image.status === Image.Error ? true : false
    }

}
