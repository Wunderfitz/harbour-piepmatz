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
#include <QSettings>
#include <QVariantMap>
#include <QVariantList>
#include "o1requestor.h"
#include "o0requestparameter.h"
#include "o0globals.h"
#include "emojisearchworker.h"
//#include "wagnis/wagnis.h"

const char API_ACCOUNT_VERIFY_CREDENTIALS[] = "https://api.twitter.com/1.1/account/verify_credentials.json";
const char API_ACCOUNT_SETTINGS[] = "https://api.twitter.com/1.1/account/settings.json";
const char API_HELP_PRIVACY[] = "https://api.twitter.com/1.1/help/privacy.json";
const char API_HELP_TOS[] = "https://api.twitter.com/1.1/help/tos.json";
const char API_MEDIA_UPLOAD[] = "https://upload.twitter.com/1.1/media/upload.json";
const char API_MEDIA_METADATA_CREATE[] = "https://upload.twitter.com/1.1/media/metadata/create.json";
const char API_STATUSES_UPDATE[] = "https://api.twitter.com/1.1/statuses/update.json";
const char API_STATUSES_HOME_TIMELINE[] = "https://api.twitter.com/1.1/statuses/home_timeline.json";
const char API_STATUSES_MENTIONS_TIMELINE[] = "https://api.twitter.com/1.1/statuses/mentions_timeline.json";
const char API_STATUSES_RETWEET_TIMELINE[] = "https://api.twitter.com/1.1/statuses/retweets_of_me.json";
const char API_FOLLOWERS_LIST[] = "https://api.twitter.com/1.1/followers/list.json";
const char API_FRIENDS_LIST[] = "https://api.twitter.com/1.1/friends/list.json";
const char API_STATUSES_SHOW[] = "https://api.twitter.com/1.1/statuses/show.json";
const char API_STATUSES_USER_TIMELINE[] = "https://api.twitter.com/1.1/statuses/user_timeline.json";
const char API_STATUSES_RETWEET[] = "https://api.twitter.com/1.1/statuses/retweet/:id.json";
const char API_STATUSES_RETWEETS_FOR[] = "https://api.twitter.com/1.1/statuses/retweets/:id.json";
const char API_STATUSES_UNRETWEET[] = "https://api.twitter.com/1.1/statuses/unretweet/:id.json";
const char API_STATUSES_DESTROY[] = "https://api.twitter.com/1.1/statuses/destroy/:id.json";
const char API_USERS_SHOW[] = "https://api.twitter.com/1.1/users/show.json";
const char API_FRIENDSHIPS_CREATE[] = "https://api.twitter.com/1.1/friendships/create.json";
const char API_FRIENDSHIPS_DESTROY[] = "https://api.twitter.com/1.1/friendships/destroy.json";
const char API_SEARCH_TWEETS[] = "https://api.twitter.com/1.1/search/tweets.json";
const char API_SEARCH_USERS[] = "https://api.twitter.com/1.1/users/search.json";
const char API_GEO_SEARCH[] = "https://api.twitter.com/1.1/geo/search.json";
const char API_FAVORITES_LIST[] = "https://api.twitter.com/1.1/favorites/list.json";
const char API_FAVORITES_CREATE[] = "https://api.twitter.com/1.1/favorites/create.json";
const char API_FAVORITES_DESTROY[] = "https://api.twitter.com/1.1/favorites/destroy.json";
const char API_DIRECT_MESSAGES_LIST[] = "https://api.twitter.com/1.1/direct_messages/events/list.json";
const char API_DIRECT_MESSAGES_NEW[] = "https://api.twitter.com/1.1/direct_messages/events/new.json";
const char API_TRENDS_PLACE[] = "https://api.twitter.com/1.1/trends/place.json";
const char API_TRENDS_CLOSEST[] = "https://api.twitter.com/1.1/trends/closest.json";
const char API_TRENDS_AVAILABLE[] = "https://api.twitter.com/1.1/trends/available.json";
const char API_LISTS_LIST[] = "https://api.twitter.com/1.1/lists/list.json";
const char API_LISTS_MEMBERSHIPS[] = "https://api.twitter.com/1.1/lists/memberships.json";
const char API_LISTS_MEMBERS[] = "https://api.twitter.com/1.1/lists/members.json";
const char API_LISTS_STATUSES[] = "https://api.twitter.com/1.1/lists/statuses.json";
const char API_SAVED_SEARCHES_LIST[] = "https://api.twitter.com/1.1/saved_searches/list.json";
const char API_SAVED_SEARCHES_CREATE[] = "https://api.twitter.com/1.1/saved_searches/create.json";
const char API_SAVED_SEARCHES_DESTROY[] = "https://api.twitter.com/1.1/saved_searches/destroy/:id.json";

