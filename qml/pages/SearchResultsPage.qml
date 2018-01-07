/*
    Copyright (C) 2017-18 Sebastian J. Wolf

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
import "../components"

Page {
    id: searchResultsPage
    allowedOrientations: Orientation.All

    property variant searchResults;
    property string searchQuery;
    property bool loaded : false;

    Component.onCompleted: {
        if (!searchResults) {
            console.log("Searching for " + searchQuery);
            twitterApi.searchTweets(searchQuery);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: searchResultsNotification
    }

    Connections {
        target: twitterApi
        onSearchTweetsSuccessful: {
            if (!searchResults) {
                searchResults = result;
                loaded = true;
            }
        }
        onSearchTweetsError: {
            if (!searchResults) {
                loaded = true;
                searchResultsNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: searchResultsContainer
        anchors.fill: parent

        LoadingIndicator {
            id: searchResultsLoadingIndicator
            visible: !searchResultsPage.loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: searchResultsPage.loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {

            width: parent.width
            height: parent.height

            PageHeader {
                id: searchResultsHeader
                title: searchResultsPage.searchQuery
            }

            Loader {
                id: searchResultsLoader
                active: searchResultsPage.loaded
                width: parent.width
                height: parent.height - searchResultsHeader.height
                sourceComponent: searchResultsComponent
            }

            Component {
                id: searchResultsComponent
                Item {
                    id: searchResultsContent

                    SilicaListView {
                        anchors.fill: parent
                        id: searchResultsListView

                        clip: true

                        model: searchResults
                        delegate: Tweet {
                            tweetModel: modelData
                        }
                        VerticalScrollDecorator {}
                    }
                }
            }
        }

    }
}
