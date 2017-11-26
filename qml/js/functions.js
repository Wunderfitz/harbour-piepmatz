/*
    Copyright (C) 2017 Sebastian J. Wolf

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
function updatePiepmatz() {
    if (typeof homeView !== "undefined") {
        homeView.reloading = true;
        notificationsColumn.updateInProgress = true;
    }
    if (typeof searchColumn !== "undefined") {
        if (searchField.text !== "") {
            console.log("Updating search as well...");
            searchColumn.tweetSearchInTransition = true;
            searchColumn.usersSearchInTransition = true;
            searchTimer.stop();
            searchTimer.start();
        }
    }
    timelineModel.update();
    mentionsModel.update();
    directMessagesModel.update();
    accountModel.verifyCredentials();
}

function findHiResImage(url) {
    var suffixIndex = url.indexOf("_normal");
    if (suffixIndex !== -1) {
        return url.substring(0, suffixIndex) + url.substring(suffixIndex + 7);
    } else {
        return url;
    }
}

function findBiggerImage(url) {
    var suffixIndex = url.indexOf("_normal");
    if (suffixIndex !== -1) {
        return url.substring(0, suffixIndex) + "_bigger" + url.substring(suffixIndex + 7);
    } else {
        return url;
    }
}

function getValidDate(twitterDate) {
    return new Date(twitterDate.replace(/^(\w+) (\w+) (\d+) ([\d:]+) \+0000 (\d+)$/,"$1, $2 $3 $5 $4 GMT"));
}

function getTweetId(url) {
    // The QML JS engine sometimes fails to match regex /twitter\.com\/\w+\/status\/(\d+)/g correctly. Therefore doing it differently now...
    if (url.indexOf("twitter.com/") !== -1 && url.indexOf("/status/") !== -1) {
        return url.substring(url.indexOf("/status/") + 8);
    }
    return null;
}

function Replacement(beginOffset, endOffset, originalString, replacementString) {
    this.beginOffset = beginOffset;
    this.endOffset = endOffset;
    this.originalString = originalString;
    this.replacementString = replacementString;
}

Replacement.prototype.toString = function replacementToString() {
    return "Begin Offset: " + this.beginOffset + ", End Offset: " + this.endOffset + ", Original String: " + this.originalString + ", Replacement String: " + this.replacementString;
}

function getRelevantTweet(tweet) {
    return tweet.retweeted_status ? tweet.retweeted_status : tweet;
}

function enhanceDescription(description) {
    var httpRegex = /\s+(http[s]*\:\/\/\S+)/g;
    description = description.replace(httpRegex, " <a href=\"$1\">$1</a>");

    var userRegex = /\s+(\@(\w+))/g;
    description = description.replace(userRegex, " <a href=\"profile://$2\">$1</a>");

    var tagRegex = /\s+(\#\w+)/g;
    description = description.replace(tagRegex, " <a href=\"tag://$1\">$1</a>");

    return description;
}

function enhanceSimpleText(tweetText, entities) {
    return enhanceTweetText(tweetText, entities, null, false);
}

function enhanceText(tweetText, entities, extendedEntities) {
    return enhanceTweetText(tweetText, entities, extendedEntities, true);
}

function enhanceTweetText(tweetText, entities, extendedEntities, withReferenceUrl) {
    var replacements = [];

    // URLs
    for (var i = 0; i < entities.urls.length; i++ ) {
        var tweetId = getTweetId(entities.urls[i].expanded_url);
        if (tweetId !== null) {
            // Remove tweet URLs - will become embedded tweets...
            embeddedTweetId = tweetId;
            twitterApi.showStatus(tweetId);
            replacements.push(new Replacement(entities.urls[i].indices[0], entities.urls[i].indices[1], entities.urls[i].url, ""));
        } else {
            var url_replacement = "<a href=\"" + entities.urls[i].expanded_url + "\">" + entities.urls[i].display_url + "</a>";
            replacements.push(new Replacement(entities.urls[i].indices[0], entities.urls[i].indices[1], entities.urls[i].url, url_replacement));
            // TODO: Could fail in case of multiple references. Well, let's see what happens :D
            if (withReferenceUrl) {
                referenceUrl = entities.urls[i].expanded_url;
                twitterApi.getOpenGraph(entities.urls[i].expanded_url);
            }
        }
    }
    // Remove media links - will become own QML entities
    if (extendedEntities) {
        for (var j = 0; j < extendedEntities.media.length; j++ ) {
            replacements.push(new Replacement(extendedEntities.media[j].indices[0], extendedEntities.media[j].indices[1], extendedEntities.media[j].url, ""));
        }
    }
    // User Mentions
    for (var k = 0; k < entities.user_mentions.length; k++ ) {
        var user_mention = qsTr("@%1").arg(entities.user_mentions[k].screen_name);
        var user_mention_url = "<a href=\"profile://" + entities.user_mentions[k].screen_name + "\">" + user_mention + "</a>";
        replacements.push(new Replacement(entities.user_mentions[k].indices[0], entities.user_mentions[k].indices[1], user_mention, user_mention_url));
    }
    // Hashtags
    for (var l = 0; l < entities.hashtags.length; l++ ) {
        var hashtag = qsTr("#%1").arg(entities.hashtags[l].text);
        var hashtag_url = "<a href=\"tag://" + hashtag + "\">" + hashtag + "</a>";
        replacements.push(new Replacement(entities.hashtags[l].indices[0], entities.hashtags[l].indices[1], hashtag, hashtag_url));
    }

    replacements.sort( function(a,b) { return b.beginOffset - a.beginOffset } );
    for (var m = 0; m < replacements.length; m++) {
        var foundString = tweetText.substring(replacements[m].beginOffset, replacements[m].endOffset);
        if (foundString.toUpperCase() === replacements[m].originalString.toUpperCase()) {
            tweetText = tweetText.substring(0, replacements[m].beginOffset) + replacements[m].replacementString + tweetText.substring(replacements[m].endOffset);
        } else {
            // Sometimes our offsets do not match the offsets by Twitter - trying a failsafe instead
            console.log("Failsafe replacement used for " + replacements[m].originalString + ", replacement: " + replacements[m].replacementString, " - Original would have been " + foundString);
            tweetText = tweetText.replace(replacements[m].originalString, replacements[m].replacementString);
        }
    }

    return tweetText;
}

function handleLink(link) {
    if (link.indexOf("profile://") === 0) {
        console.log("Profile clicked: " + link);
        pageStack.push(Qt.resolvedUrl("../pages/ProfilePage.qml"), {"profileName": link.substring(10)});
    } else if (link.indexOf("tag://") === 0) {
        console.log("Hashtag clicked: " + link);
        pageStack.push(Qt.resolvedUrl("../pages/SearchResultsPage.qml"), {"searchQuery": link.substring(6)});
    }  else {
        Qt.openUrlExternally(link);
    }
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

function getScreenNameById(userId, currentUser, userMentions) {
    if (typeof userId !== "undefined") {
        if (userId === currentUser.id) {
            return currentUser.screen_name;
        }
        for (var i = 0; i < userMentions.length ; i++) {
            if (userMentions[i].id === userId) {
                return userMentions[i].screen_name;
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

function getVideoOriginalHeight(tweet) {
    if (tweet.extended_entities) {
        for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
            if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                return tweet.extended_entities.media[i].video_info.aspect_ratio[1];
            }
        }
    }
    return 1;
}

function getVideoOriginalWidth(tweet) {
    if (tweet.extended_entities) {
        for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
            if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                return tweet.extended_entities.media[i].video_info.aspect_ratio[0];
            }
        }
    }
    return 1;
}

function getTweetUrl(tweetModel) {
    var statusUrl = "https://twitter.com/";
    if (tweetModel.retweeted_status) {
        statusUrl += tweetModel.retweeted_status.user.screen_name + "/status/" + tweetModel.retweeted_status.id_str;
    } else {
        statusUrl += tweetModel.user.screen_name + "/status/" + tweetModel.id_str;
    }
    console.log(statusUrl);
    return statusUrl;
}

function getUserUrl(userModel) {
    var userUrl = "https://twitter.com/" + userModel.screen_name;
    console.log(userUrl);
    return userUrl;
}

function getRetweetCount(tweetModel) {
    return tweetModel.retweeted_status ? ( tweetModel.retweeted_status.retweet_count ? tweetModel.retweeted_status.retweet_count : " " ) : ( tweetModel.retweet_count ? tweetModel.retweet_count : " " )
}

function getFavoritesCount(tweetModel) {
    return tweetModel.retweeted_status ? ( tweetModel.retweeted_status.favorite_count ? tweetModel.retweeted_status.favorite_count : " " ) : ( tweetModel.favorite_count ? tweetModel.favorite_count : " " );
}

function getShortenedCount(count) {
    if (count >= 1000000) {
        return qsTr("%1M").arg((count / 1000000).toLocaleString(Qt.locale(), 'f', 0));
    } else if (count >= 1000 ) {
        return qsTr("%1K").arg((count / 1000).toLocaleString(Qt.locale(), 'f', 0));
    } else {
        return count;
    }
}

function getTimeRemaining(count) {
    if (count >= 172800) {
        return qsTr("the next %1 days").arg((count / 86400).toLocaleString(Qt.locale(), 'f', 0));
    } else if (count >= 86400) {
        return qsTr("the next day");
    } else if (count >= 7200 ) {
        return qsTr("the next %1 hours").arg((count / 3600).toLocaleString(Qt.locale(), 'f', 0));
    } if (count >= 3600 ) {
        return qsTr("the next hour");
    } else if (count >= 120 ) {
        return qsTr("the next %1 minutes").arg((count / 60).toLocaleString(Qt.locale(), 'f', 0));
    } else {
        return qsTr("the next minute");
    }
}

function containsPlace(tweetModel) {
    if (tweetModel.retweeted_status) {
        if (tweetModel.retweeted_status.place) {
            return true;
        }
    } else {
        if (tweetModel.place) {
            return true;
        }
    }
    return false;
}
