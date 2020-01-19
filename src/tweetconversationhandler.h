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
#ifndef TWEETCONVERSATIONHANDLER_H
#define TWEETCONVERSATIONHANDLER_H

#include <QObject>
#include "twitterapi.h"

class TweetConversationHandler : public QObject
{
    Q_OBJECT
public:
    explicit TweetConversationHandler(TwitterApi *twitterApi, QString tweetId, QVariantList relatedTweets, QObject *parent = 0);

    Q_INVOKABLE void buildConversation();

signals:
    void tweetConversationCompleted(const QString &tweetId, const QVariantList &receivedTweets);

public slots:

private slots:
    void handleShowStatusError(const QString &errorMessage);
    void handleShowStatusSuccessful(const QVariantMap &tweet);

private:
    QString tweetId;
    QVariantList relatedTweets;
    QVariantMap receivedTweets;
    TwitterApi *twitterApi;
    int tweetsReceived = 0;

    void handleTweetReceived();
};

#endif // TWEETCONVERSATIONHANDLER_H
