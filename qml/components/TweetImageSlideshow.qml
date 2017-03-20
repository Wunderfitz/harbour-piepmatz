import QtQuick 2.0
import Sailfish.Silica 1.0
import "../pages"
import "../js/functions.js" as Functions

Column {

    property variant tweet;

    width: parent.width
    height: parent.width * 2 / 3

    ListModel {
        id: tweetImageListModel
    }

    SlideshowView {
        id: tweetImageSlideshow
        width: parent.width
        height: parent.height
        model: Functions.getTweetImageModel(tweet.retweeted_status ? tweet.retweeted_status : tweet, tweetImageListModel)
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

}
