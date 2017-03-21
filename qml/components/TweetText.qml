import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions


Column {
    id: tweetTextColumn
    property variant tweet;

    width: parent.width

    Text {
        width: parent.width
        id: tweetContentText
        text: Functions.enhanceText(Functions.getRelevantTweet(tweetTextColumn.tweet).full_text, Functions.getRelevantTweet(tweetTextColumn.tweet).entities, Functions.getRelevantTweet(tweetTextColumn.tweet).extended_entities)
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.primaryColor
        wrapMode: Text.Wrap
        textFormat: Text.StyledText
        onLinkActivated: {
            Functions.handleLink(link, tweetContentText);
        }
        linkColor: Theme.highlightColor
    }
}


