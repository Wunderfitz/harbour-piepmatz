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
    id: listTimelinePage
    allowedOrientations: Orientation.All

    focus: true
    Keys.onLeftPressed: {
        pageStack.pop();
    }
    Keys.onDownPressed: {
        listTimelineListView.flick(0, - parent.height);
    }
    Keys.onUpPressed: {
        listTimelineListView.flick(0, parent.height);
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_T) {
            listTimelineListView.scrollToTop();
            event.accepted = true;
        }
        if (event.key === Qt.Key_B) {
            listTimelineListView.scrollToBottom();
            event.accepted = true;
        }
    }

    property variant listTimelineModel;
    property string listId;
    property string listName;
    property bool loaded : false;

    function getLastTweetId() {
        return listTimelineModel[listTimelineModel.length - 1].id_str;
    }

    Component.onCompleted: {
        if (!listTimelineModel) {
            console.log("Loading list timeline for " + listName);
            twitterApi.listTimeline(listId);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: listTimelineNotification
    }

    Connections {
        target: twitterApi
        onListTimelineSuccessful: {
            if (!listTimelineModel) {
                listTimelineModel = result;
                loaded = true;
            }
        }
        onListTimelineError: {
            if (!listTimelineModel) {
                loaded = true;
                listTimelineNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: listTimelineContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Show Members")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("../pages/ListMembersPage.qml"), {"listId": listId, "listName": listName});
                }
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    listTimelinePage.loaded = false;
                    listTimelinePage.listTimelineModel = null;
                    twitterApi.listTimeline(listId);
                }
            }
        }

        LoadingIndicator {
            id: listTimelineLoadingIndicator
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent

            PageHeader {
                id: listTimelineHeader
                title: qsTr("%1: Timeline").arg(listName)
            }

            SilicaListView {
                id: listTimelineListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - listTimelineHeader.height

                clip: true

                model: listTimelineModel
                delegate: Tweet {
                    tweetModel: modelData
                }

                footer: listTimelineFooterComponent
                VerticalScrollDecorator {}
            }

            Component {
                id: listTimelineFooterComponent
                Item {
                    id: listTimelineLoadMoreRow
                    visible: listTimelineListView.count > 0
                    width: parent.width
                    height: listTimelineLoadMoreButton.height + ( 2 * Theme.paddingLarge )
                    Button {
                        id: listTimelineLoadMoreButton
                        Behavior on opacity { NumberAnimation {} }
                        text: qsTr("Load more tweets")
                        preferredWidth: Theme.buttonWidthLarge
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        opacity: visible ? 1 : 0
                        onClicked: {
                            console.log("Loading more tweets for list");
                            twitterApi.listTimeline(listId, getLastTweetId());
                            listTimelineLoadMoreBusyIndicator.visible = true;
                            listTimelineLoadMoreButton.visible = false;
                        }
                    }
                    BusyIndicator {
                        id: listTimelineLoadMoreBusyIndicator
                        Behavior on opacity { NumberAnimation {} }
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        visible: false
                        opacity: visible ? 1 : 0
                        running: visible
                        size: BusyIndicatorSize.Medium
                    }
                    Connections {
                        target: twitterApi
                        onListTimelineSuccessful: {
                            if (incrementalUpdate) {
                                listTimelineLoadMoreBusyIndicator.visible = false;
                                listTimelineLoadMoreButton.visible = true;
                                var oldIndex = listTimelineListView.indexAt(listTimelineListView.contentX, ( listTimelineListView.contentY + Math.round(listTimelinePage.height / 2)));
                                listTimelineModel = listTimelineModel.concat(result);
                                listTimelineListView.currentIndex = oldIndex;
                            }
                        }
                        onListTimelineError: {
                            listTimelineLoadMoreBusyIndicator.visible = false;
                            listTimelineLoadMoreButton.visible = false;
                        }
                    }
                }
            }

        }

    }
}

