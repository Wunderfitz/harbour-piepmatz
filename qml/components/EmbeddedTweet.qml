import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Item {

    id: embeddedTweet

    property variant tweetModel;

    width: parent.width
    height: embeddedTweetColumn.height

    Column {
        id: embeddedTweetColumn
        spacing: Theme.paddingMedium
        width: parent.width - Theme.paddingMedium
        anchors.right: parent.right
        height: embeddedTweetSeparatorTop.height + tweetUserRow.height + tweetContentText.height + 3 * ( Theme.paddingMedium ) + ( videoLoader.active ? videoLoader.height + Theme.paddingMedium : 0 ) + ( tweetImageSlideshow.visible ? tweetImageSlideshow.height + Theme.paddingMedium : 0 )

        Separator {
            id: embeddedTweetSeparatorTop
            width: parent.width
            color: Theme.primaryColor
            horizontalAlignment: Qt.AlignHCenter
        }

        TweetUser {
            id: tweetUserRow
            tweetUser: tweetModel.retweeted_status ? tweetModel.retweeted_status.user : tweetModel.user
        }

        Text {
            width: parent.width
            id: tweetContentText
            text: Functions.enhanceText(Functions.getRelevantTweet(tweetModel).full_text, Functions.getRelevantTweet(tweetModel).entities, Functions.getRelevantTweet(tweetModel).extended_entities)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            textFormat: Text.StyledText
            onLinkActivated: Functions.handleLink(link)
            linkColor: Theme.highlightColor
        }

        TweetImageSlideshow {
            id: tweetImageSlideshow
            visible: Functions.hasImage(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            tweet: tweetModel
        }

        Loader {
            id: videoLoader
            active: Functions.containsVideo(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            width: parent.width
            height: Functions.getVideoHeight(parent.width, tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            sourceComponent: tweetVideoComponent
        }

        Component {
            id: tweetVideoComponent
            TweetVideo {
                tweet: tweetModel
            }
        }

    }

}
