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
const char API_HELP_CONFIGURATION[] = "https://api.twitter.com/1.1/help/configuration.json";
const char API_HELP_PRIVACY[] = "https://api.twitter.com/1.1/help/privacy.json";
const char API_HELP_TOS[] = "https://api.twitter.com/1.1/help/tos.json";
const char API_MEDIA_UPLOAD[] = "https://upload.twitter.com/1.1/media/upload.json";
const char API_STATUSES_UPDATE[] = "https://api.twitter.com/1.1/statuses/update.json";
const char API_STATUSES_HOME_TIMELINE[] = "https://api.twitter.com/1.1/statuses/home_timeline.json";
const char API_STATUSES_MENTIONS_TIMELINE[] = "https://api.twitter.com/1.1/statuses/mentions_timeline.json";
const char API_FOLLOWERS_LIST[] = "https://api.twitter.com/1.1/followers/list.json";
const char API_FRIENDS_LIST[] = "https://api.twitter.com/1.1/friends/list.json";
const char API_STATUSES_SHOW[] = "https://api.twitter.com/1.1/statuses/show.json";
const char API_STATUSES_USER_TIMELINE[] = "https://api.twitter.com/1.1/statuses/user_timeline.json";
const char API_STATUSES_RETWEET[] = "https://api.twitter.com/1.1/statuses/retweet/:id.json";
const char API_STATUSES_UNRETWEET[] = "https://api.twitter.com/1.1/statuses/unretweet/:id.json";
const char API_USERS_SHOW[] = "https://api.twitter.com/1.1/users/show.json";
const char API_FRIENDSHIPS_CREATE[] = "https://api.twitter.com/1.1/friendships/create.json";
const char API_FRIENDSHIPS_DESTROY[] = "https://api.twitter.com/1.1/friendships/destroy.json";
const char API_SEARCH_TWEETS[] = "https://api.twitter.com/1.1/search/tweets.json";
const char API_FAVORITES_CREATE[] = "https://api.twitter.com/1.1/favorites/create.json";
const char API_FAVORITES_DESTROY[] = "https://api.twitter.com/1.1/favorites/destroy.json";

class TwitterApi : public QObject {

    Q_OBJECT
public:
    TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, QObject* parent = 0);

    Q_INVOKABLE void verifyCredentials();
    Q_INVOKABLE void helpConfiguration();
    Q_INVOKABLE void helpPrivacy();
    Q_INVOKABLE void helpTos();
    Q_INVOKABLE void tweet(const QString &text);
    Q_INVOKABLE void replyToTweet(const QString &text, const QString &replyToStatusId);
    Q_INVOKABLE void tweetWithImages(const QString &text, const QString &mediaIds);
    Q_INVOKABLE void replyToTweetWithImages(const QString &text, const QString &replyToStatusId, const QString &mediaIds);
    Q_INVOKABLE void homeTimeline();
    Q_INVOKABLE void mentionsTimeline();
    Q_INVOKABLE void userTimeline(const QString &screenName);
    Q_INVOKABLE void followers(const QString &screenName);
    Q_INVOKABLE void friends(const QString &screenName);
    Q_INVOKABLE void showStatus(const QString &statusId);
    Q_INVOKABLE void showUser(const QString &screenName);
    Q_INVOKABLE void followUser(const QString &screenName);
    Q_INVOKABLE void unfollowUser(const QString &screenName);
    Q_INVOKABLE void searchTweets(const QString &query);
    Q_INVOKABLE void favorite(const QString &statusId);
    Q_INVOKABLE void unfavorite(const QString &statusId);
    Q_INVOKABLE void retweet(const QString &statusId);
    Q_INVOKABLE void unretweet(const QString &statusId);
    Q_INVOKABLE void uploadImage(const QString &fileName);

    Q_INVOKABLE void getOpenGraph(const QString &address);

