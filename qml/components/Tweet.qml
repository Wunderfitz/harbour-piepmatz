import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

ListItem {

    id: singleTweet

    property variant tweetModel;
    property string embeddedTweetId;
    property variant embeddedTweet;
    property bool hasEmbeddedTweet : false;

    contentHeight: tweetRow.height + tweetAdditionalRow.height + tweetSeparator.height + 3 * Theme.paddingMedium
    contentWidth: parent.width

    Column {
        id: tweetColumn
        width: parent.width - ( 2 * Theme.horizontalPageMargin )
        spacing: Theme.paddingSmall
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Row {
            id: tweetRow
            width: parent.width
            spacing: Theme.paddingMedium

            Column {
                id: tweetAuthorColumn
                width: parent.width / 6
                height: parent.width / 6
                spacing: Theme.paddingSmall
                Image {
                    id: tweetRetweetedImage
                    source: "image://theme/icon-s-retweet"
                    visible: tweetModel.retweeted_status ? true : false
                    anchors.right: parent.right
                    width: Theme.fontSizeExtraSmall
                    height: Theme.fontSizeExtraSmall
                }
                Image {
                    id: tweetInReplyToImage
                    source: "image://theme/icon-s-repost"
                    visible: tweetModel.in_reply_to_user_id_str ? true : false
                    anchors.right: parent.right
                    width: Theme.fontSizeExtraSmall
                    height: Theme.fontSizeExtraSmall
                }

                Item {
                    id: tweetAuthorItem
                    width: parent.width
                    height: parent.width

                    Component {
                        id: tweetAuthorPageComponent
                        ProfilePage {
                            profileModel: tweetModel.retweeted_status ? tweetModel.retweeted_status.user : tweetModel.user
                        }
                    }

                    Image {
                        id: tweetAuthorPicture
                        source: Functions.findBiggerImage(tweetModel.retweeted_status ? tweetModel.retweeted_status.user.profile_image_url_https : tweetModel.user.profile_image_url_https )
                        width: parent.width
                        height: parent.height
                        sourceSize {
                            width: parent.width
                            height: parent.height
                        }
                        visible: false
                    }

                    Rectangle {
                        id: tweetAuthorPictureMask
                        width: parent.width
                        height: parent.height
                        color: Theme.primaryColor
                        radius: parent.width / 7
                        visible: false
                    }

                    OpacityMask {
                        id: maskedTweetAuthorPicture
                        source: tweetAuthorPicture
                        maskSource: tweetAuthorPictureMask
                        anchors.fill: tweetAuthorPicture
                        visible: tweetAuthorPicture.status === Image.Ready ? true : false
                        opacity: tweetAuthorPicture.status === Image.Ready ? 1 : 0
                        Behavior on opacity { NumberAnimation {} }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push( tweetAuthorPageComponent );
                            }
                        }
                    }

                    ImageProgressIndicator {
                        image: tweetAuthorPicture
                        withPercentage: false
                    }

                }
            }

            Column {
                id: tweetContentColumn
                width: parent.width * 5 / 6 - Theme.horizontalPageMargin

                spacing: Theme.paddingSmall

                Component {
                    id: tweetRetweetedByPageComponent
                    ProfilePage {
                        profileModel: tweetModel.user
                    }
                }

                Component {
                    id: tweetInReplyToPageComponent
                    ProfilePage {
                        profileName: Functions.getScreenNameById(tweetModel.in_reply_to_user_id, tweetModel.user, tweetModel.entities.user_mentions)
                    }
                }

                Column {
                    Text {
                        id: tweetRetweetedText
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.secondaryColor
                        text: qsTr("Retweeted by %1").arg(tweetModel.user.name)
                        visible: tweetModel.retweeted_status ? true : false
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push( tweetRetweetedByPageComponent );
                            }
                        }
                    }

                    Text {
                        id: tweetInReplyToText
                        font.pixelSize: Theme.fontSizeTiny
                        color: Theme.secondaryColor
                        text: qsTr("In reply to %1").arg(Functions.getUserNameById(tweetModel.in_reply_to_user_id, tweetModel.user, tweetModel.entities.user_mentions))
                        visible: tweetModel.in_reply_to_user_id_str ? true : false
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push( tweetInReplyToPageComponent );
                            }
                        }
                    }
                }

                TweetUser {
                    id: tweetUserRow
                    tweet: tweetModel
                }

                TweetText {
                    id: tweetContentText
                    tweet: tweetModel
                }

                Row {
                    id: tweetInfoRow
                    width: parent.width
                    spacing: Theme.paddingSmall

                    Row {
                        width: parent.width / 2
                        Text {
                            id: tweetDateText
                            font.pixelSize: Theme.fontSizeTiny
                            color: Theme.secondaryColor
                            text:  Functions.getValidDate(tweetModel.retweeted_status ? tweetModel.retweeted_status.created_at : tweetModel.created_at).toLocaleString(Qt.locale(), Locale.ShortFormat)
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }
                    }

                    Row {
                        width: parent.width / 2
                        spacing: Theme.paddingSmall
                        Column {
                            width: parent.width / 6
                            Image {
                                id: tweetRetweetedCountImage
                                anchors.right: parent.right
                                source: "image://theme/icon-s-retweet"
                                width: Theme.fontSizeExtraSmall
                                height: Theme.fontSizeExtraSmall
                            }
                        }
                        Column {
                            width: parent.width / 3
                            Text {
                                id: tweetRetweetedCountText
                                font.pixelSize: Theme.fontSizeTiny
                                anchors.left: parent.left
                                color: Theme.secondaryColor
                                text: tweetModel.retweeted_status ? ( tweetModel.retweeted_status.retweet_count ? tweetModel.retweeted_status.retweet_count : " " ) : ( tweetModel.retweet_count ? tweetModel.retweet_count : " " )
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                        }
                        Column {
                            width: parent.width / 6
                            Image {
                                id: tweetFavoritesCountImage
                                anchors.right: parent.right
                                source: "image://theme/icon-s-favorite"
                                width: Theme.fontSizeExtraSmall
                                height: Theme.fontSizeExtraSmall
                            }
                        }
                        Column {
                            width: parent.width / 3
                            Text {
                                id: tweetFavoritesCountText
                                font.pixelSize: Theme.fontSizeTiny
                                anchors.left: parent.left
                                color: Theme.secondaryColor
                                text: tweetModel.retweeted_status ? ( tweetModel.retweeted_status.favorite_count ? tweetModel.retweeted_status.favorite_count : " " ) : ( tweetModel.favorite_count ? tweetModel.favorite_count : " " )
                                elide: Text.ElideRight
                                maximumLineCount: 1
                            }
                        }
                    }
                }
            }
        }

        Row {
            id: tweetAdditionalRow
            width: parent.width
            spacing: Theme.paddingMedium

            Column {
                id: dummyLeftColumn
                width: parent.width / 6
                height: Theme.fontSizeTiny
            }

            Column {
                id: tweetAdditionalRightColumn
                width: parent.width * 5 / 6 - Theme.horizontalPageMargin

                spacing: Theme.paddingSmall

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

                Connections {
                    target: twitterApi
                    onShowStatusSuccessful: {
                        if (embeddedTweetId === result.id_str) {
                            embeddedTweet = result;
                            hasEmbeddedTweet = true;
                        }
                    }
                }

                Component {
                    id: embeddedTweetComponent
                    EmbeddedTweet {
                        id: embeddedTweetItem
                        tweetModel: embeddedTweet
                        visible: hasEmbeddedTweet
                    }
                }

                Loader {
                    id: embeddedTweetLoader
                    active: hasEmbeddedTweet
                    width: parent.width
                    sourceComponent: embeddedTweetComponent
                }
            }
        }
    }

    Separator {
        id: tweetSeparator

        anchors {
            top: tweetColumn.bottom
            topMargin: Theme.paddingMedium
        }

        width: parent.width
        color: Theme.primaryColor
        horizontalAlignment: Qt.AlignHCenter
    }


}
