function findHiResImage(url) {
    var suffixIndex = url.indexOf("_normal");
    if (suffixIndex !== -1) {
        return url.substring(0, suffixIndex) + url.substring(suffixIndex + 7);
    } else {
        return url;
    }
}

function getValidDate(twitterDate) {
    return new Date(twitterDate.replace(/^(\w+) (\w+) (\d+) ([\d:]+) \+0000 (\d+)$/,"$1, $2 $3 $5 $4 GMT"));
} 
