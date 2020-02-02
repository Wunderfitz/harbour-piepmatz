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
    ownListsModel.update();
    membershipListsModel.update();
    savedSearchesModel.update();
    // Updated via MentionsModel
    // accountModel.verifyCredentials();
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
        var statusId = url.substring(url.indexOf("/status/") + 8);
        if (statusId.indexOf("?") !== -1) {
            statusId = statusId.substring(0, statusId.indexOf("?"));
        }
        return statusId;
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
    return enhanceTweetText(tweetText, entities, null, false, false);
}

function enhanceText(tweetText, entities, extendedEntities) {
    return enhanceTweetText(tweetText, entities, extendedEntities, true, true);
}

function enhanceTweetText(tweetText, entities, extendedEntities, withReferenceUrl, followEmbeddedTweet) {
    var replacements = [];

    TwitterText.convertUnicodeIndices(tweetText, entities.hashtags);
    TwitterText.convertUnicodeIndices(tweetText, entities.symbols);
    TwitterText.convertUnicodeIndices(tweetText, entities.urls);
    TwitterText.convertUnicodeIndices(tweetText, entities.user_mentions);

    // URLs
    for (var i = 0; i < entities.urls.length; i++ ) {
        var tweetId = getTweetId(entities.urls[i].expanded_url);
        if (tweetId !== null && followEmbeddedTweet) {
            // Remove tweet URLs - will become embedded tweets...
            embeddedTweetId = tweetId;
            twitterApi.showStatus(tweetId);
            replacements.push(new Replacement(entities.urls[i].indices[0], entities.urls[i].indices[1], entities.urls[i].url, ""));
        } else {
            var url_replacement = "<a href=\"" + entities.urls[i].expanded_url + "\">" + entities.urls[i].display_url + "</a>";
            replacements.push(new Replacement(entities.urls[i].indices[0], entities.urls[i].indices[1], entities.urls[i].url, url_replacement));
            // TODO: Could fail in case of multiple references. Well, let's see what happens :D
            if (withReferenceUrl && ( appWindow.linkPreviewMode === "always" || ( appWindow.linkPreviewMode === "wifiOnly" && appWindow.isWifi ) ) ) {
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

    tweetText = tweetText.replace(new RegExp("\r?\n", "g"), "<br>");

    var spaceRegex = /\s{2,}/g;
    function spaceReplacer(match, p1, offset, string) {
        var replaceString = "";
        for (var i = 0; i < match.length; i++) {
            replaceString += "&nbsp;";
        }
        return replaceString;
    }
    tweetText = tweetText.replace(spaceRegex, spaceReplacer);

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
    if (tweet.extended_entities && listModel.rowCount() === 0) {
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
    var originalHeight = 1;
    if (tweet.extended_entities) {
        for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
            if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                originalHeight = tweet.extended_entities.media[i].video_info.aspect_ratio[1];
                break;
            }
        }
    }
    return originalHeight;
}

function getVideoOriginalWidth(tweet) {
    var originalWidth = 1;
    if (tweet.extended_entities) {
        for (var i = 0; i < tweet.extended_entities.media.length; i++ ) {
            if (tweet.extended_entities.media[i].type === "video" || tweet.extended_entities.media[i].type === "animated_gif") {
                originalWidth = tweet.extended_entities.media[i].video_info.aspect_ratio[0];
                break;
            }
        }
    }
    return originalWidth;
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

function getMediaFileName(tweet, mediaUrl) {
    var fileNamePrefix = tweet.user.screen_name + "_" + Qt.formatDate(Functions.getValidDate(tweet.retweeted_status ? tweet.retweeted_status.created_at : tweet.created_at), "yyyy-MM-dd");
    if (tweet.retweeted_status) {
        fileNamePrefix = tweet.retweeted_status.user.screen_name + "_" + Qt.formatDate(Functions.getValidDate(tweet.retweeted_status ? tweet.retweeted_status.created_at : tweet.created_at), "yyyy-MM-dd");
    }
    var lastIndexOf = (mediaUrl.indexOf("?") > -1) ? mediaUrl.indexOf("?") : mediaUrl.length;
    return fileNamePrefix + "_" + mediaUrl.substring(mediaUrl.lastIndexOf("/") + 1, lastIndexOf);
}

function htmlDecode(input)
{
    function replacer(match, p1, offset, string) {
        var htmlEntities = {
        Acirc:	"Â",
        acirc:	"Â",
        acute:	"´",
        AElig:	"Æ",
        aelig:	"æ",
        Agrave:	"À",
        agrave:	"à",
        alefsym:	"ℵ",
        Alpha:	"Α",
        alpha:	"α",
        amp:	"&",
        and:	"∧",
        ang:	"∠",
        apos:	"'",
        Aring:	"Å",
        aring:	"å",
        asymp:	"≈",
        Atilde:	"Ã",
        atilde:	"ã",
        Auml:	"Ä",
        auml:	"ä",
        bdquo:	"„",
        Beta:	"Β",
        beta:	"β",
        brvbar:	"¦",
        bull:	"•",
        cap:	"∩",
        Ccedil:	"Ç",
        ccedil:	"ç",
        cedil:	"¸",
        cent:	"¢",
        Chi:	"Χ",
        chi:	"χ",
        circ:	"ˆ",
        clubs:	"♣",
        cong:	"≅",
        copy:	"©",
        crarr:	"↵",
        cup:	"∪",
        curren:	"¤",
        Dagger:	"‡",
        dagger:	"†",
        dArr:	"⇓",
        darr:	"↓",
        deg:	"°",
        Delta:	"Δ",
        delta:	"δ",
        diams:	"♦",
        divide:	"÷",
        Eacute:	"É",
        eacute:	"é",
        Ecirc:	"Ê",
        ecirc:	"ê",
        Egrave:	"È",
        egrave:	"è",
        empty:	"∅",
        Epsilon:	"Ε",
        epsilon:	"ε",
        equiv:	"≡",
        Eta:	"Η",
        eta:	"η",
        ETH:	"Ð",
        eth:	"ð",
        Euml:	"Ë",
        euml:	"ë",
        euro:	"€",
        exist:	"∃",
        fnof:	"ƒ",
        forall:	"∀",
        frac12:	"½",
        frac14:	"¼",
        frac34:	"¾",
        frasl:	"⁄",
        Gamma:	"Γ",
        gamma:	"γ",
        ge:	"≥",
        gt:	">",
        hArr:	"⇔",
        harr:	"↔",
        hearts:	"♥",
        hellip:	"…",
        Iacute:	"Í",
        iacute:	"í",
        Icirc:	"Î",
        icirc:	"î",
        iexcl:	"¡",
        Igrave:	"Ì",
        igrave:	"ì",
        image:	"ℑ",
        infin:	"∞",
        int:	"∫",
        Iota:	"Ι",
        iota:	"ι",
        iquest:	"¿",
        isin:	"∈",
        Iuml:	"Ï",
        iuml:	"ï",
        Kappa:	"Κ",
        kappa:	"κ",
        Lambda:	"Λ",
        lambda:	"λ",
        lang:	"⟨",
        laquo:	"«",
        lArr:	"⇐",
        larr:	"←",
        lceil:	"⌈",
        ldquo:	"“",
        le:	"≤",
        lfloor:	"⌊",
        lowast:	"∗",
        loz:	"◊",
        lsaquo:	"‹",
        lsquo:	"‘",
        lt:	"<",
        macr:	"¯",
        mdash:	"—",
        micro:	"µ",
        middot:	"·",
        minus:	"−",
        Mu:	"Μ",
        mu:	"μ",
        nabla:	"∇",
        ndash:	"–",
        ne:	"≠",
        ni:	"∋",
        not:	"¬",
        notin:	"∉",
        nsub:	"⊄",
        Ntilde:	"Ñ",
        ntilde:	"ñ",
        Nu:	"Ν",
        nu:	"ν",
        Oacute:	"Ó",
        oacute:	"ó",
        Ocirc:	"Ô",
        ocirc:	"ô",
        OElig:	"Œ",
        oelig:	"œ",
        Ograve:	"Ò",
        ograve:	"ò",
        oline:	"‾",
        Omega:	"Ω",
        omega:	"ω",
        Omicron:	"Ο",
        omicron:	"ο",
        oplus:	"⊕",
        or:	"∨",
        ordf:	"ª",
        ordm:	"º",
        Oslash:	"Ø",
        oslash:	"ø",
        Otilde:	"Õ",
        otilde:	"õ",
        otimes:	"⊗",
        Ouml:	"Ö",
        ouml:	"ö",
        para:	"¶",
        part:	"∂",
        permil:	"‰",
        perp:	"⊥",
        Phi:	"Φ",
        phi:	"φ",
        Pi:	"Π",
        pi:	"π",
        piv:	"ϖ",
        plusmn:	"±",
        pound:	"£",
        Prime:	"″",
        prime:	"′",
        prod:	"∏",
        prop:	"∝",
        Psi:	"Ψ",
        psi:	"ψ",
        quot:	"\"",
        radic:	"√",
        rang:	"⟩",
        raquo:	"»",
        rArr:	"⇒",
        rarr:	"→",
        rceil:	"⌉",
        rdquo:	"”",
        real:	"ℜ",
        reg:	"®",
        rfloor:	"⌋",
        Rho:	"Ρ",
        rho:	"ρ",
        rsaquo:	"›",
        rsquo:	"’",
        sbquo:	"‚",
        Scaron:	"Š",
        scaron:	"š",
        sdot:	"⋅",
        sect:	"§",
        Sigma:	"Σ",
        sigma:	"σ",
        sigmaf:	"ς",
        sim:	"∼",
        spades:	"♠",
        sub:	"⊂",
        sube:	"⊆",
        sum:	"∑",
        sup:	"⊃",
        sup1:	"¹",
        sup2:	"²",
        sup3:	"³",
        supe:	"⊇",
        szlig:	"ß",
        Tau:	"Τ",
        tau:	"τ",
        there4:	"∴",
        Theta:	"Θ",
        theta:	"θ",
        thetasym:	"ϑ",
        THORN:	"Þ",
        thorn:	"þ",
        tilde:	"˜",
        times:	"×",
        trade:	"™",
        Uacute:	"Ú",
        uacute:	"ú",
        uArr:	"⇑",
        uarr:	"↑",
        Ucirc:	"Û",
        ucirc:	"û",
        Ugrave:	"Ù",
        ugrave:	"ù",
        uml:	"¨",
        upsih:	"ϒ",
        Upsilon:	"Υ",
        upsilon:	"υ",
        Uuml:	"Ü",
        uuml:	"ü",
        weierp:	"℘",
        Xi:	"Ξ",
        xi:	"ξ",
        Yacute:	"Ý",
        yacute:	"ý",
        yen:	"¥",
        Yuml:	"Ÿ",
        yuml:	"ÿ",
        Zeta:	"Ζ",
        zeta:	"ζ"
        };

        if (typeof htmlEntities[p1] === "undefined") {
            return match;
        } else {
            return htmlEntities[p1];
        }
    }

    function replacerUnicode(match, p1, offset, string) {
        var htmlEntitiesUnicode = {
            XA1: "¡",
            XA2: "¢",
            XA3: "£",
            XA4: "¤",
            XA5: "¥",
            XA6: "¦",
            XA7: "§",
            XA8: "¨",
            XA9: "©",
            XAA: "ª",
            XAB: "«",
            XAC: "¬",
            XAD: "­",
            XAE: "®",
            XAF: "¯",
            XB0: "°",
            XB1: "±",
            XB2: "²",
            XB3: "³",
            XB4: "´",
            XB5: "µ",
            XB6: "¶",
            XB7: "·",
            XB8: "¸",
            XB9: "¹",
            XBA: "º",
            XBB: "»",
            XBC: "¼",
            XBD: "½",
            XBE: "¾",
            XBF: "¿",
            XC0: "À",
            XC1: "Á",
            XC2: "Â",
            XC3: "Ã",
            XC4: "Ä",
            XC5: "Å",
            XC6: "Æ",
            XC7: "Ç",
            XC8: "È",
            XC9: "É",
            XCA: "Ê",
            XCB: "Ë",
            XCC: "Ì",
            XCD: "Í",
            XCE: "Î",
            XCF: "Ï",
            XD0: "Ð",
            XD1: "Ñ",
            XD2: "Ò",
            XD3: "Ó",
            XD4: "Ô",
            XD5: "Õ",
            XD6: "Ö",
            XD7: "×",
            XD8: "Ø",
            XD9: "Ù",
            XDA: "Ú",
            XDB: "Û",
            XDC: "Ü",
            XDD: "Ý",
            XDE: "Þ",
            XDF: "ß",
            XE0: "à",
            XE1: "á",
            XE2: "â",
            XE3: "ã",
            XE4: "ä",
            XE5: "å",
            XE6: "æ",
            XE7: "ç",
            XE8: "è",
            XE9: "é",
            XEA: "ê",
            XEB: "ë",
            XEC: "ì",
            XED: "í",
            XEE: "î",
            XEF: "ï",
            XF0: "ð",
            XF1: "ñ",
            XF2: "ò",
            XF3: "ó",
            XF4: "ô",
            XF5: "õ",
            XF6: "ö",
            XF7: "÷",
            XF8: "ø",
            XF9: "ù",
            XFA: "ú",
            XFB: "û",
            XFC: "ü",
            XFD: "ý",
            XFE: "þ",
            XFF: "ÿ",
            X100: "Ā",
            X101: "ā",
            X102: "Ă",
            X103: "ă",
            X104: "Ą",
            X105: "ą",
            X106: "Ć",
            X107: "ć",
            X108: "Ĉ",
            X109: "ĉ",
            X10A: "Ċ",
            X10B: "ċ",
            X10C: "Č",
            X10D: "č",
            X10E: "Ď",
            X10F: "ď",
            X110: "Đ",
            X111: "đ",
            X112: "Ē",
            X113: "ē",
            X114: "Ĕ",
            X115: "ĕ",
            X116: "Ė",
            X117: "ė",
            X118: "Ę",
            X119: "ę",
            X11A: "Ě",
            X11B: "ě",
            X11C: "Ĝ",
            X11D: "ĝ",
            X11E: "Ğ",
            X11F: "ğ",
            X120: "Ġ",
            X121: "ġ",
            X122: "Ģ",
            X123: "ģ",
            X124: "Ĥ",
            X125: "ĥ",
            X126: "Ħ",
            X127: "ħ",
            X128: "Ĩ",
            X129: "ĩ",
            X12A: "Ī",
            X12B: "ī",
            X12C: "Ĭ",
            X12D: "ĭ",
            X12E: "Į",
            X12F: "į",
            X130: "İ",
            X131: "ı",
            X132: "Ĳ",
            X133: "ĳ",
            X134: "Ĵ",
            X135: "ĵ",
            X136: "Ķ",
            X137: "ķ",
            X138: "ĸ",
            X139: "Ĺ",
            X13A: "ĺ",
            X13B: "Ļ",
            X13C: "ļ",
            X13D: "Ľ",
            X13E: "ľ",
            X13F: "Ŀ",
            X140: "ŀ",
            X141: "Ł",
            X142: "ł",
            X143: "Ń",
            X144: "ń",
            X145: "Ņ",
            X146: "ņ",
            X147: "Ň",
            X148: "ň",
            X149: "ŉ",
            X14A: "Ŋ",
            X14B: "ŋ",
            X14C: "Ō",
            X14D: "ō",
            X14E: "Ŏ",
            X14F: "ŏ",
            X150: "Ő",
            X151: "ő",
            X152: "Œ",
            X153: "œ",
            X154: "Ŕ",
            X155: "ŕ",
            X156: "Ŗ",
            X157: "ŗ",
            X158: "Ř",
            X159: "ř",
            X15A: "Ś",
            X15B: "ś",
            X15C: "Ŝ",
            X15D: "ŝ",
            X15E: "Ş",
            X15F: "ş",
            X160: "Š",
            X161: "š",
            X162: "Ţ",
            X163: "ţ",
            X164: "Ť",
            X165: "ť",
            X166: "Ŧ",
            X167: "ŧ",
            X168: "Ũ",
            X169: "ũ",
            X16A: "Ū",
            X16B: "ū",
            X16C: "Ŭ",
            X16D: "ŭ",
            X16E: "Ů",
            X16F: "ů",
            X170: "Ű",
            X171: "ű",
            X172: "Ų",
            X173: "ų",
            X174: "Ŵ",
            X175: "ŵ",
            X176: "Ŷ",
            X177: "ŷ",
            X178: "Ÿ",
            X179: "Ź",
            X17A: "ź",
            X17B: "Ż",
            X17C: "ż",
            X17D: "Ž",
            X17E: "ž",
            X17F: "ſ",
            X400: "Ѐ",
            X401: "Ё",
            X402: "Ђ",
            X403: "Ѓ",
            X404: "Є",
            X405: "Ѕ",
            X406: "І",
            X407: "Ї",
            X408: "Ј",
            X409: "Љ",
            X40A: "Њ",
            X40B: "Ћ",
            X40C: "Ќ",
            X40D: "Ѝ",
            X40E: "Ў",
            X40F: "Џ",
            X410: "А",
            X411: "Б",
            X412: "В",
            X413: "Г",
            X414: "Д",
            X415: "Е",
            X416: "Ж",
            X417: "З",
            X418: "И",
            X419: "Й",
            X41A: "К",
            X41B: "Л",
            X41C: "М",
            X41D: "Н",
            X41E: "О",
            X41F: "П",
            X420: "Р",
            X421: "С",
            X422: "Т",
            X423: "У",
            X424: "Ф",
            X425: "Х",
            X426: "Ц",
            X427: "Ч",
            X428: "Ш",
            X429: "Щ",
            X42A: "Ъ",
            X42B: "Ы",
            X42C: "Ь",
            X42D: "Э",
            X42E: "Ю",
            X42F: "Я",
            X430: "а",
            X431: "б",
            X432: "в",
            X433: "г",
            X434: "д",
            X435: "е",
            X436: "ж",
            X437: "з",
            X438: "и",
            X439: "й",
            X43A: "к",
            X43B: "л",
            X43C: "м",
            X43D: "н",
            X43E: "о",
            X43F: "п",
            X440: "р",
            X441: "с",
            X442: "т",
            X443: "у",
            X444: "ф",
            X445: "х",
            X446: "ц",
            X447: "ч",
            X448: "ш",
            X449: "щ",
            X44A: "ъ",
            X44B: "ы",
            X44C: "ь",
            X44D: "э",
            X44E: "ю",
            X44F: "я",
            X450: "ѐ",
            X451: "ё",
            X452: "ђ",
            X453: "ѓ",
            X454: "є",
            X455: "ѕ",
            X456: "і",
            X457: "ї",
            X458: "ј",
            X459: "љ",
            X45A: "њ",
            X45B: "ћ",
            X45C: "ќ",
            X45D: "ѝ",
            X45E: "ў",
            X45F: "џ",
            X460: "Ѡ",
            X461: "ѡ",
            X462: "Ѣ",
            X463: "ѣ",
            X464: "Ѥ",
            X465: "ѥ",
            X466: "Ѧ",
            X467: "ѧ",
            X468: "Ѩ",
            X469: "ѩ",
            X46A: "Ѫ",
            X46B: "ѫ",
            X46C: "Ѭ",
            X46D: "ѭ",
            X46E: "Ѯ",
            X46F: "ѯ",
            X470: "Ѱ",
            X471: "ѱ",
            X472: "Ѳ",
            X473: "ѳ",
            X474: "Ѵ",
            X475: "ѵ",
            X476: "Ѷ",
            X477: "ѷ",
            X478: "Ѹ",
            X479: "ѹ",
            X47A: "Ѻ",
            X47B: "ѻ",
            X47C: "Ѽ",
            X47D: "ѽ",
            X47E: "Ѿ",
            X47F: "ѿ",
            X480: "Ҁ",
            X481: "ҁ",
            X482: "҂",
            X483: "҃",
            X484: "҄",
            X485: "҅",
            X486: "҆",
            X487: "҇",
            X488: "҈",
            X489: "҉",
            X48A: "Ҋ",
            X48B: "ҋ",
            X48C: "Ҍ",
            X48D: "ҍ",
            X48E: "Ҏ",
            X48F: "ҏ",
            X490: "Ґ",
            X491: "ґ",
            X492: "Ғ",
            X493: "ғ",
            X494: "Ҕ",
            X495: "ҕ",
            X496: "Җ",
            X497: "җ",
            X498: "Ҙ",
            X499: "ҙ",
            X49A: "Қ",
            X49B: "қ",
            X49C: "Ҝ",
            X49D: "ҝ",
            X49E: "Ҟ",
            X49F: "ҟ",
            X4A0: "Ҡ",
            X4A1: "ҡ",
            X4A2: "Ң",
            X4A3: "ң",
            X4A4: "Ҥ",
            X4A5: "ҥ",
            X4A6: "Ҧ",
            X4A7: "ҧ",
            X4A8: "Ҩ",
            X4A9: "ҩ",
            X4AA: "Ҫ",
            X4AB: "ҫ",
            X4AC: "Ҭ",
            X4AD: "ҭ",
            X4AE: "Ү",
            X4AF: "ү",
            X4B0: "Ұ",
            X4B1: "ұ",
            X4B2: "Ҳ",
            X4B3: "ҳ",
            X4B4: "Ҵ",
            X4B5: "ҵ",
            X4B6: "Ҷ",
            X4B7: "ҷ",
            X4B8: "Ҹ",
            X4B9: "ҹ",
            X4BA: "Һ",
            X4BB: "һ",
            X4BC: "Ҽ",
            X4BD: "ҽ",
            X4BE: "Ҿ",
            X4BF: "ҿ",
            X4C0: "Ӏ",
            X4C1: "Ӂ",
            X4C2: "ӂ",
            X4C3: "Ӄ",
            X4C4: "ӄ",
            X4C5: "Ӆ",
            X4C6: "ӆ",
            X4C7: "Ӈ",
            X4C8: "ӈ",
            X4C9: "Ӊ",
            X4CA: "ӊ",
            X4CB: "Ӌ",
            X4CC: "ӌ",
            X4CD: "Ӎ",
            X4CE: "ӎ",
            X4CF: "ӏ",
            X4D0: "Ӑ",
            X4D1: "ӑ",
            X4D2: "Ӓ",
            X4D3: "ӓ",
            X4D4: "Ӕ",
            X4D5: "ӕ",
            X4D6: "Ӗ",
            X4D7: "ӗ",
            X4D8: "Ә",
            X4D9: "ә",
            X4DA: "Ӛ",
            X4DB: "ӛ",
            X4DC: "Ӝ",
            X4DD: "ӝ",
            X4DE: "Ӟ",
            X4DF: "ӟ",
            X4E0: "Ӡ",
            X4E1: "ӡ",
            X4E2: "Ӣ",
            X4E3: "ӣ",
            X4E4: "Ӥ",
            X4E5: "ӥ",
            X4E6: "Ӧ",
            X4E7: "ӧ",
            X4E8: "Ө",
            X4E9: "ө",
            X4EA: "Ӫ",
            X4EB: "ӫ",
            X4EC: "Ӭ",
            X4ED: "ӭ",
            X4EE: "Ӯ",
            X4EF: "ӯ",
            X4F0: "Ӱ",
            X4F1: "ӱ",
            X4F2: "Ӳ",
            X4F3: "ӳ",
            X4F4: "Ӵ",
            X4F5: "ӵ",
            X4F6: "Ӷ",
            X4F7: "ӷ",
            X4F8: "Ӹ",
            X4F9: "ӹ",
            X4FA: "Ӻ",
            X4FB: "ӻ",
            X4FC: "Ӽ",
            X4FD: "ӽ",
            X4FE: "Ӿ",
            X4FF: "ӿ"
        };
        var integerValue = parseInt(p1);
        if (!isNaN(integerValue)) {
            p1 = "X" + integerValue.toString(16);
        }

        var capitalizedUnicode = p1.toUpperCase();
        if (typeof htmlEntitiesUnicode[capitalizedUnicode] === "undefined") {
            return match;
        } else {
            return htmlEntitiesUnicode[capitalizedUnicode];
        }
    }

    var processedText = input.replace(/\&([a-zA-Z0-9]*)\;/g, replacer);
    processedText = processedText.replace(/\&\#([a-zA-Z0-9]*)\;/g, replacerUnicode);

    return processedText;
}
