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
    var regex = /https:\/\/twitter\.com\/\w+\/status\/(\d+)/g;
    var matchingResult = regex.exec(url);
    if (matchingResult !== null) {
        return matchingResult[1];
    }
    return null;
}

function enhanceText(tweet) {
    var tweetText = tweet.full_text;
    // URLs
    for (var i = 0; i < tweet.entities.urls.length; i++ ) {
        var tweetId = getTweetId(tweet.entities.urls[i].expanded_url);
        if (tweetId !== null) {
            // Remove tweet URLs - will become embedded tweets...
            embeddedTweetId = tweetId;
            twitterApi.showStatus(tweetId);
            tweetText = tweetText.replace(tweet.entities.urls[i].url, "");
        } else {
            tweetText = tweetText.replace(tweet.entities.urls[i].url, "<a href=\"" + tweet.entities.urls[i].expanded_url + "\">" + tweet.entities.urls[i].display_url + "</a>");
        }
    }
    // Remove media links - will become own QML entities
    if (tweet.extended_entities) {
        for (var j = 0; j < tweet.extended_entities.media.length; j++ ) {
            tweetText = tweetText.replace(tweet.extended_entities.media[j].url, "");
        }
    }
    // User Mentions
    var mention_replacements = [];
    for (var k = 0; k < tweet.entities.user_mentions.length; k++ ) {
        var user_mention = tweetText.substring(tweet.entities.user_mentions[k].indices[0], tweet.entities.user_mentions[k].indices[1]);
        var user_mention_url = "<a href=\"profile://" + tweet.entities.user_mentions[k].screen_name + "\">" + user_mention + "</a>";
        mention_replacements.push({ mention: user_mention, url: user_mention_url });
        // TODO: Replace correctly... *sigh*
    }
    return tweetText;
}

function handleLink(link) {
    console.log(link);
    if (link.indexOf("profile://") === 0) {
        var profileComponent = Qt.createComponent("../Profile.qml");
        // TODO: Open profile page...
    } else {
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
