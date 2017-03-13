import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Item {

    id: embeddedTweet

    property variant tweetModel;

    width: parent.width
    height: embeddedTweetColumn.height

    Column {
        id: embeddedTweetColumn
        spacing: Theme.paddingMedium
        width: parent.width
        height: embeddedTweetSeparator.height + tweetUserRow.height + tweetContentText.height + 3 * ( Theme.paddingMedium ) + ( videoLoader.active ? videoLoader.height + Theme.paddingMedium : 0 ) + ( tweetImageSlideshow.visible ? tweetImageSlideshow.height + Theme.paddingMedium : 0 )

        Separator {
            id: embeddedTweetSeparator
            width: parent.width
            color: Theme.primaryColor
            horizontalAlignment: Qt.AlignHCenter
        }

        Row {
            id: tweetUserRow
            width: parent.width
            spacing: Theme.paddingSmall

            Text {
                id: tweetUserNameText
                font.pixelSize: Theme.fontSizeExtraSmall
                font.bold: true
                color: Theme.primaryColor
                text: tweetModel.retweeted_status ? tweetModel.retweeted_status.user.name : tweetModel.user.name
                elide: Text.ElideRight
                maximumLineCount: 1
            }

            Image {
                id: tweetUserVerifiedImage
                source: "image://theme/icon-s-installed"
                visible: tweetModel.retweeted_status ? tweetModel.retweeted_status.user.verified : tweetModel.user.verified
                width: Theme.fontSizeSmall
                height: Theme.fontSizeSmall
            }

            Text {
                id: tweetUserHandleText
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                anchors.bottom: tweetUserNameText.bottom
                text: qsTr("@%1").arg(tweetModel.retweeted_status ? tweetModel.retweeted_status.user.screen_name : tweetModel.user.screen_name)
                elide: Text.ElideRight
                maximumLineCount: 1
            }
        }

        Text {
            width: parent.width
            id: tweetContentText
            text: Functions.enhanceText(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.primaryColor
            wrapMode: Text.Wrap
            textFormat: Text.StyledText
            onLinkActivated: Qt.openUrlExternally(link)
            linkColor: Theme.highlightColor
        }

        ListModel {
            id: tweetImageListModel
        }

        SlideshowView {
            id: tweetImageSlideshow
            visible: Functions.hasImage(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel)
            width: parent.width
            height: parent.width * 2 / 3
            model: Functions.getTweetImageModel(tweetModel.retweeted_status ? tweetModel.retweeted_status : tweetModel, tweetImageListModel)
            delegate: Item {
                width: parent.width
                height: parent.height

                Component {
                    id: singleImageComponent
                    ImagePage {
                        imageUrl: media_url_https
                        imageHeight: sizes.large.h
                        imageWidth: sizes.large.w
                    }
                }

                Image {
                    id: tweetImage
                    source: media_url_https
                    width: parent.width
                    height: parent.height
                    sourceSize.width: parent.width
                    sourceSize.height: parent.height
                    fillMode: Image.PreserveAspectCrop
                    visible: status === Image.Ready ? true : false
                    opacity: status === Image.Ready ? 1 : 0
                    Behavior on opacity { NumberAnimation {} }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            pageStack.push( singleImageComponent );
                        }
                    }
                }
                ImageProgressIndicator {
                    image: tweetImage
                    withPercentage: true
                }
            }
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

    }

}
