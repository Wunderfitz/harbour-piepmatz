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
import QtQuick 2.2
import Sailfish.Silica 1.0
import "../js/functions.js" as Functions
import "../components"

Page {
    id: registrationPage
    allowedOrientations: Orientation.All

    property bool registrationLoading : false;

    Component.onCompleted: {
        if (wagnis.isRegistered()) {
            surveyFlickable.visible = true;
            surveyFlickable.opacity = 1;
        } else {
            registrationFlickable.visible = true;
            registrationFlickable.opacity = 1;
        }
    }

    Connections {
        target: wagnis
        onRegistrationError: {
            registrationPage.registrationLoading = false;
            registrationErrorFlickable.visible = true;
            registrationErrorFlickable.opacity = 1;
        }
        onRegistrationInvalid: {
            registrationPage.registrationLoading = false;
            registrationInvalidFlickable = true;
            registrationInvalidFlickable.opacity = 1;
        }
        onRegistrationValid: {
            registrationPage.registrationLoading = false;
            if (wagnis.hasFeature("survey")) {
                pageStack.clear();
                accountModel.isLinked() ? pageStack.push(overviewPage) : pageStack.push(welcomePage);
            } else {
                surveyFlickable.visible = true;
                surveyFlickable.opacity = 1;
            }
        }
    }


    Column {
        anchors {
            fill: parent
        }

        id: registrationLoadingColumn
        Behavior on opacity { NumberAnimation {} }
        opacity: registrationPage.registrationLoading ? 1 : 0
        visible: registrationPage.registrationLoading ? true : false

        LoadingIndicator {
            id: registrationLoadingIndicator
            visible: registrationPage.registrationLoading
            Behavior on opacity { NumberAnimation {} }
            opacity: registrationPage.registrationLoading ? 1 : 0
            height: parent.height
            width: parent.width
            withOverlay: false
        }
    }


    SilicaFlickable {
        id: registrationFlickable
        anchors.fill: parent
        contentHeight: termsOfUseColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: termsOfUseColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Welcome to Piepmatz!")
            }

            Image {
                id: piepmatzImage
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            SectionHeader {
                text: qsTr("Committed to Free Software")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Welcome to Piepmatz - a Twitter client for SailfishOS! Piepmatz is <a href=\"https://www.gnu.org/philosophy/free-sw\">free software</a> and licensed under the conditions of the <a href=\"https://www.gnu.org/licenses/gpl.html\">General Public License in version 3 (GPLv3)</a>. By using this software, you accept the terms and conditions of this license.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            SectionHeader {
                text: qsTr("Anonymous Registration")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Moreover, Piepmatz will register this installation automatically on my server if you press 'Accept'. I respect your privacy, therefore only anonymous and a very limited set of information will be transmitted: a unique identifer for this installation and your country. I simply would like to know how many users Piepmatz has and where they come from. If this is not OK for you, simply close the application. No information has been transmitted so far.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Text {
                id: candidateRegistrationData
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.primaryColor
                textFormat: Text.StyledText
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                visible: false
            }

            Button {
                text: qsTr("Show Registration Data")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    candidateRegistrationData.text = wagnis.getCandidateRegistrationData();
                    visible = false;
                    candidateRegistrationData.visible = true;
                }
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("The data isn't shared with anyone else. Please see the <a href=\"http://legal.ygriega.de/\">legal notice</a> for additional information how the data is processed. For additional details about the motivation for this, the unique ID and how it is generated please see my page <a href=\"http://wagnis.ygriega.de/\">about Wagnis</a>.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }

            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Have fun with Piepmatz! Sebastian J. Wolf")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Accept")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationFlickable.opacity = 0;
                    registrationFlickable.visible = false;
                }
            }

            Label {
                id: separatorLabel
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

        }

    }


    SilicaFlickable {
        id: registrationErrorFlickable
        anchors.fill: parent
        contentHeight: registrationErrorColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0


        Column {
            id: registrationErrorColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Registration Error")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Registration failed. Please ensure that your device is connected to the Internet and press 'Restart Registration'. In case a restart doesn't work, please contact me via <a href=\"mailto:sebastian@ygriega.de\">E-Mail</a>")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Restart Registration")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationErrorFlickable.visible = false;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: registrationInvalidFlickable
        anchors.fill: parent
        contentHeight: registrationInvalidColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        Column {
            id: registrationInvalidColumn
            width: parent.width
            spacing: Theme.paddingLarge

            Image {
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            InfoLabel {
                text: qsTr("Registration Invalid")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("The registration file on your device is corrupt. The registration process needs to be restarted. Please ensure that your device is connected to the Internet and press 'Restart Registration'. In case the new registration isn't successful, please contact me via <a href=\"mailto:sebastian@ygriega.de\">E-Mail</a>")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                text: qsTr("Restart Registration")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.registerApplication();
                    registrationPage.registrationLoading = true;
                    registrationInvalidFlickable.visible = false;
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    SilicaFlickable {
        id: surveyFlickable
        anchors.fill: parent
        contentHeight: surveyColumn.height
        Behavior on opacity { NumberAnimation {} }
        visible: false
        opacity: 0

        property bool canSkip: false
        property string remainingTime

        onVisibleChanged: {
            if (visible) {
                surveyFlickable.canSkip = wagnis.inTestingPeriod();
                surveyFlickable.remainingTime = Functions.getTimeRemaining(wagnis.getRemainingTime());
            }
        }

        Column {
            id: surveyColumn
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("Survey about Piepmatz")
            }

            Image {
                source: "../../images/piepmatz.svg"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                fillMode: Image.PreserveAspectFit
                width: 1/2 * parent.width
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Before you can get started with Piepmatz, it would be great if you could answer the following survey. Like the registration, your data is processed anonymously.")
                font.pixelSize: Theme.fontSizeSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            SectionHeader {
                text: qsTr("Would you pay for Piepmatz?")
            }

            Text {
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Piepmatz will remain open source software. However, I'm considering to ask you for a contribution before you can run Piepmatz in a future version. So, would you pay for Piepmatz and if yes, how much?")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            ComboBox {
                id: contributionQuestionCombobox
                label: qsTr("Options: ")
                menu: ContextMenu {
                    MenuItem {
                        text: qsTr("Please select an option...")
                    }
                    MenuItem {
                        text: qsTr("Yes, more than 10 Euro")
                    }
                    MenuItem {
                        text: qsTr("Yes, between 8 and 10 Euro")
                    }
                    MenuItem {
                        text: qsTr("Yes, between 6 and 8 Euro")
                    }
                    MenuItem {
                        text: qsTr("Yes, between 4 and 6 Euro")
                    }
                    MenuItem {
                        text: qsTr("Yes, between 2 and 4 Euro")
                    }
                    MenuItem {
                        text: qsTr("Yes, maximum 2 Euro")
                    }
                    MenuItem {
                        text: qsTr("No, I wouldn't pay for Piepmatz.")
                    }
                    MenuItem {
                        text: qsTr("No, but for my other device.")
                    }
                }
            }

            Text {
                visible: ( contributionQuestionCombobox.currentIndex === 8 ) ? true : false
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("Simply open Piepmatz on the other device and access the 'About Piepmatz' page. The Wagnis ID is listed there.")
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Column {
                width: parent.width
                visible: ( contributionQuestionCombobox.currentIndex === 8 ) ? true : false

                TextField {
                    id: wagnisIdTextField
                    width: parent.width
                    labelVisible: false
                    placeholderText: "1234-5678-90ab-cdef"
                }

                Text {
                    text: qsTr("Enter the Wagnis ID of Piepmatz on the other device")
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeTiny
                    anchors.left: parent.left
                    anchors.leftMargin: Theme.horizontalPageMargin
                }
            }

            Button {
                enabled: {
                    if ( contributionQuestionCombobox.currentIndex === 0 ) {
                        false;
                    } else {
                        if ( contributionQuestionCombobox.currentIndex !== 8) {
                            true;
                        } else {
                            if (wagnisIdTextField.text.match(/^[\da-f]{4}\-[\da-f]{4}\-[\da-f]{4}\-[\da-f]{4}$/)) {
                                true;
                            } else {
                                false;
                            }
                        }
                    }
                }
                text: qsTr("Send")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    wagnis.sendSurvey(contributionQuestionCombobox.currentIndex, wagnisIdTextField.text);
                    registrationPage.registrationLoading = true;
                    surveyFlickable.opacity = 0;
                    surveyFlickable.visible = false;
                }
            }

            Text {
                visible: surveyFlickable.canSkip
                wrapMode: Text.Wrap
                x: Theme.horizontalPageMargin
                width: parent.width - ( 2 * Theme.horizontalPageMargin )
                horizontalAlignment: Text.AlignJustify
                text: qsTr("You can skip the survey for %1 if you want to test Piepmatz before answering the question.").arg(surveyFlickable.remainingTime)
                font.pixelSize: Theme.fontSizeExtraSmall
                linkColor: Theme.highlightColor
                color: Theme.primaryColor
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                visible: surveyFlickable.canSkip
                text: qsTr("Skip Survey")
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
                onClicked: {
                    pageStack.clear();
                    accountModel.isLinked() ? pageStack.push(overviewPage) : pageStack.push(welcomePage)
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width  - ( 2 * Theme.horizontalPageMargin )
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.Wrap
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

}
