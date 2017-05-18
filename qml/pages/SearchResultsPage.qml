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