const char HEADER_NO_RECURSION[] = "X-Piepmatz-No-Recursion";

class TwitterApi : public QObject {

    Q_OBJECT
public:
    //TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, Wagnis *wagnis, QObject* parent = 0);
    TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, O1Requestor* secretIdentityRequestor = 0, QObject *parent = 0);

    Q_INVOKABLE void verifyCredentials();
    Q_INVOKABLE void accountSettings();
    Q_INVOKABLE void helpPrivacy();
    Q_INVOKABLE void helpTos();
    Q_INVOKABLE void tweet(const QString &text, const QString &placeId = QString());
    Q_INVOKABLE void replyToTweet(const QString &text, const QString &replyToStatusId, const QString &placeId = QString());
    Q_INVOKABLE void retweetWithComment(const QString &text, const QString &attachmentUrl, const QString &placeId = QString());
    Q_INVOKABLE void tweetWithImages(const QString &text, const QString &mediaIds, const QString &placeId = QString());
    Q_INVOKABLE void replyToTweetWithImages(const QString &text, const QString &replyToStatusId, const QString &mediaIds, const QString &placeId = QString());
    Q_INVOKABLE void homeTimeline(const QString &maxId = QString());
    Q_INVOKABLE void mentionsTimeline();
    Q_INVOKABLE void retweetTimeline();
    Q_INVOKABLE void userTimeline(const QString &screenName, const bool &useSecretIdentity = false);
    Q_INVOKABLE void followers(const QString &screenName);
    Q_INVOKABLE void friends(const QString &screenName, const QString &cursor = 0);
    Q_INVOKABLE void showStatus(const QString &statusId, const bool &useSecretIdentity = false);
    Q_INVOKABLE void showUser(const QString &screenName);
    Q_INVOKABLE void showUserById(const QString &userId);
    Q_INVOKABLE void followUser(const QString &screenName);
    Q_INVOKABLE void unfollowUser(const QString &screenName);
    Q_INVOKABLE void searchTweets(const QString &query);
    Q_INVOKABLE void searchUsers(const QString &query);
    Q_INVOKABLE void searchGeo(const QString &latitude, const QString &longitude);
    Q_INVOKABLE void favorite(const QString &statusId);
    Q_INVOKABLE void unfavorite(const QString &statusId);
    Q_INVOKABLE void favorites(const QString &screenName);
    Q_INVOKABLE void retweet(const QString &statusId);
    Q_INVOKABLE void retweetsFor(const QString &statusId);
    Q_INVOKABLE void unretweet(const QString &statusId);
    Q_INVOKABLE void destroyTweet(const QString &statusId);
    Q_INVOKABLE void uploadImage(const QString &fileName);
    Q_INVOKABLE void uploadImageDescription(const QString &mediaId, const QString &description);
    Q_INVOKABLE void downloadFile(const QString &address, const QString &fileName);
    Q_INVOKABLE void directMessagesList(const QString &cursor = QString());
    Q_INVOKABLE void directMessagesNew(const QString &text, const QString &recipientId);
    Q_INVOKABLE void trends(const QString &placeId);
    Q_INVOKABLE void placesForTrends(const QString &latitude, const QString &longitude);
    Q_INVOKABLE void userLists();
    Q_INVOKABLE void listsMemberships();
    Q_INVOKABLE void listMembers(const QString &listId);
    Q_INVOKABLE void listTimeline(const QString &listId, const QString &maxId = QString());
    Q_INVOKABLE void savedSearches();
    Q_INVOKABLE void saveSearch(const QString &query);
    Q_INVOKABLE void destroySavedSearch(const QString &id);

    Q_INVOKABLE void getOpenGraph(const QString &address);
    Q_INVOKABLE void getSingleTweet(const QString &tweetId, const QString &address);
    Q_INVOKABLE void getSingleTweetWithConversationId(const QString &tweetId);
    Q_INVOKABLE void getTweetConversation(const QString &conversationId);
    Q_INVOKABLE void getIpInfo();
    Q_INVOKABLE void controlScreenSaver(const bool &enabled);
    Q_INVOKABLE void handleAdditionalInformation(const QString &additionalInformation);    

    Q_INVOKABLE bool getDeveloperMode();
    Q_INVOKABLE void setDeveloperMode(const bool enableDeveloperMode);
    Q_INVOKABLE QString getBearerToken();
    Q_INVOKABLE void setBearerToken(const QString &bearerToken);

    Q_INVOKABLE QVariantMap parseErrorResponse(const QString &errorText, const QByteArray &responseText);

