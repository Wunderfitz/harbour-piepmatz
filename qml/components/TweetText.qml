import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Text {

    property variant tweet;

    width: parent.width
    id: tweetContentText
    text: Functions.enhanceText(Functions.getRelevantTweet(tweet).full_text, Functions.getRelevantTweet(tweet).entities, Functions.getRelevantTweet(tweet).extended_entities)
    font.pixelSize: Theme.fontSizeExtraSmall
    color: Theme.primaryColor
    wrapMode: Text.Wrap
    textFormat: Text.StyledText
    onLinkActivated: Functions.handleLink(link)
    linkColor: Theme.highlightColor
}