signals:
    void verifyCredentialsSuccessful(const QVariantMap &result);
    void verifyCredentialsError(const QString &errorMessage);
    void helpConfigurationSuccessful(const QVariantMap &result);
    void helpConfigurationError(const QString &errorMessage);
    void helpPrivacySuccessful(const QVariantMap &result);
    void helpPrivacyError(const QString &errorMessage);
    void helpTosSuccessful(const QVariantMap &result);
    void helpTosError(const QString &errorMessage);
    void tweetSuccessful(const QVariantMap &result);
    void tweetError(const QString &errorMessage);
    void homeTimelineSuccessful(const QVariantList &result);
    void homeTimelineError(const QString &errorMessage);
    void mentionsTimelineSuccessful(const QVariantList &result);
    void mentionsTimelineError(const QString &errorMessage);
    void userTimelineSuccessful(const QVariantList &result);
    void userTimelineError(const QString &errorMessage);
    void followersSuccessful(const QVariantMap &result);
    void followersError(const QString &errorMessage);
    void friendsSuccessful(const QVariantMap &result);
    void friendsError(const QString &errorMessage);
    void showStatusSuccessful(const QVariantMap &result);
    void showStatusError(const QString &errorMessage);
    void showUserSuccessful(const QVariantMap &result);
    void showUserError(const QString &errorMessage);
    void followUserSuccessful(const QVariantMap &result);
    void followUserError(const QString &errorMessage);
    void unfollowUserSuccessful(const QVariantMap &result);
    void unfollowUserError(const QString &errorMessage);
    void searchTweetsSuccessful(const QVariantList &result);
    void searchTweetsError(const QString &errorMessage);
    void favoriteSuccessful(const QVariantMap &result);
    void favoriteError(const QString &errorMessage);
    void unfavoriteSuccessful(const QVariantMap &result);
    void unfavoriteError(const QString &errorMessage);
    void retweetSuccessful(const QVariantMap &result);
    void retweetError(const QString &errorMessage);
    void unretweetSuccessful(const QVariantMap &result);
    void unretweetError(const QString &errorMessage);
    void imageUploadSuccessful(const QString &fileName, const QVariantMap &result);
    void imageUploadError(const QString &fileName, const QString &errorMessage);
    void imageUploadStatus(const QString &fileName, qint64 bytesSent, qint64 bytesTotal);

    void getOpenGraphSuccessful(const QVariantMap &result);
    void getOpenGraphError(const QString &errorMessage);

private:
    O1Requestor *requestor;
    QNetworkAccessManager *manager;

private slots:
    void handleVerifyCredentialsSuccessful();
    void handleVerifyCredentialsError(QNetworkReply::NetworkError error);
    void handleHelpConfigurationSuccessful();
    void handleHelpConfigurationError(QNetworkReply::NetworkError error);
    void handleHelpPrivacySuccessful();
    void handleHelpPrivacyError(QNetworkReply::NetworkError error);
    void handleHelpTosSuccessful();
    void handleHelpTosError(QNetworkReply::NetworkError error);
    void handleTweetError(QNetworkReply::NetworkError error);
    void handleTweetFinished();
    void handleHomeTimelineError(QNetworkReply::NetworkError error);
    void handleHomeTimelineFinished();
    void handleMentionsTimelineError(QNetworkReply::NetworkError error);
    void handleMentionsTimelineFinished();
    void handleUserTimelineError(QNetworkReply::NetworkError error);
    void handleUserTimelineFinished();
    void handleFollowersError(QNetworkReply::NetworkError error);
    void handleFollowersFinished();
    void handleFriendsError(QNetworkReply::NetworkError error);
    void handleFriendsFinished();
    void handleShowStatusError(QNetworkReply::NetworkError error);
    void handleShowStatusFinished();
    void handleShowUserError(QNetworkReply::NetworkError error);
    void handleShowUserFinished();
    void handleFollowUserError(QNetworkReply::NetworkError error);
    void handleFollowUserFinished();
    void handleUnfollowUserError(QNetworkReply::NetworkError error);
    void handleUnfollowUserFinished();
    void handleSearchTweetsError(QNetworkReply::NetworkError error);
    void handleSearchTweetsFinished();
    void handleFavoriteError(QNetworkReply::NetworkError error);
    void handleFavoriteFinished();
    void handleUnfavoriteError(QNetworkReply::NetworkError error);
    void handleUnfavoriteFinished();
    void handleRetweetError(QNetworkReply::NetworkError error);
    void handleRetweetFinished();
    void handleUnretweetError(QNetworkReply::NetworkError error);
    void handleUnretweetFinished();

    void handleGetOpenGraphError(QNetworkReply::NetworkError error);
    void handleGetOpenGraphFinished();

};

#endif // TWITTERAPI_H