signals:
    void verifyCredentialsSuccessful(const QVariantMap &result);
    void verifyCredentialsError(const QString &errorMessage);
    void accountSettingsSuccessful(const QVariantMap &result);
    void accountSettingsError(const QString &errorMessage);
    void helpPrivacySuccessful(const QVariantMap &result);
    void helpPrivacyError(const QString &errorMessage);
    void helpTosSuccessful(const QVariantMap &result);
    void helpTosError(const QString &errorMessage);
    void tweetSuccessful(const QVariantMap &result);
    void tweetError(const QString &errorMessage);
    void homeTimelineSuccessful(const QVariantList &result, const bool incrementalUpdate);
    void homeTimelineError(const QString &errorMessage);
    void mentionsTimelineSuccessful(const QVariantList &result);
    void mentionsTimelineError(const QString &errorMessage);
    void retweetTimelineSuccessful(const QVariantList &result);
    void retweetTimelineError(const QString &errorMessage);
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
    void showUserByIdSuccessful(const QVariantMap &result);
    void showUserByIdError(const QString &errorMessage);
    void followUserSuccessful(const QVariantMap &result);
    void followUserError(const QString &errorMessage);
    void unfollowUserSuccessful(const QVariantMap &result);
    void unfollowUserError(const QString &errorMessage);
    void searchTweetsSuccessful(const QVariantList &result);
    void searchTweetsError(const QString &errorMessage);
    void searchUsersSuccessful(const QVariantList &result);
    void searchUsersError(const QString &errorMessage);
    void searchGeoSuccessful(const QVariantMap &result);
    void searchGeoError(const QString &errorMessage);
    void favoriteSuccessful(const QVariantMap &result);
    void favoriteError(const QString &errorMessage);
    void unfavoriteSuccessful(const QVariantMap &result);
    void unfavoriteError(const QString &errorMessage);
    void favoritesSuccessful(const QVariantList &result);
    void favoritesError(const QString &errorMessage);
    void retweetSuccessful(const QVariantMap &result);
    void retweetError(const QString &errorMessage);
    void retweetsForSuccessful(const QString &statusId, const QVariantList &result);
    void retweetsForError(const QString &statusId, const QString &errorMessage);
    void unretweetSuccessful(const QVariantMap &result);
    void unretweetError(const QString &errorMessage);
    void destroyError(const QString &errorMessage);
    void destroySuccessful(const QVariantMap &result);
    void imageUploadSuccessful(const QString &fileName, const QVariantMap &result);
    void imageUploadError(const QString &fileName, const QString &errorMessage);
    void imageUploadStatus(const QString &fileName, qint64 bytesSent, qint64 bytesTotal);
    void imageDescriptionUploadSuccessful(const QString &fileName);
    void imageDescriptionUploadError(const QString &fileName, const QString &errorMessage);
    void downloadSuccessful(const QString &fileName, const QString &filePath);
    void downloadError(const QString &fileName, const QString &errorMessage);
    void downloadStatus(const QString &fileName, int percentCompleted);
    void directMessagesListSuccessful(const QVariantMap &result);
    void directMessagesListError(const QString &errorMessage);
    void directMessagesNewSuccessful(const QVariantMap &result);
    void directMessagesNewError(const QString &errorMessage);
    void trendsSuccessful(const QVariantList &result);
    void trendsError(const QString &errorMessage);
    void placesForTrendsSuccessful(const QVariantList &result);
    void placesForTrendsError(const QString &errorMessage);
    void userListsSuccessful(const QVariantList &result);
    void userListsError(const QString &errorMessage);
    void listsMembershipsSuccessful(const QVariantMap &result);
    void listsMembershipsError(const QString &errorMessage);
    void listMembersSuccessful(const QVariantMap &result);
    void listMembersError(const QString &errorMessage);
    void listTimelineSuccessful(const QVariantList &result, const bool incrementalUpdate);
    void listTimelineError(const QString &errorMessage);
    void savedSearchesSuccessful(const QVariantList &result);
    void savedSearchesError(const QString &errorMessage);
    void saveSearchSuccessful(const QVariantMap &result);
    void saveSearchError(const QString &errorMessage);
    void destroySavedSearchSuccessful(const QVariantMap &result);
    void destroySavedSearchError(const QString &errorMessage);

    void getOpenGraphSuccessful(const QVariantMap &result);
    void getOpenGraphError(const QString &errorMessage);
    void tweetConversationReceived(const QString &tweetId, const QVariantList &receivedTweets);
    void getIpInfoSuccessful(const QVariantMap &result);
    void getIpInfoError(const QString &errorMessage);

    void bearerTokenChanged(const QString &bearerToken);
    void developerModeChanged(const bool developerModel);

