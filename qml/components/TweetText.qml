/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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
    visible: (tweetContentText.text !== "")

    width: parent.width

    function makeTheTextGreatAgain() {
        var relevantTweet = Functions.getRelevantTweet(tweetTextColumn.tweet);
        var greatifiedText = relevantTweet.full_text;
        greatifiedText = Emoji.emojify(Functions.enhanceText(greatifiedText, relevantTweet.entities, relevantTweet.extended_entities), Theme.fontSizeSmall);
        return greatifiedText;
    }

    Text {
        width: parent.width
        id: tweetContentText
        text: tweetTextColumn.truncateText ? Emoji.emojify(Functions.getRelevantTweet(tweetTextColumn.tweet).full_text, Theme.fontSizeSmall) : makeTheTextGreatAgain()
        font.pixelSize: Theme.fontSizeSmall
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
}


