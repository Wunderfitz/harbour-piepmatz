/*
  Copyright (C) 2017 Sebastian J. Wolf
*/


import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtMultimedia 5.0
import Sailfish.Silica 1.0
import "../components"

Page {
    id: overviewPage
    allowedOrientations: Orientation.All

    function hideAccountVerificationColumn() {
        accountVerificationColumn.opacity = 0;
        accountVerificationIndicator.running = false;
        accountVerificationColumn.visible = false;
    }

    function showAccountVerificationColumn() {
        accountVerificationColumn.opacity = 1;
        accountVerificationIndicator.running = true;
        accountVerificationColumn.visible = true;
    }

    function hideBackgroundColumn() {
        backgroundColumn.opacity = 0;
        backgroundColumn.visible = false;
    }

    function showBackgroundColumn() {
        backgroundColumn.opacity = 1;
        backgroundColumn.visible = true;
    }

    function getNavigationRowSize() {
        return Theme.iconSizeMedium + Theme.fontSizeMedium + Theme.paddingLarge;
    }

    function findHiResImage(url) {
        var suffixIndex = url.indexOf("_normal");
        if (suffixIndex !== -1) {
            return url.substring(0, suffixIndex) + url.substring(suffixIndex + 7);
        } else {
            return url;
        }
    }

    function getValidDate(twitterDate) {
        return new Date(twitterDate.replace(/^(\w+) (\w+) (\d+) ([\d:]+) \+0000 (\d+)$/,"$1, $2 $3 $5 $4 GMT"));
    }

    function enhanceText(tweet) {
        var tweetText = tweet.full_text;
        // URLs
        for (var i = 0; i < tweet.entities.urls.length; i++ ) {
            tweetText = tweetText.replace(tweet.entities.urls[i].url, "<a href=\"" + tweet.entities.urls[i].expanded_url + "\">" + tweet.entities.urls[i].display_url + "</a>");
        }
        // Remove media links - will become own QML entities
        if (tweet.extended_entities) {
            for (var j = 0; j < tweet.extended_entities.media.length; j++ ) {
                tweetText = tweetText.replace(tweet.extended_entities.media[j].url, "");
            }
        }
        return tweetText;
    }

    function hasImage(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "photo" ) {
                    return true;
                }
            }
        }
        return false;
    }

    function getTweetImageUrl(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "photo" ) {
                    return tweet.extended_entities.media[i].media_url_https;
                }
            }
        }
        return "";
    }

    function getTweetImageModel(tweet, listModel) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "photo" ) {
                    listModel.append(tweet.extended_entities.media[i]);
                }
            }
        }
        return listModel;
    }

    function getUserNameById(userId, currentUser, userMentions) {
        if (typeof userId !== "undefined") {
            if (userId === currentUser.id) {
                return currentUser.name;
            }
            for (var i = 0; i < userMentions.length ; i++) {
                if (userMentions[i].id === userId) {
                    return userMentions[i].name;
                }
            }
        }
        return "";
    }

    function containsVideo(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                    return true;
                }
            }
        }
        return false;
    }

    function getVideoHeight(videoWidth, tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                    var videoHeight = Math.round(videoWidth * tweet.extended_entities.media[i].video_info.aspect_ratio[1] / tweet.extended_entities.media[i].video_info.aspect_ratio[0]);
                    return videoHeight;
                }
            }
        }
        return 1;
    }

    function updatePiepmatz() {
        homeView.reloading = true;
        timelineModel.update();
    }

    property string activeTabId: "home"

    function openTab(tabId) {

        activeTabId = tabId;

        switch (tabId) {
        case "home":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.highlightColor
            homeButtonText.color = Theme.highlightColor
            homeView.visible = true
            homeView.opacity = 1
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "notifications":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.highlightColor
            notificationsButtonText.color = Theme.highlightColor
            notificationsColumn.visible = true
            notificationsColumn.opacity = 1
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "messages":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.highlightColor
            messagesButtonText.color = Theme.highlightColor
            messagesColumn.visible = true
            messagesColumn.opacity = 1
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "search":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.highlightColor
            searchButtonText.color = Theme.highlightColor
            searchColumn.visible = true
            searchColumn.opacity = 1
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.primaryColor
            profileButtonText.color = Theme.primaryColor
            profileView.opacity = 0
            profileView.visible = false
            break;
        case "profile":
            homeButtonImage.icon.source = "image://theme/icon-m-home?" + Theme.primaryColor
            homeButtonText.color = Theme.primaryColor
            homeView.opacity = 0
            homeView.visible = false
            notificationsButtonImage.icon.source = "image://theme/icon-m-alarm?" + Theme.primaryColor
            notificationsButtonText.color = Theme.primaryColor
            notificationsColumn.opacity = 0
            notificationsColumn.visible = false
            messagesButtonImage.icon.source = "image://theme/icon-m-mail?" + Theme.primaryColor
            messagesButtonText.color = Theme.primaryColor
            messagesColumn.opacity = 0
            messagesColumn.visible = false
            searchButtonImage.icon.source = "image://theme/icon-m-search?" + Theme.primaryColor
            searchButtonText.color = Theme.primaryColor
            searchColumn.opacity = 0
            searchColumn.visible = false
            profileButtonImage.icon.source = "image://theme/icon-m-person?" + Theme.highlightColor
            profileButtonText.color = Theme.highlightColor
            profileView.visible = true
            profileView.opacity = 1
            break;
        default:
            console.log("Some strange navigation happened!")
        }
    }

    Component.onCompleted: {
        accountModel.verifyCredentials()
    }

    Connections {
        target: accountModel
        onCredentialsVerified: {
            hideAccountVerificationColumn()
            overviewContainer.visible = true
            overviewColumn.visible = true
            overviewColumn.opacity = 1
            openTab("home")
            timelineModel.update()
        }
        onVerificationError: {
            hideAccountVerificationColumn()
            verificationFailedColumn.visible = true;
            verificationFailedColumn.opacity = 1
        }
    }

    Column {
        y: ( parent.height - ( accountVerificationImage.height + accountVerificationIndicator.height + accountVerificationLabel.height + ( 3 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: accountVerificationColumn
        spacing: Theme.paddingSmall
        Behavior on opacity { NumberAnimation {} }

        Image {
            id: accountVerificationImage
            source: "../../images/piepmatz.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: accountVerificationLabel
            text: qsTr("Saying hello to Twitter...")
        }

        BusyIndicator {
            id: accountVerificationIndicator
            anchors.horizontalCenter: parent.horizontalCenter
            running: true
            size: BusyIndicatorSize.Large
        }

    }

    Column {
        y: ( parent.height - ( verificationFailedImage.height + verificationFailedInfoLabel.height + verifyAgainButton.height + reauthenticateButton.height + ( 4 * Theme.paddingSmall ) ) ) / 2
        width: parent.width
        id: verificationFailedColumn
        spacing: Theme.paddingSmall
        Behavior on opacity { NumberAnimation {} }
        opacity: 0
        visible: false

        Image {
            id: verificationFailedImage
            source: "../../images/piepmatz.svg"
            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            fillMode: Image.PreserveAspectFit
            width: 1/2 * parent.width
        }

        InfoLabel {
            id: verificationFailedInfoLabel
            font.pixelSize: Theme.fontSizeLarge
            text: qsTr("Piepmatz could not log you in!")
        }

        Button {
            id: verifyAgainButton
            text: qsTr("Try again")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                verificationFailedColumn.opacity = 0;
                verificationFailedColumn.visible = false;
                showAccountVerificationColumn()
                accountModel.verifyCredentials()
            }
        }

        Button {
            id: reauthenticateButton
            text: qsTr("Authenticate")
            anchors {
                horizontalCenter: parent.horizontalCenter
            }
            onClicked: {
                verificationFailedColumn.opacity = 0;
                verificationFailedColumn.visible = false;
                pageStack.clear()
                pageStack.push(welcomePage)
            }
        }

    }

    SilicaFlickable {
        id: overviewContainer
        anchors.fill: parent
        visible: false

        PullDownMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
            MenuItem {
                text: qsTr("New Tweet")
                onClicked: pageStack.push(newTweetPage)
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: updatePiepmatz()
            }
        }

        PushUpMenu {
            MenuItem {
                text: qsTr("About Piepmatz")
                onClicked: pageStack.push(aboutPage)
            }
            MenuItem {
                text: qsTr("New Tweet")
                onClicked: pageStack.push(newTweetPage)
            }
            MenuItem {
                text: qsTr("Refresh")
                onClicked: updatePiepmatz()
            }
        }

        Column {
            id: overviewColumn
            opacity: 0
            visible: false
            anchors {
                fill: parent
            }
            Behavior on opacity { NumberAnimation {} }

            SilicaListView {
                id: profileView
                opacity: 0
                Behavior on opacity { NumberAnimation {} }
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                clip: true

                model: accountModel

                delegate: Item {
                    id: profileComponent
                    height: profileItemColumn.height
                    width: parent.width

                    Column {
                        id: profileItemColumn
                        width: parent.width
                        spacing: 2 * Theme.horizontalPageMargin

                        Row {
                            id: profileTitleRow
                            spacing: Theme.horizontalPageMargin
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                            Column {
                                id: profilePictureColumn
                                width: parent.width / 3
                                height: parent.width / 3
                                anchors {
                                    leftMargin: Theme.horizontalPageMargin
                                    topMargin: Theme.horizontalPageMargin
                                    top: parent.top
                                }
                                Item {
                                    id: profilePictureItem
                                    width: parent.width
                                    height: parent.height
                                    Rectangle {
                                        id: profilePictureBackground
                                        width: parent.width
                                        height: parent.height
                                        color: Theme.primaryColor
                                        radius: parent.width / 6
                                        anchors {
                                            margins: Theme.horizontalPageMargin
                                        }
                                    }

                                    Image {
                                        id: profilePicture
                                        source: findHiResImage(display.profile_image_url_https)
                                        width: parent.width - parent.width / 10
                                        height: parent.height - parent.height / 10
                                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                                        anchors.centerIn: profilePictureBackground
                                        visible: false
                                    }

                                    Rectangle {
                                        id: profilePictureMask
                                        width: parent.width - parent.width / 10
                                        height: parent.height - parent.height / 10
                                        color: Theme.primaryColor
                                        radius: parent.width / 7
                                        anchors.margins: Theme.horizontalPageMargin + parent.width / 60
                                        anchors.centerIn: profilePictureBackground
                                        visible: false
                                    }

                                    OpacityMask {
                                        id: maskedProfilePicture
                                        source: profilePicture
                                        maskSource: profilePictureMask
                                        anchors.fill: profilePicture
                                    }

                                }
                            }

                            Column {
                                id: profileOverviewColumn
                                width: parent.width * 2 / 3
                                spacing: Theme.paddingSmall
                                anchors {
                                    leftMargin: Theme.horizontalPageMargin
                                    topMargin: Theme.horizontalPageMargin
                                    top: parent.top
                                }
                                Text {
                                    id: profileNameText
                                    text: display.name
                                    font {
                                        pixelSize: Theme.fontSizeLarge
                                        bold: true
                                    }
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileScreenNameText
                                    text: qsTr("@%1").arg(display.screen_name)
                                    font {
                                        pixelSize: Theme.fontSizeMedium
                                        bold: true
                                    }
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileFriendsText
                                    text: qsTr("%1 Following").arg(display.friends_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                                Text {
                                    id: profileFollowersText
                                    text: qsTr("%1 Followers").arg(display.followers_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                }
                            }
                        }

                        Column {
                            id: profileDetailsColumn
                            spacing: Theme.paddingLarge
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                            Text {
                                id: profileDescriptionText
                                text: display.description
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                wrapMode: Text.Wrap
                                width: parent.width
                            }
                            Row {
                                id: profileMoreInformationRow
                                spacing: Theme.horizontalPageMargin
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Text {
                                    id: profileTweetsText
                                    text: qsTr("%1 Tweets").arg(display.statuses_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                                Text {
                                    id: profileMoreInfoSeparatorText
                                    text: qsTr("|")
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                                Text {
                                    id: profileFavoritesText
                                    text: qsTr("%1 Favorites").arg(display.favourites_count)
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                }
                            }
                            Text {
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                id: profileJoinedText
                                text: qsTr("Joined in %1").arg(getValidDate(display.created_at).toLocaleDateString(Qt.locale(), "MMMM yyyy"))
                                font.pixelSize: Theme.fontSizeSmall
                                color: Theme.primaryColor
                                wrapMode: Text.Wrap
                            }

                            Row {
                                id: profileLocationRow
                                visible: display.location.length === 0 ? false : true
                                spacing: Theme.horizontalPageMargin
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Image {
                                    id: profileLocationImage
                                    source: "image://theme/icon-m-location"
                                }
                                Text {
                                    id: profileLocationText
                                    text: display.location
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            Row {
                                id: profileUrlRow
                                spacing: Theme.horizontalPageMargin
                                visible: display.entities.url.urls.length === 0 ? false : true
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                }
                                Image {
                                    id: profileUrlImage
                                    source: "image://theme/icon-m-link"
                                }
                                Text {
                                    id: profileUrlText
                                    text: "<a href=\"" + display.entities.url.urls[0].url + "\">" + display.entities.url.urls[0].display_url + "</a>"
                                    font.pixelSize: Theme.fontSizeSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                    anchors.verticalCenter: parent.verticalCenter
                                    onLinkActivated: Qt.openUrlExternally(display.entities.url.urls[0].url)
                                    linkColor: Theme.highlightColor
                                }
                            }

                        }
                    }
                }
                VerticalScrollDecorator {}
            }

            Item {
                id: homeView
                width: parent.width
                height: parent.height - getNavigationRowSize()
                visible: false

                property bool loaded : false;
                property bool error : false;
                property bool reloading: false;

                Connections {
                    target: timelineModel
                    onHomeTimelineUpdated: {
                        homeView.loaded = true;
                        homeView.reloading = false;
                    }
                    onHomeTimelineError: {
                        homeView.loaded = true;
                        homeView.error = true;
                        homeView.reloading = false;
                        homeErrorLabel.text = errorMessage;
                    }
                }

                Column {
                    width: parent.width
                    height: homeProgressLabel.height + homeProgressIndicator.height + Theme.paddingSmall
                    visible: !homeView.loaded
                    opacity: homeView.loaded ? 0 : 1
                    id: homeProgressColumn
                    spacing: Theme.paddingSmall
                    Behavior on opacity { NumberAnimation {} }
                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: homeProgressLabel
                        text: qsTr("Loading timeline...")
                    }

                    BusyIndicator {
                        id: homeProgressIndicator
                        anchors.horizontalCenter: parent.horizontalCenter
                        running: !homeView.loaded
                        size: BusyIndicatorSize.Large
                    }
                }

                Column {
                    width: parent.width
                    height: homeProgressLabel.height + homeProgressIndicator.height + Theme.paddingSmall
                    visible: homeView.loaded && homeView.error
                    opacity: homeView.loaded && homeView.error ? 0 : 1
                    id: homeErrorColumn
                    spacing: Theme.paddingSmall
                    Behavior on opacity { NumberAnimation {} }
                    anchors.verticalCenter: parent.verticalCenter

                    InfoLabel {
                        id: homeErrorLabel
                        text: ""
                    }
                }

                SilicaListView {
                    id: homeListView
                    opacity: homeView.loaded && !homeView.error ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
                    visible: homeView.loaded && !homeView.error
                    width: parent.width
                    height: parent.height
                    clip: true

                    model: timelineModel

                    delegate: ListItem {
                        contentHeight: homeTweetRow.height + 2 * Theme.paddingMedium
                        contentWidth: parent.width

                        Row {
                            id: homeTweetRow
                            width: parent.width - ( 2 * Theme.horizontalPageMargin )
                            height: homeTweetAuthorColumn.height > homeTweetContentColumn.height ? homeTweetAuthorColumn.height + Theme.paddingSmall : homeTweetContentColumn.height + Theme.paddingSmall
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                verticalCenter: parent.verticalCenter
                            }
                            spacing: Theme.paddingMedium

                            Column {
                                id: homeTweetAuthorColumn
                                width: parent.width / 6
                                height: parent.width / 6
                                spacing: Theme.paddingSmall
                                Image {
                                    id: homeTweetRetweetedImage
                                    source: "image://theme/icon-s-retweet"
                                    visible: display.retweeted_status ? true : false
                                    anchors.right: parent.right
                                    width: Theme.fontSizeExtraSmall
                                    height: Theme.fontSizeExtraSmall
                                }
                                Image {
                                    id: homeTweetInReplyToImage
                                    source: "image://theme/icon-s-repost"
                                    visible: display.in_reply_to_user_id_str ? true : false
                                    anchors.right: parent.right
                                    width: Theme.fontSizeExtraSmall
                                    height: Theme.fontSizeExtraSmall
                                }

                                Item {
                                    id: homeTweetAuthorItem
                                    width: parent.width
                                    height: parent.width
                                    Image {
                                        id: homeTweetAuthorPicture
                                        source: findHiResImage(display.retweeted_status ? display.retweeted_status.user.profile_image_url_https : display.user.profile_image_url_https )
                                        width: parent.width
                                        height: parent.height
                                        sourceSize {
                                            width: parent.width
                                            height: parent.height
                                        }
                                        visible: false
                                    }

                                    Rectangle {
                                        id: homeTweetAuthorPictureMask
                                        width: parent.width
                                        height: parent.height
                                        color: Theme.primaryColor
                                        radius: parent.width / 7
                                        visible: false
                                    }

                                    OpacityMask {
                                        id: maskedhomeTweetAuthorPicture
                                        source: homeTweetAuthorPicture
                                        maskSource: homeTweetAuthorPictureMask
                                        anchors.fill: homeTweetAuthorPicture
                                        visible: homeTweetAuthorPicture.status === Image.Ready ? true : false
                                        opacity: homeTweetAuthorPicture.status === Image.Ready ? 1 : 0
                                        Behavior on opacity { NumberAnimation {} }
                                    }

                                    ImageProgressIndicator {
                                        image: homeTweetAuthorPicture
                                        withPercentage: false
                                    }

                                }
                            }

                            Column {
                                id: homeTweetContentColumn
                                width: parent.width * 5 / 6 - Theme.horizontalPageMargin
                                spacing: Theme.paddingSmall

                                Text {
                                    id: homeTweetRetweetedText
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.secondaryColor
                                    text: qsTr("Retweeted by %1").arg(display.user.name)
                                    visible: display.retweeted_status ? true : false
                                }

                                Text {
                                    id: homeTweetInReplyToText
                                    font.pixelSize: Theme.fontSizeTiny
                                    color: Theme.secondaryColor
                                    text: qsTr("In reply to %1").arg(getUserNameById(display.in_reply_to_user_id, display.user, display.entities.user_mentions))
                                    visible: display.in_reply_to_user_id_str ? true : false
                                }

                                Row {
                                    id: homeTweetUserRow
                                    width: parent.width
                                    spacing: Theme.paddingSmall

                                    Text {
                                        id: homeTweetUserNameText
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        font.bold: true
                                        color: Theme.primaryColor
                                        text: display.retweeted_status ? display.retweeted_status.user.name : display.user.name
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }
                                    Text {
                                        id: homeTweetUserHandleText
                                        font.pixelSize: Theme.fontSizeExtraSmall
                                        color: Theme.secondaryColor
                                        anchors.bottom: homeTweetUserNameText.bottom
                                        text: qsTr("@%1").arg(display.retweeted_status ? display.retweeted_status.user.screen_name : display.user.screen_name)
                                        elide: Text.ElideRight
                                        maximumLineCount: 1
                                    }
                                }

                                Text {
                                    width: parent.width
                                    id: homeTweetContentText
                                    text: enhanceText(display.retweeted_status ? display.retweeted_status : display)
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                    color: Theme.primaryColor
                                    wrapMode: Text.Wrap
                                    textFormat: Text.StyledText
                                    onLinkActivated: Qt.openUrlExternally(link)
                                    linkColor: Theme.highlightColor
                                }

                                Row {
                                    id: homeTweetInfoRow
                                    width: parent.width
                                    spacing: Theme.paddingSmall

                                    Row {
                                        width: parent.width / 2
                                        Text {
                                            id: homeTweetDateText
                                            font.pixelSize: Theme.fontSizeTiny
                                            color: Theme.secondaryColor
                                            text:  getValidDate(display.retweeted_status ? display.retweeted_status.created_at : display.created_at).toLocaleString(Qt.locale(), Locale.ShortFormat)
                                            elide: Text.ElideRight
                                            maximumLineCount: 1
                                        }
                                    }

                                    Row {
                                        width: parent.width / 2
                                        spacing: Theme.paddingSmall
                                        Column {
                                            width: parent.width / 6
                                            Image {
                                                id: homeTweetRetweetedCountImage
                                                anchors.right: parent.right
                                                source: "image://theme/icon-s-retweet"
                                                width: Theme.fontSizeExtraSmall
                                                height: Theme.fontSizeExtraSmall
                                            }
                                        }
                                        Column {
                                            width: parent.width / 3
                                            Text {
                                                id: homeTweetRetweetedCountText
                                                font.pixelSize: Theme.fontSizeTiny
                                                anchors.left: parent.left
                                                color: Theme.secondaryColor
                                                text: display.retweeted_status ? ( display.retweeted_status.retweet_count ? display.retweeted_status.retweet_count : " " ) : ( display.retweet_count ? display.retweet_count : " " )
                                                elide: Text.ElideRight
                                                maximumLineCount: 1
                                            }
                                        }
                                        Column {
                                            width: parent.width / 6
                                            Image {
                                                id: homeTweetFavoritesCountImage
                                                anchors.right: parent.right
                                                source: "image://theme/icon-s-favorite"
                                                width: Theme.fontSizeExtraSmall
                                                height: Theme.fontSizeExtraSmall
                                            }
                                        }
                                        Column {
                                            width: parent.width / 3
                                            Text {
                                                id: homeTweetFavoritesCountText
                                                font.pixelSize: Theme.fontSizeTiny
                                                anchors.left: parent.left
                                                color: Theme.secondaryColor
                                                text: display.retweeted_status ? ( display.retweeted_status.favorite_count ? display.retweeted_status.favorite_count : " " ) : ( display.favorite_count ? display.favorite_count : " " )
                                                elide: Text.ElideRight
                                                maximumLineCount: 1
                                            }
                                        }
                                    }
                                }

                                ListModel {
                                    id: tweetImageListModel
                                }

                                SlideshowView {
                                    id: homeTweetImageSlideshow
                                    visible: hasImage(display.retweeted_status ? display.retweeted_status : display)
                                    width: parent.width
                                    height: parent.width * 2 / 3
                                    model: getTweetImageModel(display.retweeted_status ? display.retweeted_status : display, tweetImageListModel)
                                    delegate: Item {
                                        width: parent.width
                                        height: parent.height

                                        Component {
                                            id: singleImageComponent
                                            ImagePage {
                                                imageUrl: media_url_https
                                                imageHeight: sizes.large.h
                                                imageWidth: sizes.large.w
                                            }
                                        }

                                        Image {
                                            id: homeTweetImage
                                            source: media_url_https
                                            width: parent.width
                                            height: parent.height
                                            sourceSize.width: parent.width
                                            sourceSize.height: parent.height
                                            fillMode: Image.PreserveAspectCrop
                                            visible: status === Image.Ready ? true : false
                                            opacity: status === Image.Ready ? 1 : 0
                                            Behavior on opacity { NumberAnimation {} }
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    pageStack.push( singleImageComponent );
                                                }
                                            }
                                        }
                                        ImageProgressIndicator {
                                            image: homeTweetImage
                                            withPercentage: true
                                        }
                                    }
                                }

                                Loader {
                                    id: videoLoader
                                    active: containsVideo(display.retweeted_status ? display.retweeted_status : display)
                                    width: parent.width
                                    height: getVideoHeight(parent.width, display.retweeted_status ? display.retweeted_status : display)
                                    sourceComponent: tweetVideoComponent
                                }

                                Component {
                                    id: tweetVideoComponent
                                    TweetVideoComponent {
                                        tweet: display
                                    }
                                }

                            }
                        }

                        Separator {
                            id: homeTweetSeparator
                            width: parent.width
                            color: Theme.primaryColor
                            horizontalAlignment: Qt.AlignHCenter
                        }
                    }

                    VerticalScrollDecorator {}
                }

                LoadingIndicator {
                    id: homeLoadingIndicator
                    visible: homeView.reloading
                    Behavior on opacity { NumberAnimation {} }
                    opacity: homeView.reloading ? 1 : 0
                    height: parent.height
                    width: parent.width
                }
            }


            Column {
                id: notificationsColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Notifications not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Column {
                id: messagesColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Messages not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Column {
                id: searchColumn
                opacity: 0
                visible: false
                width: parent.width
                height: parent.height - getNavigationRowSize()
                Behavior on opacity { NumberAnimation {} }
                Text {
                    text: "Search not yet implemented"
                    font.pixelSize: Theme.fontSizeMedium
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    width: parent.width - 2 * Theme.horizontalPageMargin
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Row {
                id: navigationRow
                width: parent.width
                height: getNavigationRowSize()

                Column {
                    id: homeButtonColumn
                    width: parent.width / 5
                    height: parent.height

                    IconButton {
                        id: homeButtonImage
                        icon.source: "image://theme/icon-m-home"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: {
                            if (overviewPage.activeTabId === "home") {
                                homeListView.scrollToTop();
                            } else {
                                openTab("home");
                            }
                        }
                    }
                    Label {
                        id: homeButtonText
                        text: qsTr("Timeline")
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                Column {
                    id: notificationsButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: notificationsButtonImage
                        icon.source: "image://theme/icon-m-alarm"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: openTab("notifications")
                    }
                    Label {
                        id: notificationsButtonText
                        text: qsTr("Notifications")
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                Column {
                    id: messagesButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: messagesButtonImage
                        icon.source: "image://theme/icon-m-mail"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: openTab("messages")
                    }
                    Label {
                        id: messagesButtonText
                        text: qsTr("Messages")
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                Column {
                    id: searchButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: searchButtonImage
                        icon.source: "image://theme/icon-m-search"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: openTab("search")
                    }
                    Label {
                        id: searchButtonText
                        text: qsTr("Search")
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                Column {
                    id: profileButtonColumn
                    width: parent.width / 5
                    height: parent.height
                    IconButton {
                        id: profileButtonImage
                        icon.source: "image://theme/icon-m-person"
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }
                        onClicked: openTab("profile")
                    }
                    Label {
                        id: profileButtonText
                        text: qsTr("Profile")
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.primaryColor
                        truncationMode: TruncationMode.Fade
                        anchors {
                            horizontalCenter: parent.horizontalCenter
                        }

                    }
                }
            }

        }

    }
}
