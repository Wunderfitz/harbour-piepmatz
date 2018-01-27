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
