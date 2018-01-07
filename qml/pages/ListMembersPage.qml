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
    id: listMembersPage
    allowedOrientations: Orientation.All

    property variant listMembersModel;
    property string listId;
    property string listName;
    property bool loaded : false;

    Component.onCompleted: {
        if (!listMembersModel) {
            console.log("Loading members of list " + listName);
            twitterApi.listMembers(listId);
        } else {
            loaded = true;
        }
    }

    AppNotification {
        id: listMembersNotification
    }

    Connections {
        target: twitterApi
        onListMembersSuccessful: {
            if (!listMembersModel) {
                listMembersModel = result.users;
                loaded = true;
            }
        }
        onListMembersError: {
            if (!listMembersModel) {
                loaded = true;
                listMembersNotification.show(errorMessage);
            }
        }
    }

    SilicaFlickable {
        id: listMembersContainer
        width: parent.width
        height: parent.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Refresh")
                onClicked: {
                    listMembersPage.loaded = false;
                    listMembersPage.listMembersModel = null;
                    twitterApi.listMembers(listId);
                }
            }
        }

        LoadingIndicator {
            id: listMembersLoadingIndictor
            visible: !loaded
            Behavior on opacity { NumberAnimation {} }
            opacity: loaded ? 0 : 1
            height: parent.height
            width: parent.width
        }

        Column {
            anchors.fill: parent

            PageHeader {
                id: listMembersHeader
                title: qsTr("%1: Members").arg(listName)
            }

            SilicaListView {
                id: listMembersListView

                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height - listMembersHeader.height

                clip: true

                model: listMembersModel
                delegate: User {
                    userModel: modelData
                }
                VerticalScrollDecorator {}
            }

        }

    }
}

