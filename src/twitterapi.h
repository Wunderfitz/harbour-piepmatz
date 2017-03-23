#ifndef TWITTERAPI_H
#define TWITTERAPI_H

#include <QObject>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantMap>
#include <QVariantList>
#include "o1requestor.h"
#include "o0requestparameter.h"
#include "o0globals.h"

const char API_ACCOUNT_VERIFY_CREDENTIALS[] = "https://api.twitter.com/1.1/account/verify_credentials.json";
const char API_STATUSES_UPDATE[] = "https://api.twitter.com/1.1/statuses/update.json";
const char API_STATUSES_HOME_TIMELINE[] = "https://api.twitter.com/1.1/statuses/home_timeline.json";
const char API_STATUSES_SHOW[] = "https://api.twitter.com/1.1/statuses/show.json";
const char API_STATUSES_USER_TIMELINE[] = "https://api.twitter.com/1.1/statuses/user_timeline.json";
const char API_USERS_SHOW[] = "https://api.twitter.com/1.1/users/show.json";
const char API_FRIENDSHIPS_CREATE[] = "https://api.twitter.com/1.1/friendships/create.json";
const char API_FRIENDSHIPS_DESTROY[] = "https://api.twitter.com/1.1/friendships/destroy.json";

class TwitterApi : public QObject {

    Q_OBJECT
public:
    TwitterApi(O1Requestor* requestor, QObject* parent = 0);

    Q_INVOKABLE void verifyCredentials();
    Q_INVOKABLE void tweet(const QString &text);
    Q_INVOKABLE void homeTimeline();
    Q_INVOKABLE void userTimeline(const QString &screenName);
    Q_INVOKABLE void showStatus(const QString &statusId);
    Q_INVOKABLE void showUser(const QString &screenName);
    Q_INVOKABLE void followUser(const QString &screenName);
    Q_INVOKABLE void unfollowUser(const QString &screenName);

signals:
    void verifyCredentialsSuccessful(const QVariantMap &result);
    void verifyCredentialsError(const QString &errorMessage);
    void tweetSuccessful(const QVariantMap &result);
    void tweetError(const QString &errorMessage);
    void homeTimelineSuccessful(const QVariantList &result);
    void homeTimelineError(const QString &errorMessage);
    void userTimelineSuccessful(const QVariantList &result);
    void userTimelineError(const QString &errorMessage);
    void showStatusSuccessful(const QVariantMap &result);
    void showStatusError(const QString &errorMessage);
    void showUserSuccessful(const QVariantMap &result);
    void showUserError(const QString &errorMessage);
    void followUserSuccessful(const QVariantMap &result);
    void followUserError(const QString &errorMessage);
    void unfollowUserSuccessful(const QVariantMap &result);
    void unfollowUserError(const QString &errorMessage);

private:
    O1Requestor *requestor;

private slots:
    void handleVerifyCredentialsSuccessful();
    void handleVerifyCredentialsError(QNetworkReply::NetworkError error);
    void handleTweetError(QNetworkReply::NetworkError error);
    void handleTweetFinished();
    void handleHomeTimelineError(QNetworkReply::NetworkError error);
    void handleHomeTimelineFinished();
    void handleUserTimelineError(QNetworkReply::NetworkError error);
    void handleUserTimelineFinished();
    void handleShowStatusError(QNetworkReply::NetworkError error);
    void handleShowStatusFinished();
    void handleShowUserError(QNetworkReply::NetworkError error);
    void handleShowUserFinished();
    void handleFollowUserError(QNetworkReply::NetworkError error);
    void handleFollowUserFinished();
    void handleUnfollowUserError(QNetworkReply::NetworkError error);
    void handleUnfollowUserFinished();

};

#endif // TWITTERAPI_H
