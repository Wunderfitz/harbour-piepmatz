/*
  Copyright (C) 2017 Sebastian J. Wolf
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{

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

    initialPage: accountModel.isLinked() ? overviewPage : welcomePage
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}