private:
    O1Requestor *requestor;
    O1Requestor *secretIdentityRequestor;
    QNetworkAccessManager *manager;
    QSettings twitterSettings;
    //Wagnis *wagnis;

private slots:
    void handleVerifyCredentialsSuccessful();
    void handleVerifyCredentialsError(QNetworkReply::NetworkError error);
    void handleAccountSettingsSuccessful();
    void handleAccountSettingsError(QNetworkReply::NetworkError error);
    void handleHelpPrivacySuccessful();
    void handleHelpPrivacyError(QNetworkReply::NetworkError error);
    void handleHelpTosSuccessful();
    void handleHelpTosError(QNetworkReply::NetworkError error);
    void handleTweetError(QNetworkReply::NetworkError error);
    void handleTweetFinished();
    void handleHomeTimelineError(QNetworkReply::NetworkError error);
    void handleHomeTimelineFinished();
    void handleHomeTimelineLoadMoreFinished();
    void handleMentionsTimelineError(QNetworkReply::NetworkError error);
    void handleMentionsTimelineFinished();
    void handleRetweetTimelineError(QNetworkReply::NetworkError error);
    void handleRetweetTimelineFinished();
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
    void handleShowUserByIdError(QNetworkReply::NetworkError error);
    void handleShowUserByIdFinished();
    void handleFollowUserError(QNetworkReply::NetworkError error);
    void handleFollowUserFinished();
    void handleUnfollowUserError(QNetworkReply::NetworkError error);
    void handleUnfollowUserFinished();
    void handleSearchTweetsError(QNetworkReply::NetworkError error);
    void handleSearchTweetsFinished();
    void handleSearchUsersError(QNetworkReply::NetworkError error);
    void handleSearchUsersFinished();
    void handleSearchGeoError(QNetworkReply::NetworkError error);
    void handleSearchGeoFinished();
    void handleFavoriteError(QNetworkReply::NetworkError error);
    void handleFavoriteFinished();
    void handleUnfavoriteError(QNetworkReply::NetworkError error);
    void handleUnfavoriteFinished();
    void handleFavoritesError(QNetworkReply::NetworkError error);
    void handleFavoritesFinished();
    void handleRetweetError(QNetworkReply::NetworkError error);
    void handleRetweetFinished();
    void handleRetweetsForError(QNetworkReply::NetworkError error);
    void handleRetweetsForFinished();
    void handleUnretweetError(QNetworkReply::NetworkError error);
    void handleUnretweetFinished();
    void handleDestroyError(QNetworkReply::NetworkError error);
    void handleDestroyFinished();
    void handleDirectMessagesListError(QNetworkReply::NetworkError error);
    void handleDirectMessagesListFinished();
    void handleDirectMessagesNewError(QNetworkReply::NetworkError error);
    void handleDirectMessagesNewFinished();
    void handleTrendsError(QNetworkReply::NetworkError error);
    void handleTrendsFinished();
    void handlePlacesForTrendsError(QNetworkReply::NetworkError error);
    void handlePlacesForTrendsFinished();
    void handleUserListsError(QNetworkReply::NetworkError error);
    void handleUserListsFinished();
    void handleListsMembershipsError(QNetworkReply::NetworkError error);
    void handleListsMembershipsFinished();
    void handleListMembersError(QNetworkReply::NetworkError error);
    void handleListMembersFinished();
    void handleListTimelineError(QNetworkReply::NetworkError error);
    void handleListTimelineFinished();
    void handleListTimelineLoadMoreFinished();
    void handleSavedSearchesError(QNetworkReply::NetworkError error);
    void handleSavedSearchesFinished();
    void handleSaveSearchError(QNetworkReply::NetworkError error);
    void handleSaveSearchFinished();
    void handleDestroySavedSearchError(QNetworkReply::NetworkError error);
    void handleDestroySavedSearchFinished();

    void handleGetOpenGraphError(QNetworkReply::NetworkError error);
    void handleGetOpenGraphFinished();
    void handleGetSingleTweetError(QNetworkReply::NetworkError error);
    void handleGetSingleTweetFinished();
    void handleGetSingleTweetWithConversationIdFinished();
    void handleGetTweetConversationFinished();
    void handleTweetConversationReceived(QString tweetId, QVariantList receivedTweets);
    void handleGetIpInfoError(QNetworkReply::NetworkError error);
    void handleGetIpInfoFinished();

};

#endif // TWITTERAPI_H
