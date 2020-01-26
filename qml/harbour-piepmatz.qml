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
import "pages"
import "components"

ApplicationWindow
{

    id: appWindow

    property bool isWifi: accountModel.isWiFi();
    property string linkPreviewMode: accountModel.getLinkPreviewMode();

    property string currentStatusInProgress;

    Connections {
        target: dBusAdaptor
        onPleaseOpenUrl: {
            var twitterRegex = /https\:\/\/\w*\.*twitter\.com\//;
            if (url.match(twitterRegex)) {
                console.log("Probably we have a Twitter link...");
                var userRegex = /https\:\/\/\w*\.*twitter\.com\/(\w+)/;
                var userResult = url.match(userRegex);
                var statusRegex = /https\:\/\/\w*\.*twitter\.com\/\w+\/status\/(\w+)/;
                var statusResult = url.match(statusRegex);
                if (statusResult) {
                    console.log("Opening tweet " + statusResult[1]);
                    currentStatusInProgress = statusResult[1];
                    twitterApi.showStatus(currentStatusInProgress);
                } else if (userResult) {
                    console.log("Opening profile for user " + userResult[1]);
                    pageStack.push(Qt.resolvedUrl("pages/ProfilePage.qml"), {"profileName": userResult[1] });
                }
            } else {
                console.log("EVIL LINK!");
            }
            appWindow.activate();
        }
    }

    Connections {
        target: twitterApi
        onShowStatusSuccessful: {
            console.log("Received tweet " + result.id_str);
            if (result.id_str === currentStatusInProgress) {
                pageStack.push(Qt.resolvedUrl("pages/TweetPage.qml"), {"tweetModel": result });
                currentStatusInProgress = "";
            }
        }
        onShowStatusError: {
            currentStatusInProgress = "";
        }
    }

    Component {
        id: aboutPage
        AboutPage {}
    }

    Component {
        id: welcomePage
        WelcomePage {}
    }

    Component {
        id: overviewPage
        OverviewPage {}
    }

    Component {
        id: newTweetPage
        NewTweetPage {}
    }

    Component {
        id: attachImagesPage
        AttachImagesPage {}
    }

    Component {
        id: registrationPage
        RegistrationPage {}
    }

    Component {
        id: settingsPage
        SettingsPage {}
    }

    //initialPage: ( wagnis.isRegistered() && wagnis.hasFeature("contribution") ) ? (accountModel.isLinked() ? overviewPage : welcomePage) : registrationPage
    initialPage: accountModel.isLinked() ? overviewPage : welcomePage
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations

}

