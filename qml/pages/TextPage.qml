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
import Sailfish.Silica 1.0
import "../js/functions.js" as Functions
import "../components"

Page {
    id: textPage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onEscapePressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        textContainer.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        textContainer.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            textContainer.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            textContainer.scrollToBottom();
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageDown) {
            textContainer.flick(0, - parent.height * 2);
            event.accepted = true;
        }
        if (event.key === Qt.Key_PageUp) {
            textContainer.flick(0, parent.height * 2);
            event.accepted = true;
        }
    }

    property string contentId;
    property bool loading;

    Component.onCompleted: {
        if (textPage.contentId === "tos") {
            textHeader.title = qsTr("Terms of Service");
            textPage.loading = true;
            twitterApi.helpTos();
            return;
        }
        if (textPage.contentId === "privacy") {
            textHeader.title = qsTr("Privacy Policy");
            textPage.loading = true;
            twitterApi.helpPrivacy();
            return;
        }
        textNotification.show("Piepmatz doesn't know what you were asking for!");
    }

    Connections {
        target: twitterApi

        onHelpPrivacySuccessful: {
            textPage.loading = false;
            textContent.text = result.privacy;
        }
        onHelpPrivacyError: {
            textPage.loading = false;
            textNotification.show(errorMessage);
        }
        onHelpTosSuccessful: {
            textPage.loading = false;
            textContent.text = result.tos;
        }
        onHelpTosError: {
            textPage.loading = false;
            textNotification.show(errorMessage);
        }
    }

    Column {
        anchors {
            fill: parent
        }

        id: textLoadingColumn
        Behavior on opacity { NumberAnimation {} }
        opacity: textPage.loading ? 1 : 0
        visible: textPage.loading ? true : false

        LoadingIndicator {
            id: textLoadingIndicator
            visible: textPage.loading
            Behavior on opacity { NumberAnimation {} }
            opacity: textPage.loading ? 1 : 0
            height: parent.height
            width: parent.width
        }
    }

    SilicaFlickable {
        id: textContainer
        contentHeight: column.height
        anchors.fill: parent

        AppNotification {
            id: textNotification
        }

        Column {
            id: column
            width: textPage.width
            spacing: Theme.paddingLarge

            PageHeader {
                id: textHeader
            }

            Text {
                id: textContent
                width: parent.width - 2 * Theme.horizontalPageMargin
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.primaryColor
                linkColor: Theme.highlightColor
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
            }

            VerticalScrollDecorator {}
        }

    }
}
