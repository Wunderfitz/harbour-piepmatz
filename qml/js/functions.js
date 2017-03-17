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

function Replacement(beginOffset, endOffset, replacementString) {
    this.beginOffset = beginOffset;
    this.endOffset = endOffset;
    this.replacementString = replacementString;
}

Replacement.prototype.toString = function replacementToString() {
    return "Begin Offset: " + this.beginOffset + ", End Offset: " + this.endOffset + ", Replacement String: " + this.replacementString;
}

function enhanceText(tweet) {
    var tweetText = tweet.full_text;

    var replacements = [];

    // URLs
    for (var i = 0; i < tweet.entities.urls.length; i++ ) {
        var tweetId = getTweetId(tweet.entities.urls[i].expanded_url);
        if (tweetId !== null) {
            // Remove tweet URLs - will become embedded tweets...
            embeddedTweetId = tweetId;
            twitterApi.showStatus(tweetId);
            replacements.push(new Replacement(tweet.entities.urls[i].indices[0], tweet.entities.urls[i].indices[1], ""));
        } else {
            var url_replacement = "<a href=\"" + tweet.entities.urls[i].expanded_url + "\">" + tweet.entities.urls[i].display_url + "</a>";
            replacements.push(new Replacement(tweet.entities.urls[i].indices[0], tweet.entities.urls[i].indices[1], url_replacement));
        }
    }
    // Remove media links - will become own QML entities
    if (tweet.extended_entities) {
        for (var j = 0; j < tweet.extended_entities.media.length; j++ ) {
            replacements.push(new Replacement(tweet.extended_entities.media[j].indices[0], tweet.extended_entities.media[j].indices[1], ""));
        }
    }
    // User Mentions
    for (var k = 0; k < tweet.entities.user_mentions.length; k++ ) {
        var user_mention = tweetText.substring(tweet.entities.user_mentions[k].indices[0], tweet.entities.user_mentions[k].indices[1]);
        var user_mention_url = "<a href=\"profile://" + tweet.entities.user_mentions[k].screen_name + "\">" + user_mention + "</a>";
        replacements.push(new Replacement(tweet.entities.user_mentions[k].indices[0], tweet.entities.user_mentions[k].indices[1], user_mention_url));
    }
    // Hashtags
    for (var l = 0; l < tweet.entities.hashtags.length; l++ ) {
        var hashtag = tweetText.substring(tweet.entities.hashtags[l].indices[0], tweet.entities.hashtags[l].indices[1]);
        var hashtag_url = "<a href=\"tag://" + hashtag + "\">" + hashtag + "</a>";
        replacements.push(new Replacement(tweet.entities.hashtags[l].indices[0], tweet.entities.hashtags[l].indices[1], hashtag_url));
    }

    replacements.sort( function(a,b) { return b.beginOffset - a.beginOffset } );
    for (var m = 0; m < replacements.length; m++) {
        tweetText = tweetText.substring(0, replacements[m].beginOffset) + replacements[m].replacementString + tweetText.substring(replacements[m].endOffset);
    }

    return tweetText;
}

function handleLink(link) {
    if (link.indexOf("profile://") === 0) {
        console.log("Profile clicked: " + link);
        var profileComponent = Qt.createComponent("../pages/ProfilePage.qml");
        var profilePage = profileComponent.createObject(appWindow, {"profileName": link.substring(10)});
        pageStack.push(profilePage);
    } else if (link.indexOf("tag://") === 0) {
        console.log("Hashtag clicked: " + link);
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
