#include "tweetconversationhandler.h"

TweetConversationHandler::TweetConversationHandler(TwitterApi *twitterApi, QString tweetId, QVariantList relatedTweets, QObject *parent) : QObject(parent)
{
    this->tweetId = tweetId;
    this->relatedTweets = relatedTweets;
    this->twitterApi = twitterApi;

    connect(this->twitterApi, SIGNAL(showStatusError(QString)), this, SLOT(handleShowStatusError(QString)));
    connect(this->twitterApi, SIGNAL(showStatusSuccessful(QVariantMap)), this, SLOT(handleShowStatusSuccessful(QVariantMap)));
}

void TweetConversationHandler::buildConversation()
{
    if (this->relatedTweets.size() > 1) {
        this->tweetsReceived = 0;
        QListIterator<QVariant> relatedTweetIterator(relatedTweets);
        while (relatedTweetIterator.hasNext()) {
            QString relatedTweetId = relatedTweetIterator.next().toString();
            twitterApi->showStatus(relatedTweetId);
        }
    }
}

void TweetConversationHandler::handleShowStatusError(const QString &errorMessage)
{
    qDebug() << "Error receiving tweet from a conversation" << errorMessage;
    this->handleTweetReceived();
}

void TweetConversationHandler::handleShowStatusSuccessful(const QVariantMap &tweet)
{
    QString tweetId = tweet.value("id_str").toString();
    if (this->relatedTweets.contains(tweetId)) {
        qDebug() << "Received tweet from a conversation" << tweetId;
        this->receivedTweets.insert(tweet.value("id_str").toString(), tweet);
        this->handleTweetReceived();
    }
}

void TweetConversationHandler::handleTweetReceived()
{
    this->tweetsReceived++;
    if (this->tweetsReceived >= this->relatedTweets.size()) {
        qDebug() << "Tweet conversation completed";
        QVariantList conversationTweets;
        QListIterator<QVariant> relatedTweetsIterator(this->relatedTweets);
        while (relatedTweetsIterator.hasNext()) {
            QString processedTweetId = relatedTweetsIterator.next().toString();
            qDebug() << "Processing Tweet ID: " << processedTweetId;
            QVariant foundTweet = this->receivedTweets.value(processedTweetId);
            if (!foundTweet.isNull()) {
                qDebug() << "Adding tweet to conversation";
                conversationTweets.append(foundTweet);
            }
        }
        emit tweetConversationCompleted(this->tweetId, conversationTweets);
        deleteLater();
    }
}
