/*
  Copyright (C) 2017 Sebastian J. Wolf
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"
import "components"

ApplicationWindow
{

    id: appWindow

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

    initialPage: accountModel.isLinked() ? overviewPage : welcomePage
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}

