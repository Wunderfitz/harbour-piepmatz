/*
    Copyright (C) 2017-20 Sebastian J. Wolf

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
import "../pages"
import "../js/functions.js" as Functions
import "../js/twemoji.js" as Emoji
import "../js/twitter-text.js" as TwitterText

Column {
    id: tweetTextColumn
    property variant tweet;
    property bool truncateText : false;
    property bool noteTweetExpanded: false;
    property string componentFontSize: ( accountModel.getFontSize() === "piepmatz" ? Theme.fontSizeExtraSmall : Theme.fontSizeSmall) ;
    visible: (tweetContentText.text !== "")

    width: parent.width

    Connections {
        target: accountModel
        onFontSizeChanged: {
            if (fontSize === "piepmatz") {
                componentFontSize = Theme.fontSizeExtraSmall;
            } else {
                componentFontSize = Theme.fontSizeSmall;
            }
        }
    }

    function makeTheTextGreatAgain() {
        var relevantTweet = Functions.getRelevantTweet(tweetTextColumn.tweet);
        var tweetText = (tweetTextColumn.noteTweetExpanded && relevantTweet.note_tweet) ? relevantTweet.note_tweet : relevantTweet.full_text;
        var entities = (tweetTextColumn.noteTweetExpanded && relevantTweet.note_tweet_entities) ? relevantTweet.note_tweet_entities : relevantTweet.entities;
        return Emoji.emojify(Functions.enhanceText(tweetText, entities, relevantTweet.extended_entities), componentFontSize);
    }

    Text {
        width: parent.width
        id: tweetContentText
        text: tweetTextColumn.truncateText ? Emoji.emojify(Functions.getRelevantTweet(tweetTextColumn.tweet).full_text, componentFontSize) : makeTheTextGreatAgain()
        font.pixelSize: componentFontSize
        font.italic: tweetTextColumn.tweet.fakeTweet ? true : false
        color: Theme.primaryColor
        elide: tweetTextColumn.truncateText ? Text.ElideRight : Text.ElideNone
        wrapMode: Text.Wrap
        textFormat: Text.StyledText
        maximumLineCount: tweetTextColumn.truncateText ? 3 : 42
        onLinkActivated: {
            Functions.handleLink(link);
        }
        linkColor: Theme.highlightColor
    }

    Text {
        id: noteTweetShowMore
        property var relevantTweet: Functions.getRelevantTweet(tweetTextColumn.tweet)
        visible: !tweetTextColumn.truncateText && !tweetTextColumn.noteTweetExpanded
                 && !!relevantTweet.note_tweet
                 && relevantTweet.note_tweet.length > relevantTweet.full_text.length
        width: parent.width
        text: qsTr("<a href=\"note_tweet\">Show more...</a>")
        font.pixelSize: componentFontSize
        color: Theme.primaryColor
        textFormat: Text.StyledText
        linkColor: Theme.highlightColor
        onLinkActivated: {
            tweetTextColumn.noteTweetExpanded = true;
        }
    }
}


