import QtQuick 2.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: profilePage
    allowedOrientations: Orientation.All

    property variant profileModel;
    property string profileName;

    SilicaFlickable {
        id: profileContainer
        anchors.fill: parent

        Profile {
            id: otherProfile
            accountModel: profilePage.profileModel
            accountName: profilePage.profileName
        }

    }
}
