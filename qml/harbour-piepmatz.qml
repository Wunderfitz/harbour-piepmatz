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

    initialPage: welcomePage
    cover: Qt.resolvedUrl("pages/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}

