import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0

Item {
    id: tweetVideoComponent

    property variant tweet;

    width: parent.width
    height: parent.height

    function getTweetVideoUrl(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                    for (var j = 0; j < tweet.extended_entities.media[i].video_info.variants.length; j++) {
                        if (tweet.extended_entities.media[i].video_info.variants[j].content_type === "video/mp4") {
                            return tweet.extended_entities.media[i].video_info.variants[j].url;
                        }
                    }
                }
            }
        }
        return "";
    }

    function getTweetVideoPlaceholderImageUrl(tweet) {
        if (tweet.extended_entities) {
            for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
                if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                    return tweet.extended_entities.media[i].media_url_https;
                }
            }
        }
        return "";
    }

    Image {
        id: placeholderImage
        source: getTweetVideoPlaceholderImageUrl(tweet.retweeted_status ? tweet.retweeted_status : tweet)
        width: parent.width
        height: parent.height
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        fillMode: Image.PreserveAspectCrop
        visible: status === Image.Ready ? true : false
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (tweetVideo.error === MediaPlayer.NoError) {
                    tweetVideo.visible = true
                    placeholderImage.visible = false
                    playButton.visible = false
                    tweetVideo.play()
                } else {
                    errorText.text = qsTr("Error loading video! " + tweetVideo.errorString)
                    errorTextOverlay.visible = true
                    errorText.visible = true
                }
            }
        }
    }

    Image {
        id: playButton
        anchors.centerIn: placeholderImage
        width: Theme.iconSizeLarge
        height: Theme.iconSizeLarge
        source: "image://theme/icon-l-play"
        visible: placeholderImage.status === Image.Ready ? true : false
    }

    ImageProgressIndicator {
        image: placeholderImage
        withPercentage: true
    }

    Rectangle {
        id: errorTextOverlay
        color: "black"
        opacity: 0.8
        width: parent.width
        height: parent.height
        visible: false
    }

    Text {
        id: errorText
        visible: false
        width: parent.width
        color: Theme.primaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
        horizontalAlignment: Text.AlignHCenter
        anchors {
            verticalCenter: parent.verticalCenter
        }
        wrapMode: Text.Wrap
        text: ""
    }

    Video {
        id: tweetVideo
        visible: false
        width: parent.width
        height: parent.height
        source: getTweetVideoUrl(tweet.retweeted_status ? tweet.retweeted_status : tweet)
        MouseArea {
            anchors.fill: parent
            onClicked: tweetVideo.playbackState === MediaPlayer.PlayingState ? tweetVideo.pause() : tweetVideo.play()
        }
        onStopped: {
            tweetVideo.visible = false
            placeholderImage.visible = true
            playButton.visible = true
        }
    }
}
