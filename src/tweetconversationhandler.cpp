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
