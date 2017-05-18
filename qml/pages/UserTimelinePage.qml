import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: userTimelinePage
    allowedOrientations: Orientation.All

    property variant userTimelineModel;
    property string screenName;
    property string userName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!userTimelineModel) {
            console.log("Loading timeline for " + screenName);
            twitterApi.userTimeline(screenName);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: userTimelineNotification
    }

    Connections {
        target: twitterApi
        onUserTimelineSuccessful: {
            if (!userTimelineModel) {
                userTimelineModel = result;
                loaded = true;
            }
        }
        onShowUserError: {
            if (!userTimelineModel) {
                loaded = true;
                userTimelineNotification.show(errorMessage);
            }
        }
    }

    PageHeader {
        id: userTimelineHeader
        title: userName
    }

    SilicaFlickable {
        id: userTimelineContainer
        width: parent.width
        height: parent.height - userTimelineHeader.height
        anchors.top: userTimelineHeader.bottom

        LoadingIndicator {
            id: userTimelineLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        SilicaListView {
            id: userTimelineListView

            anchors.fill: parent

            clip: true

            model: userTimelineModel
            delegate: Tweet {
                tweetModel: modelData
            }
            VerticalScrollDecorator {}
        }
    }
}

