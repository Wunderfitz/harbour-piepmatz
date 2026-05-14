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
#include "twitterapi.h"

#include "imageresponsehandler.h"
#include "imagemetadataresponsehandler.h"
#include "downloadresponsehandler.h"
#include "tweetconversationhandler.h"
#include "contentextractor.h"
#include "QGumboParser/qgumbodocument.h"
#include "QGumboParser/qgumbonode.h"
#include <QBuffer>
#include <QFile>
#include <QHttpMultiPart>
#include <QXmlStreamReader>
#include <QProcess>
#include <QTextCodec>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

const char SETTINGS_DEVELOPER_MODE[] = "twitterSettings/developerMode";
const char SETTINGS_BEARER_TOKEN[] = "twitterSettings/bearerToken";
const char SETTINGS_MY_USER_ID[] = "twitterSettings/myUserId";

//TwitterApi::TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, Wagnis *wagnis, QObject* parent) : QObject(parent) {
TwitterApi::TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, O1Requestor *secretIdentityRequestor, QObject* parent) : QObject(parent), twitterSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/de.ygriega/piepmatz/twitterSettings.conf", QSettings::NativeFormat) {
    this->requestor = requestor;
    this->manager = manager;
    this->secretIdentityRequestor = secretIdentityRequestor;
    //this->wagnis = wagnis;
}

void TwitterApi::verifyCredentials()
{
    qDebug() << "TwitterApi::verifyCredentials";

//    if (!wagnis->hasFeature("contribution") && wagnis->getRemainingTime() == 0) {
//        emit verifyCredentialsError("You haven't completed the registration process!");
//        return;
//    }

    QUrl url = QUrl(API_ACCOUNT_VERIFY_CREDENTIALS);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleVerifyCredentialsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleVerifyCredentialsSuccessful()));
}

void TwitterApi::accountSettings()
{
    qDebug() << "TwitterApi::accountSettings";
    QUrl url = QUrl(API_ACCOUNT_SETTINGS);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleAccountSettingsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleAccountSettingsSuccessful()));
}

void TwitterApi::helpPrivacy()
{
    qDebug() << "TwitterApi::helpPrivacy";
    QUrl url = QUrl(API_HELP_PRIVACY);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHelpPrivacyError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleHelpPrivacySuccessful()));
}

void TwitterApi::helpTos()
{
    qDebug() << "TwitterApi::helpTos";
    QUrl url = QUrl(API_HELP_TOS);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHelpTosError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleHelpTosSuccessful()));
}

void TwitterApi::handleVerifyCredentialsSuccessful()
{
    qDebug() << "TwitterApi::handleVerifyCredentialsSuccessful";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        emit verifyCredentialsSuccessful(jsonDocument.object().toVariantMap());
    } else {
        emit verifyCredentialsError("Piepmatz couldn't understand Twitter's response! (Verify Credentials)");
    }
}

void TwitterApi::handleVerifyCredentialsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleVerifyCredentialsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit verifyCredentialsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleAccountSettingsSuccessful()
{
    qDebug() << "TwitterApi::handleAccountSettingsSuccessful";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        emit accountSettingsSuccessful(jsonDocument.object().toVariantMap());
    } else {
        emit accountSettingsError("Piepmatz couldn't understand Twitter's response! (Account Settings)");
    }
}

void TwitterApi::handleAccountSettingsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleAccountSettingsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit accountSettingsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleHelpPrivacySuccessful()
{
    qDebug() << "TwitterApi::handleHelpPrivacySuccessful";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        emit helpPrivacySuccessful(jsonDocument.object().toVariantMap());
    } else {
        emit helpPrivacyError("Piepmatz couldn't understand Twitter's response! (Help Privacy)");
    }
}

void TwitterApi::handleHelpPrivacyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHelpPrivacyError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit helpPrivacyError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleHelpTosSuccessful()
{
    qDebug() << "TwitterApi::handleHelpTosSuccessful";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        emit helpTosSuccessful(jsonDocument.object().toVariantMap());
    } else {
        emit helpTosError("Piepmatz couldn't understand Twitter's response! (Help TOS)");
    }
}

void TwitterApi::handleHelpTosError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHelpTosError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit helpTosError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::tweet(const QString &text, const QString &placeId)
{
    qDebug() << "TwitterApi::tweet" << placeId;
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("status"), text.toUtf8()));
    if (!placeId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("place_id"), placeId.toUtf8()));
    }
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::replyToTweet(const QString &text, const QString &replyToStatusId, const QString &placeId)
{
    qDebug() << "TwitterApi::replyToTweet" << replyToStatusId << placeId;
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("status"), text.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("in_reply_to_status_id"), replyToStatusId.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("auto_populate_reply_metadata"), QByteArray("true")));
    if (!placeId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("place_id"), placeId.toUtf8()));
    }
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::retweetWithComment(const QString &text, const QString &attachmentUrl, const QString &placeId)
{
    qDebug() << "TwitterApi::retweetWithComment" << attachmentUrl << placeId;
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("status"), text.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("attachment_url"), attachmentUrl.toUtf8()));
    if (!placeId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("place_id"), placeId.toUtf8()));
    }
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::tweetWithImages(const QString &text, const QString &mediaIds, const QString &placeId)
{
    qDebug() << "TwitterApi::tweetWithImages" << placeId;
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("status"), text.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("media_ids"), mediaIds.toUtf8()));
    if (!placeId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("place_id"), placeId.toUtf8()));
    }
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::replyToTweetWithImages(const QString &text, const QString &replyToStatusId, const QString &mediaIds, const QString &placeId)
{
    qDebug() << "TwitterApi::replyToTweetWithImages" << replyToStatusId << mediaIds << placeId;
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("status"), text.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("in_reply_to_status_id"), replyToStatusId.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("auto_populate_reply_metadata"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("media_ids"), mediaIds.toUtf8()));
    if (!placeId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("place_id"), placeId.toUtf8()));
    }
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::homeTimeline(const QString &paginationToken)
{
    qDebug() << "TwitterApi::homeTimeline paginationToken:" << paginationToken;

    QString myUserId = getMyUserId();
    if (myUserId.isEmpty()) {
        qWarning() << "TwitterApi::homeTimeline: user ID not set, cannot build v2 URL";
        emit homeTimelineError("User ID not available for timeline request. Please verify credentials first.");
        return;
    }

    QUrl url = QUrl(QString(API_V2_HOME_TIMELINE_BASE) + myUserId + "/timelines/reverse_chronological");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("tweet.fields", "id,text,created_at,author_id,entities,referenced_tweets,attachments,public_metrics,in_reply_to_user_id,note_tweet");
    urlQuery.addQueryItem("expansions", "author_id,referenced_tweets.id,referenced_tweets.id.author_id,attachments.media_keys");
    urlQuery.addQueryItem("user.fields", "id,name,username,profile_image_url,verified,protected,description,public_metrics");
    urlQuery.addQueryItem("media.fields", "media_key,type,url,preview_image_url,alt_text,width,height,variants");
    urlQuery.addQueryItem("max_results", "10");
    if (!paginationToken.isEmpty()) {
        urlQuery.addQueryItem("pagination_token", paginationToken);
    }
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters;
    requestParameters.append(O0RequestParameter(QByteArray("tweet.fields"), QByteArray("id,text,created_at,author_id,entities,referenced_tweets,attachments,public_metrics,in_reply_to_user_id,note_tweet")));
    requestParameters.append(O0RequestParameter(QByteArray("expansions"), QByteArray("author_id,referenced_tweets.id,referenced_tweets.id.author_id,attachments.media_keys")));
    requestParameters.append(O0RequestParameter(QByteArray("user.fields"), QByteArray("id,name,username,profile_image_url,verified,protected,description,public_metrics")));
    requestParameters.append(O0RequestParameter(QByteArray("media.fields"), QByteArray("media_key,type,url,preview_image_url,alt_text,width,height,variants")));
    requestParameters.append(O0RequestParameter(QByteArray("max_results"), QByteArray("10")));
    if (!paginationToken.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("pagination_token"), paginationToken.toUtf8()));
    }
    QNetworkReply *reply = requestor->get(request, requestParameters);

    if (paginationToken.isEmpty()) {
        connect(reply, SIGNAL(finished()), this, SLOT(handleHomeTimelineFinished()));
    } else {
        connect(reply, SIGNAL(finished()), this, SLOT(handleHomeTimelineLoadMoreFinished()));
    }
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHomeTimelineError(QNetworkReply::NetworkError)));
}

void TwitterApi::mentionsTimeline()
{
    qDebug() << "TwitterApi::mentionsTimeline";
    QUrl url = QUrl(API_STATUSES_MENTIONS_TIMELINE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleMentionsTimelineError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleMentionsTimelineFinished()));
}

void TwitterApi::retweetTimeline()
{
    qDebug() << "TwitterApi::retweetTimeline";
    QUrl url = QUrl(API_STATUSES_RETWEET_TIMELINE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("trim_user", "false");
    urlQuery.addQueryItem("count", "10");
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("trim_user"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("10")));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRetweetTimelineError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRetweetTimelineFinished()));
}

void TwitterApi::showStatus(const QString &statusId, const bool &useSecretIdentity)
{
    // Very weird, some statusIds contain a query string. Why?
    QString sanitizedStatus = statusId;
    int qm = statusId.indexOf(QLatin1Char('?'));
    if (qm >= 0) {
        sanitizedStatus = statusId.left(qm);
    }
    qDebug() << "TwitterApi::showStatus" << sanitizedStatus;

    QUrl url = QUrl(QString(API_V2_TWEETS_BASE) + sanitizedStatus);
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("tweet.fields", "id,text,created_at,author_id,entities,referenced_tweets,attachments,public_metrics,in_reply_to_user_id,note_tweet");
    urlQuery.addQueryItem("expansions", "author_id,referenced_tweets.id,referenced_tweets.id.author_id,attachments.media_keys");
    urlQuery.addQueryItem("user.fields", "id,name,username,profile_image_url,verified,protected,description,public_metrics");
    urlQuery.addQueryItem("media.fields", "media_key,type,url,preview_image_url,alt_text,width,height,variants");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters;
    requestParameters.append(O0RequestParameter(QByteArray("tweet.fields"), QByteArray("id,text,created_at,author_id,entities,referenced_tweets,attachments,public_metrics,in_reply_to_user_id,note_tweet")));
    requestParameters.append(O0RequestParameter(QByteArray("expansions"), QByteArray("author_id,referenced_tweets.id,referenced_tweets.id.author_id,attachments.media_keys")));
    requestParameters.append(O0RequestParameter(QByteArray("user.fields"), QByteArray("id,name,username,profile_image_url,verified,protected,description,public_metrics")));
    requestParameters.append(O0RequestParameter(QByteArray("media.fields"), QByteArray("media_key,type,url,preview_image_url,alt_text,width,height,variants")));

    QNetworkReply *reply;
    if (useSecretIdentity && secretIdentityRequestor != nullptr) {
        request.setRawHeader(HEADER_NO_RECURSION, "X");
        reply = secretIdentityRequestor->get(request, requestParameters);
    } else {
        reply = requestor->get(request, requestParameters);
    }

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleShowStatusError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleShowStatusFinished()));
}

void TwitterApi::showUser(const QString &screenName)
{
    qDebug() << "TwitterApi::showUser" << screenName;
    QUrl url = QUrl(API_USERS_SHOW);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("screen_name", screenName);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleShowUserError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleShowUserFinished()));
}

void TwitterApi::showUserById(const QString &userId)
{
    qDebug() << "TwitterApi::showUserById" << userId;
    QUrl url = QUrl(API_USERS_SHOW);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("user_id", userId);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("user_id"), userId.toUtf8()));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleShowUserByIdError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleShowUserByIdFinished()));
}

void TwitterApi::userTimeline(const QString &screenName, const bool &useSecretIdentity)
{
    qDebug() << "TwitterApi::userTimeline" << screenName;
    QUrl url = QUrl(API_STATUSES_USER_TIMELINE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("include_rts", "true");
    urlQuery.addQueryItem("exclude_replies", "false");
    urlQuery.addQueryItem("screen_name", screenName);
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("include_rts"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("exclude_replies"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));

    QNetworkReply *reply;
    if (useSecretIdentity && secretIdentityRequestor != nullptr) {
        request.setRawHeader(HEADER_NO_RECURSION, "X");
        reply = secretIdentityRequestor->get(request, requestParameters);
    } else {
        reply = requestor->get(request, requestParameters);
    }

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleUserTimelineError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleUserTimelineFinished()));
}

void TwitterApi::followers(const QString &screenName)
{
    qDebug() << "TwitterApi::followers" << screenName;
    QUrl url = QUrl(API_FOLLOWERS_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("screen_name", screenName);
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("skip_status", "true");
    urlQuery.addQueryItem("include_user_entities", "true");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("skip_status"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("include_user_entities"), QByteArray("true")));

    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleFollowersError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleFollowersFinished()));
}

void TwitterApi::friends(const QString &screenName, const QString &cursor)
{
    qDebug() << "TwitterApi::friends" << screenName;
    QUrl url = QUrl(API_FRIENDS_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("screen_name", screenName);
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("skip_status", "true");
    urlQuery.addQueryItem("include_user_entities", "true");
    if (cursor != "") {
        urlQuery.addQueryItem("cursor", cursor);
    }

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("skip_status"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("include_user_entities"), QByteArray("true")));
    if (cursor != "") {
        requestParameters.append(O0RequestParameter(QByteArray("cursor"), cursor.toUtf8()));
    }

    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleFriendsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleFriendsFinished()));
}

void TwitterApi::followUser(const QString &screenName)
{
    qDebug() << "TwitterApi::followUser" << screenName;
    QUrl url = QUrl(API_FRIENDSHIPS_CREATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleFollowUserError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleFollowUserFinished()));
}

void TwitterApi::unfollowUser(const QString &screenName)
{
    qDebug() << "TwitterApi::unfollowUser" << screenName;
    QUrl url = QUrl(API_FRIENDSHIPS_DESTROY);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleUnfollowUserError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleUnfollowUserFinished()));
}

void TwitterApi::searchTweets(const QString &query)
{
    if (query.isEmpty()) {
        emit searchTweetsSuccessful(QVariantList());
        return;
    }

    QString searchString = QString(QUrl::toPercentEncoding(query));

    qDebug() << "TwitterApi::searchTweets" << searchString;
    QUrl url = QUrl(API_SEARCH_TWEETS);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("q", searchString);
    urlQuery.addQueryItem("count", "100");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("q"), query.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("100")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSearchTweetsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSearchTweetsFinished()));
}

void TwitterApi::searchUsers(const QString &query)
{
    if (query.isEmpty()) {
        emit searchUsersSuccessful(QVariantList());
        return;
    }

    QString searchString = QString(QUrl::toPercentEncoding(query));

    qDebug() << "TwitterApi::searchUsers" << searchString;
    QUrl url = QUrl(API_SEARCH_USERS);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("q", searchString);
    urlQuery.addQueryItem("count", "20");
    urlQuery.addQueryItem("include_entities", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("q"), query.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("20")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSearchUsersError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSearchUsersFinished()));
}

void TwitterApi::searchGeo(const QString &latitude, const QString &longitude)
{
    qDebug() << "TwitterApi::searchGeo" << latitude << longitude;
    QUrl url = QUrl(API_GEO_SEARCH);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("lat", latitude);
    urlQuery.addQueryItem("long", longitude);
    urlQuery.addQueryItem("max_results", "1");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("lat"), latitude.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("long"), longitude.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("max_results"), QByteArray("1")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSearchGeoError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSearchGeoFinished()));
}

void TwitterApi::favorite(const QString &statusId)
{
    qDebug() << "TwitterApi::favorite" << statusId;
    QUrl url = QUrl(API_FAVORITES_CREATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("id"), statusId.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleFavoriteError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleFavoriteFinished()));
}

void TwitterApi::unfavorite(const QString &statusId)
{
    qDebug() << "TwitterApi::unfavorite" << statusId;
    QUrl url = QUrl(API_FAVORITES_DESTROY);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("id"), statusId.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleUnfavoriteError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleUnfavoriteFinished()));
}

void TwitterApi::favorites(const QString &screenName)
{
    qDebug() << "TwitterApi::favorites" << screenName;
    QUrl url = QUrl(API_FAVORITES_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("screen_name", screenName);
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("screen_name"), screenName.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleFavoritesError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleFavoritesFinished()));
}

void TwitterApi::retweet(const QString &statusId)
{
    qDebug() << "TwitterApi::retweet" << statusId;
    QUrl url = QUrl(QString(API_STATUSES_RETWEET).replace(":id", statusId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRetweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRetweetFinished()));
}

void TwitterApi::retweetsFor(const QString &statusId)
{
    qDebug() << "TwitterApi::retweetUsers" << statusId;
    QUrl url = QUrl(QString(API_STATUSES_RETWEETS_FOR).replace(":id", statusId));
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("count", "21");
    urlQuery.addQueryItem("trim_user", "false");

    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("21")));
    requestParameters.append(O0RequestParameter(QByteArray("trim_user"), QByteArray("false")));

    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRetweetsForError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRetweetsForFinished()));
}

void TwitterApi::unretweet(const QString &statusId)
{
    qDebug() << "TwitterApi::unretweet" << statusId;
    QUrl url = QUrl(QString(API_STATUSES_UNRETWEET).replace(":id", statusId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleUnretweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleUnretweetFinished()));
}

void TwitterApi::destroyTweet(const QString &statusId)
{
    qDebug() << "TwitterApi::destroy" << statusId;
    QUrl url = QUrl(QString(API_STATUSES_DESTROY).replace(":id", statusId));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDestroyError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDestroyFinished()));
}

void TwitterApi::uploadImage(const QString &fileName)
{
    qDebug() << "TwitterApi::uploadImage" << fileName;
    QUrl url = QUrl(QString(API_MEDIA_UPLOAD));
    QNetworkRequest request(url);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"media\""));

    QFile *file = new QFile(fileName);
    file->open(QIODevice::ReadOnly);
    QByteArray rawImage = file->readAll();
    imagePart.setBody(rawImage);
    file->setParent(multiPart);

    multiPart->append(imagePart);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();

    QNetworkReply *reply = requestor->post(request, requestParameters, multiPart);
    multiPart->setParent(reply);
    reply->setObjectName(fileName);

    ImageResponseHandler *imageResponseHandler = new ImageResponseHandler(fileName, this);
    imageResponseHandler->setParent(reply);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), imageResponseHandler, SLOT(handleImageUploadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), imageResponseHandler, SLOT(handleImageUploadFinished()));
    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), imageResponseHandler, SLOT(handleImageUploadProgress(qint64,qint64)));
}

void TwitterApi::uploadImageDescription(const QString &mediaId, const QString &description)
{
    qDebug() << "TwitterApi::uploadImageDescription" << mediaId << description;
    QUrl url = QUrl(API_MEDIA_METADATA_CREATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_JSON);
    request.setRawHeader(QByteArray("charset"), QByteArray("UTF-8"));

    QJsonObject alternativeTextObject;
    alternativeTextObject.insert("text", description);
    QJsonObject metadataObject;
    metadataObject.insert("alt_text", alternativeTextObject);
    metadataObject.insert("media_id", mediaId);

    QJsonDocument requestDocument(metadataObject);
    QByteArray jsonAsByteArray = requestDocument.toJson();
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonAsByteArray.size()));

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->post(request, requestParameters, jsonAsByteArray);

    ImageMetadataResponseHandler *imageMetadataResponseHandler = new ImageMetadataResponseHandler(mediaId, this);
    imageMetadataResponseHandler->setParent(reply);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), imageMetadataResponseHandler, SLOT(handleImageMetadataUploadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), imageMetadataResponseHandler, SLOT(handleImageMetadataUploadFinished()));

}

void TwitterApi::downloadFile(const QString &address, const QString &fileName)
{
    qDebug() << "TwitterApi::downloadFile" << address << fileName;
    QUrl url = QUrl(address);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    DownloadResponseHandler *downloadResponseHandler = new DownloadResponseHandler(fileName, this);
    downloadResponseHandler->setParent(reply);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), downloadResponseHandler, SLOT(handleDownloadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), downloadResponseHandler, SLOT(handleDownloadFinished()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), downloadResponseHandler, SLOT(handleDownloadProgress(qint64,qint64)));
}

void TwitterApi::directMessagesList(const QString &cursor)
{
    qDebug() << "TwitterApi::directMessagesList" << cursor;
    QUrl url = QUrl(API_DIRECT_MESSAGES_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("count", "50");
    if (!cursor.isEmpty()) {
        urlQuery.addQueryItem("cursor", cursor);
    }
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("50")));
    if (!cursor.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("cursor"), cursor.toUtf8()));
    }
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDirectMessagesListError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDirectMessagesListFinished()));
}

void TwitterApi::directMessagesNew(const QString &text, const QString &recipientId)
{
    qDebug() << "TwitterApi::directMessagesNew" << recipientId;
    QUrl url = QUrl(API_DIRECT_MESSAGES_NEW);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_JSON);

    QJsonObject messageTargetObject;
    messageTargetObject.insert("recipient_id", recipientId);
    QJsonObject messageDataObject;
    messageDataObject.insert("text", text);
    QJsonObject messageCreateObject;
    messageCreateObject.insert("target", messageTargetObject);
    messageCreateObject.insert("message_data", messageDataObject);

    QJsonObject eventObject;
    eventObject.insert("type", QString("message_create"));
    eventObject.insert("message_create", messageCreateObject);

    QJsonObject requestObject;
    requestObject.insert("event", eventObject);

    QJsonDocument requestDocument(requestObject);
    QByteArray jsonAsByteArray = requestDocument.toJson();
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonAsByteArray.size()));

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->post(request, requestParameters, jsonAsByteArray);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDirectMessagesNewError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDirectMessagesNewFinished()));
}

void TwitterApi::trends(const QString &placeId)
{
    qDebug() << "TwitterApi::trends" << placeId;
    QUrl url = QUrl(API_TRENDS_PLACE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("id", placeId);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("id"), placeId.toUtf8()));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTrendsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTrendsFinished()));
}

void TwitterApi::placesForTrends(const QString &latitude, const QString &longitude)
{
    qDebug() << "TwitterApi::placesForTrends" << latitude << longitude;
    QUrl url = QUrl(API_TRENDS_CLOSEST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("lat", latitude);
    urlQuery.addQueryItem("long", longitude);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("lat"), latitude.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("long"), longitude.toUtf8()));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handlePlacesForTrendsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handlePlacesForTrendsFinished()));
}

void TwitterApi::userLists()
{
    qDebug() << "TwitterApi::userLists";
    QUrl url = QUrl(API_LISTS_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("reverse", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("reverse"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleUserListsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleUserListsFinished()));
}

void TwitterApi::listsMemberships()
{
    qDebug() << "TwitterApi::listsMemberships";
    QUrl url = QUrl(API_LISTS_MEMBERSHIPS);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("count", "100");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("100")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleListsMembershipsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleListsMembershipsFinished()));
}

void TwitterApi::listMembers(const QString &listId)
{
    qDebug() << "TwitterApi::listsMembers" << listId;
    QUrl url = QUrl(API_LISTS_MEMBERS);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("list_id", listId);
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("skip_status", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("list_id"), listId.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("skip_status"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleListMembersError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleListMembersFinished()));
}

void TwitterApi::listTimeline(const QString &listId, const QString &maxId)
{
    qDebug() << "TwitterApi::listTimeline" << listId << maxId;
    QUrl url = QUrl(API_LISTS_STATUSES);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("list_id", listId);
    if (!maxId.isEmpty()) {
        urlQuery.addQueryItem("max_id", maxId);
    }
    urlQuery.addQueryItem("count", "200");
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("list_id"), listId.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    if (!maxId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("max_id"), maxId.toUtf8()));
    }
    QNetworkReply *reply = requestor->get(request, requestParameters);

    if (maxId.isEmpty()) {
        connect(reply, SIGNAL(finished()), this, SLOT(handleListTimelineFinished()));
    } else {
        connect(reply, SIGNAL(finished()), this, SLOT(handleListTimelineLoadMoreFinished()));
    }
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleListTimelineError(QNetworkReply::NetworkError)));

}

void TwitterApi::savedSearches()
{
    qDebug() << "TwitterApi::savedSearches";
    QUrl url = QUrl(API_SAVED_SEARCHES_LIST);
    QUrlQuery urlQuery = QUrlQuery();
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSavedSearchesError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSavedSearchesFinished()));
}

void TwitterApi::saveSearch(const QString &query)
{
    qDebug() << "TwitterApi::saveSearch" << query;
    QUrl url = QUrl(QString(API_SAVED_SEARCHES_CREATE));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("query"), query.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleSaveSearchError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleSaveSearchFinished()));
}

void TwitterApi::destroySavedSearch(const QString &id)
{
    qDebug() << "TwitterApi::destroySavedSearch" << id;
    QUrl url = QUrl(QString(API_SAVED_SEARCHES_DESTROY).replace(":id", id));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleDestroySavedSearchError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleDestroySavedSearchFinished()));
}

void TwitterApi::getOpenGraph(const QString &address)
{
    qDebug() << "TwitterApi::getOpenGraph" << address;
    if (address.startsWith("https://meet.jit.cloud/")) {
        // Other party closes connection before establishing it properly - causes segmentation fault that can't be circumvented (why?)
        // Error: Client network socket disconnected before secure TLS connection was established
        // See https://github.com/Wunderfitz/harbour-piepmatz/issues/105
        qDebug() << "Issues with this URL, not invoking Open Graph query";
        return;
    }
    QUrl url = QUrl(address);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Piepmatz Bot (Sailfish OS)");
    request.setRawHeader(QByteArray("Accept"), QByteArray("text/html,application/xhtml+xml"));
    request.setRawHeader(QByteArray("Accept-Charset"), QByteArray("utf-8"));
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));
    request.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetOpenGraphError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetOpenGraphFinished()));
}

void TwitterApi::getSingleTweet(const QString &tweetId, const QString &address)
{
    qDebug() << "TwitterApi::getSingleTweet" << tweetId << address;

    QUrl url = QUrl(address);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Piepmatz Bot (Sailfish OS)");
    request.setRawHeader(QByteArray("Accept-Charset"), QByteArray("utf-8"));
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));
    request.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetSingleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetSingleTweetFinished()));
}

void TwitterApi::getSingleTweetWithConversationId(const QString &tweetId)
{
    qDebug() << "TwitterApi::getSingleTweetWithConversationId" << tweetId;

    QString bearerToken = this->getBearerToken();
    if (bearerToken.isEmpty()) {
        qDebug() << "TwitterApi::getSingleTweet: No OAuth2 Bearer token, aborting!";
        return;
    }

    QUrl url = QUrl("https://api.twitter.com/2/tweets/" + tweetId + "?tweet.fields=conversation_id");
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + bearerToken.toUtf8()));

    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(handleGetSingleTweetWithConversationIdFinished()));
}

void TwitterApi::getTweetConversation(const QString &conversationId)
{
    qDebug() << "TwitterApi::getTweetConversation" << conversationId;

    QString bearerToken = this->getBearerToken();
    if (bearerToken.isEmpty()) {
        qDebug() << "TwitterApi::getTweetConversation: No OAuth2 Bearer token, aborting!";
        return;
    }

    QUrl url = QUrl("https://api.twitter.com/2/tweets/search/recent?query=conversation_id:" + conversationId + "&max_results=50&tweet.fields=created_at,conversation_id,author_id&expansions=referenced_tweets.id");
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("Authorization"), QByteArray("Bearer " + bearerToken.toUtf8()));

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetTweetConversationFinished()));

}

void TwitterApi::getIpInfo()
{
    qDebug() << "TwitterApi::getIpInfo";
    QUrl url = QUrl("https://ipinfo.io/json");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetIpInfoError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetIpInfoFinished()));
}

void TwitterApi::controlScreenSaver(const bool &enabled)
{
    qDebug() << "TwitterApi::controlScreenSaver";
    QDBusConnection dbusConnection = QDBusConnection::connectToBus(QDBusConnection::SystemBus, "system");
    QDBusInterface dbusInterface("com.nokia.mce", "/com/nokia/mce/request", "com.nokia.mce.request", dbusConnection);

    if (enabled) {
        qDebug() << "Enabling screensaver";
        dbusInterface.call("req_display_cancel_blanking_pause");
    } else {
        qDebug() << "Disabling screensaver";
        dbusInterface.call("req_display_blanking_pause");
    }

}

void TwitterApi::handleAdditionalInformation(const QString &additionalInformation)
{
    qDebug() << "TwitterApi::handleAdditionalInformation" << additionalInformation;
    // For now only used to open downloaded files...
    QStringList argumentsList;
    argumentsList.append(additionalInformation);
    bool successfullyStarted = QProcess::startDetached("xdg-open", argumentsList);
    if (successfullyStarted) {
        qDebug() << "Successfully opened file " << additionalInformation;
    } else {
        qDebug() << "Error opening file " << additionalInformation;
    }
}

bool TwitterApi::getDeveloperMode()
{
    return twitterSettings.value(SETTINGS_DEVELOPER_MODE, false).toBool();
}

void TwitterApi::setDeveloperMode(const bool enableDeveloperMode)
{
    qDebug() << "TwitterApi::setDeveloperMode" << enableDeveloperMode;
    twitterSettings.setValue(SETTINGS_DEVELOPER_MODE, enableDeveloperMode);
    emit developerModeChanged(enableDeveloperMode);
}

QString TwitterApi::getBearerToken()
{
    return twitterSettings.value(SETTINGS_BEARER_TOKEN, "").toString();
}

void TwitterApi::setBearerToken(const QString &bearerToken)
{
    qDebug() << "TwitterApi::setBearerToken" << bearerToken;
    twitterSettings.setValue(SETTINGS_BEARER_TOKEN, bearerToken);
    emit bearerTokenChanged(bearerToken);
}

QString TwitterApi::getMyUserId()
{
    return twitterSettings.value(SETTINGS_MY_USER_ID, "").toString();
}

void TwitterApi::setMyUserId(const QString &userId)
{
    qDebug() << "TwitterApi::setMyUserId" << userId;
    twitterSettings.setValue(SETTINGS_MY_USER_ID, userId);
}

QVariantList TwitterApi::normalizeV2TimelineResponse(const QJsonObject &responseObj, QString &nextToken)
{
    QVariantList result;

    QJsonObject includes = responseObj.value("includes").toObject();

    QVariantMap usersById;
    for (const QJsonValue &val : includes.value("users").toArray()) {
        QJsonObject user = val.toObject();
        QString userId = user.value("id").toString();
        usersById.insert(userId, normalizeUserV2(user));
    }

    QVariantMap mediaByKey;
    for (const QJsonValue &val : includes.value("media").toArray()) {
        QJsonObject media = val.toObject();
        mediaByKey.insert(media.value("media_key").toString(), media.toVariantMap());
    }

    QVariantMap tweetsById;
    for (const QJsonValue &val : includes.value("tweets").toArray()) {
        QJsonObject tweet = val.toObject();
        tweetsById.insert(tweet.value("id").toString(), tweet.toVariantMap());
    }

    nextToken = responseObj.value("meta").toObject().value("next_token").toString();

    for (const QJsonValue &val : responseObj.value("data").toArray()) {
        QVariantMap v1Tweet = normalizeTweetV2(val.toObject(), usersById, mediaByKey, tweetsById);
        if (!v1Tweet.isEmpty()) {
            result.append(v1Tweet);
        }
    }

    return result;
}

QVariantMap TwitterApi::normalizeUserV2(const QJsonObject &user)
{
    QVariantMap v1User;
    QString userId = user.value("id").toString();
    v1User.insert("id_str", userId);
    v1User.insert("id", userId.toLongLong());
    v1User.insert("name", user.value("name").toString());
    v1User.insert("screen_name", user.value("username").toString());
    QString profileImageUrl = user.value("profile_image_url").toString();
    v1User.insert("profile_image_url_https", profileImageUrl);
    v1User.insert("profile_image_url", profileImageUrl);
    v1User.insert("verified", user.value("verified").toBool());
    v1User.insert("protected", user.value("protected").toBool());
    v1User.insert("description", user.value("description").toString());
    if (user.contains("public_metrics")) {
        QJsonObject metrics = user.value("public_metrics").toObject();
        v1User.insert("followers_count", metrics.value("followers_count").toInt());
        v1User.insert("friends_count", metrics.value("following_count").toInt());
        v1User.insert("statuses_count", metrics.value("tweet_count").toInt());
    }
    return v1User;
}

QVariantMap TwitterApi::normalizeEntitiesV2(const QJsonObject &v2Entities, const QVariantMap &usersById)
{
    QVariantMap v1Entities;

    QVariantList hashtags;
    for (const QJsonValue &hv : v2Entities.value("hashtags").toArray()) {
        QJsonObject h = hv.toObject();
        QVariantMap ht;
        ht.insert("text", h.value("tag").toString());
        ht.insert("indices", QVariantList() << h.value("start").toInt() << h.value("end").toInt());
        hashtags.append(ht);
    }
    v1Entities.insert("hashtags", hashtags);

    QVariantList mentions;
    for (const QJsonValue &mv : v2Entities.value("mentions").toArray()) {
        QJsonObject m = mv.toObject();
        QVariantMap mention;
        mention.insert("screen_name", m.value("username").toString());
        QString mentionId = m.value("id").toString();
        mention.insert("id_str", mentionId);
        mention.insert("id", mentionId.toLongLong());
        if (usersById.contains(mentionId)) {
            mention.insert("name", usersById.value(mentionId).toMap().value("name"));
        }
        mention.insert("indices", QVariantList() << m.value("start").toInt() << m.value("end").toInt());
        mentions.append(mention);
    }
    v1Entities.insert("user_mentions", mentions);

    QVariantList urls;
    for (const QJsonValue &uv : v2Entities.value("urls").toArray()) {
        QJsonObject u = uv.toObject();
        QVariantMap url;
        url.insert("url", u.value("url").toString());
        url.insert("expanded_url", u.value("expanded_url").toString());
        url.insert("display_url", u.value("display_url").toString());
        url.insert("indices", QVariantList() << u.value("start").toInt() << u.value("end").toInt());
        urls.append(url);
    }
    v1Entities.insert("urls", urls);

    v1Entities.insert("symbols", QVariantList());

    return v1Entities;
}

QVariantMap TwitterApi::normalizeMediaV2(const QJsonObject &media)
{
    QVariantMap v1Media;
    QString mediaType = media.value("type").toString();
    v1Media.insert("type", mediaType);
    v1Media.insert("id_str", media.value("media_key").toString());

    if (mediaType == "photo") {
        v1Media.insert("media_url_https", media.value("url").toString());
        int width = media.value("width").toInt();
        int height = media.value("height").toInt();
        if (width > 0 && height > 0) {
            QVariantMap largeSize;
            largeSize.insert("w", width);
            largeSize.insert("h", height);
            largeSize.insert("resize", "fit");
            QVariantMap sizes;
            sizes.insert("large", largeSize);
            v1Media.insert("sizes", sizes);
        }
    } else {
        v1Media.insert("media_url_https", media.value("preview_image_url").toString());
        QVariantMap videoInfo;
        int width = media.value("width").toInt();
        int height = media.value("height").toInt();
        if (width > 0 && height > 0) {
            videoInfo.insert("aspect_ratio", QVariantList() << width << height);
        }
        QVariantList variants;
        for (const QVariant &vv : media.value("variants").toArray().toVariantList()) {
            QVariantMap variant = vv.toMap();
            QVariantMap v1Variant;
            v1Variant.insert("content_type", variant.value("content_type"));
            v1Variant.insert("url", variant.value("url"));
            if (variant.contains("bit_rate")) {
                v1Variant.insert("bitrate", variant.value("bit_rate"));
            }
            variants.append(v1Variant);
        }
        videoInfo.insert("variants", variants);
        v1Media.insert("video_info", videoInfo);
    }

    if (media.contains("alt_text")) {
        v1Media.insert("ext_alt_text", media.value("alt_text").toString());
    }
    v1Media.insert("url", "");
    v1Media.insert("indices", QVariantList() << 0 << 0);

    return v1Media;
}

QVariantMap TwitterApi::normalizeTweetV2(const QJsonObject &tweet, const QVariantMap &usersById, const QVariantMap &mediaByKey, const QVariantMap &tweetsById)
{
    QVariantMap v1Tweet;

    QString tweetId = tweet.value("id").toString();
    v1Tweet.insert("id_str", tweetId);
    v1Tweet.insert("id", tweetId.toLongLong());
    v1Tweet.insert("full_text", tweet.value("text").toString());
    v1Tweet.insert("text", tweet.value("text").toString());

    // Pass ISO 8601 date through as-is; getValidDate() in QML handles both formats
    v1Tweet.insert("created_at", tweet.value("created_at").toString());

    QString authorId = tweet.value("author_id").toString();
    if (usersById.contains(authorId)) {
        v1Tweet.insert("user", usersById.value(authorId));
    }

    if (tweet.contains("public_metrics")) {
        QJsonObject metrics = tweet.value("public_metrics").toObject();
        v1Tweet.insert("favorite_count", metrics.value("like_count").toInt());
        v1Tweet.insert("retweet_count", metrics.value("retweet_count").toInt());
        v1Tweet.insert("reply_count", metrics.value("reply_count").toInt());
        v1Tweet.insert("quote_count", metrics.value("quote_count").toInt());
    }

    // Per-user interaction state is not provided by the v2 timeline
    v1Tweet.insert("favorited", false);
    v1Tweet.insert("retweeted", false);

    if (tweet.contains("in_reply_to_user_id")) {
        QString replyUserId = tweet.value("in_reply_to_user_id").toString();
        v1Tweet.insert("in_reply_to_user_id_str", replyUserId);
        v1Tweet.insert("in_reply_to_user_id", replyUserId.toLongLong());
    }

    if (tweet.contains("referenced_tweets")) {
        for (const QJsonValue &refVal : tweet.value("referenced_tweets").toArray()) {
            QJsonObject ref = refVal.toObject();
            QString refType = ref.value("type").toString();
            QString refId = ref.value("id").toString();

            if (tweetsById.contains(refId)) {
                QJsonObject refTweet = QJsonObject::fromVariantMap(tweetsById.value(refId).toMap());
                QVariantMap normalizedRef = normalizeTweetV2(refTweet, usersById, mediaByKey, tweetsById);
                if (refType == "retweeted") {
                    v1Tweet.insert("retweeted_status", normalizedRef);
                } else if (refType == "quoted") {
                    v1Tweet.insert("quoted_status", normalizedRef);
                }
            }
            if (refType == "replied_to") {
                v1Tweet.insert("in_reply_to_status_id_str", refId);
                v1Tweet.insert("in_reply_to_status_id", refId.toLongLong());
            }
        }
    }

    QVariantMap v1Entities;
    if (tweet.contains("entities")) {
        v1Entities = normalizeEntitiesV2(tweet.value("entities").toObject(), usersById);
    } else {
        v1Entities.insert("hashtags", QVariantList());
        v1Entities.insert("user_mentions", QVariantList());
        v1Entities.insert("urls", QVariantList());
        v1Entities.insert("symbols", QVariantList());
    }
    v1Tweet.insert("entities", v1Entities);

    if (tweet.contains("note_tweet")) {
        QJsonObject noteTweet = tweet.value("note_tweet").toObject();
        v1Tweet.insert("note_tweet", noteTweet.value("text").toString());
        if (noteTweet.contains("entities")) {
            v1Tweet.insert("note_tweet_entities", normalizeEntitiesV2(noteTweet.value("entities").toObject(), usersById));
        }
    }

    if (tweet.contains("attachments")) {
        QJsonArray mediaKeys = tweet.value("attachments").toObject().value("media_keys").toArray();
        QVariantList v1MediaList;
        for (const QJsonValue &mkVal : mediaKeys) {
            QString mediaKey = mkVal.toString();
            if (mediaByKey.contains(mediaKey)) {
                QJsonObject media = QJsonObject::fromVariantMap(mediaByKey.value(mediaKey).toMap());
                v1MediaList.append(normalizeMediaV2(media));
            }
        }
        if (!v1MediaList.isEmpty()) {
            QVariantMap extendedEntities;
            extendedEntities.insert("media", v1MediaList);
            v1Tweet.insert("extended_entities", extendedEntities);
        }
    }

    return v1Tweet;
}

QVariantMap TwitterApi::parseErrorResponse(const QString &errorText, const QByteArray &responseText)
{
    qDebug() << "TwitterApi::parseErrorResponse" << errorText << responseText;
    QVariantMap errorResponse;
    errorResponse.insert("message", errorText);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(responseText);
    if (jsonDocument.isObject()) {
        QJsonValue errorsValue = jsonDocument.object().value("errors");
        if (errorsValue.isArray()) {
            foreach (const QJsonValue &errorsValue, errorsValue.toArray()) {
                QJsonObject errorElementObject = errorsValue.toObject();
                errorResponse.insert("code", QString::number(errorElementObject.value("code").toInt()));
                errorResponse.insert("message", errorElementObject.value("message").toString());
            }
        }
    }
    return errorResponse;
}

void TwitterApi::handleTweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleTweetError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit tweetError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleTweetFinished()
{
    qDebug() << "TwitterApi::handleTweetFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit tweetSuccessful(responseObject.toVariantMap());
    } else {
        emit tweetError("Piepmatz couldn't understand Twitter's response! (Tweet finished)");
    }
}

void TwitterApi::handleHomeTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHomeTimelineError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit homeTimelineError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleHomeTimelineFinished()
{
    qDebug() << "TwitterApi::handleHomeTimelineFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QString nextToken;
        QVariantList tweets = normalizeV2TimelineResponse(jsonDocument.object(), nextToken);
        emit homeTimelineSuccessful(tweets, false, nextToken);
    } else {
        emit homeTimelineError("Piepmatz couldn't understand Twitter's response! (Home Timeline)");
    }
}

void TwitterApi::handleHomeTimelineLoadMoreFinished()
{
    qDebug() << "TwitterApi::handleHomeTimelineLoadMoreFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QString nextToken;
        QVariantList tweets = normalizeV2TimelineResponse(jsonDocument.object(), nextToken);
        emit homeTimelineSuccessful(tweets, true, nextToken);
    } else {
        emit homeTimelineError("Piepmatz couldn't understand Twitter's response! (Timeline load more)");
    }
}

void TwitterApi::handleMentionsTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleMentionsTimelineError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit mentionsTimelineError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleMentionsTimelineFinished()
{
    qDebug() << "TwitterApi::handleMentionsTimelineFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit mentionsTimelineSuccessful(responseArray.toVariantList());
    } else {
        emit mentionsTimelineError("Piepmatz couldn't understand Twitter's response! (Mentions Timeline)");
    }
}

void TwitterApi::handleRetweetTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleRetweetTimelineError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit mentionsTimelineError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleRetweetTimelineFinished()
{
    qDebug() << "TwitterApi::handleRetweetTimelineFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit retweetTimelineSuccessful(responseArray.toVariantList());
    } else {
        emit retweetTimelineError("Piepmatz couldn't understand Twitter's response! (Retweet Timeline)");
    }
}

void TwitterApi::handleUserTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUserTimelineError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    QUrlQuery urlQuery(reply->request().url());
    if (reply->request().hasRawHeader(HEADER_NO_RECURSION)) {
        qDebug() << "Probably a secret identity response...";
    } else {
        qDebug() << "Standard response...";
    }
    // We use the secret identity if it exists, if we were blocked and if the previous request wasn't already a secret request
    if (secretIdentityRequestor != nullptr && parsedErrorResponse.value("code") == "136" && !reply->request().hasRawHeader(HEADER_NO_RECURSION)) {
        qDebug() << "Using secret identity for user " << urlQuery.queryItemValue("screen_name");
        this->userTimeline(urlQuery.queryItemValue("screen_name"), true);
    } else {
        emit userTimelineError(parsedErrorResponse.value("message").toString());
    }
}

void TwitterApi::handleUserTimelineFinished()
{
    qDebug() << "TwitterApi::handleUserTimelineFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit userTimelineSuccessful(responseArray.toVariantList());
    } else {
        emit userTimelineError("Piepmatz couldn't understand Twitter's response! (User Timeline)");
    }
}

void TwitterApi::handleFollowersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFollowersError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit followersError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleFollowersFinished()
{
    qDebug() << "TwitterApi::handleFollowersFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit followersSuccessful(responseObject.toVariantMap());
    } else {
        emit followersError("Piepmatz couldn't understand Twitter's response! (Followers)");
    }
}

void TwitterApi::handleFriendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFriendsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit friendsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleFriendsFinished()
{
    qDebug() << "TwitterApi::handleFriendsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit friendsSuccessful(responseObject.toVariantMap());
    } else {
        emit friendsError("Piepmatz couldn't understand Twitter's response! (Friends)");
    }
}

void TwitterApi::handleShowStatusError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleShowStatusError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    qDebug() << "Tweet couldn't be loaded for URL " << reply->request().url().toString() << ", errors: " << parsedErrorResponse;
    // emit showStatusError(parsedErrorResponse.value("message").toString());
    // Extract tweet ID from v2 URL path: https://api.x.com/2/tweets/{id}?...
    QString requestPath = reply->request().url().path();
    QString tweetId = requestPath.mid(requestPath.lastIndexOf('/') + 1);
    if (reply->request().hasRawHeader(HEADER_NO_RECURSION)) {
        qDebug() << "Probably a secret identity response...";
    } else {
        qDebug() << "Standard response...";
    }
    // We use the secret identity if it exists, if we were blocked and if the previous request wasn't already a secret request
    if (secretIdentityRequestor != nullptr && parsedErrorResponse.value("code") == "136" && !reply->request().hasRawHeader(HEADER_NO_RECURSION)) {
        qDebug() << "Using secret identity for tweet " << tweetId;
        this->showStatus(tweetId, true);
    } else {
        QVariantMap fakeTweet;
        fakeTweet.insert("fakeTweet", true);
        QVariantMap fakeUser;
        fakeUser.insert("name", "");
        fakeUser.insert("verified", false);
        fakeUser.insert("protected", false);
        fakeUser.insert("profile_image_url_https", "");
        fakeTweet.insert("user", fakeUser);
        fakeTweet.insert("source", "Piepmatz");
        fakeTweet.insert("retweeted", false);
        fakeTweet.insert("favorited", false);
        QVariantMap fakeEntities;
        QVariantList fakeHashtags;
        fakeEntities.insert("hashtags", fakeHashtags);
        QVariantList fakeSymbols;
        fakeEntities.insert("symbols", fakeSymbols);
        QVariantList fakeUrls;
        fakeEntities.insert("urls", fakeUrls);
        QVariantList fakeMentions;
        fakeEntities.insert("user_mentions", fakeMentions);
        fakeTweet.insert("entities", fakeEntities);
        fakeTweet.insert("created_at", "Sun Jan 05 13:05:00 +0000 2020");
        fakeTweet.insert("id_str", tweetId);
        fakeTweet.insert("full_text", parsedErrorResponse.value("message").toString());
        emit showStatusSuccessful(fakeTweet);
    }
}

void TwitterApi::handleShowStatusFinished()
{
    qDebug() << "TwitterApi::handleShowStatusFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        QJsonObject tweetData = responseObject.value("data").toObject();
        if (tweetData.isEmpty()) {
            emit showStatusError("Piepmatz couldn't understand Twitter's response! (Show Status)");
            return;
        }

        QJsonObject includes = responseObject.value("includes").toObject();

        QVariantMap usersById;
        for (const QJsonValue &val : includes.value("users").toArray()) {
            QJsonObject user = val.toObject();
            usersById.insert(user.value("id").toString(), normalizeUserV2(user));
        }

        QVariantMap mediaByKey;
        for (const QJsonValue &val : includes.value("media").toArray()) {
            QJsonObject media = val.toObject();
            mediaByKey.insert(media.value("media_key").toString(), media.toVariantMap());
        }

        QVariantMap tweetsById;
        for (const QJsonValue &val : includes.value("tweets").toArray()) {
            QJsonObject tweet = val.toObject();
            tweetsById.insert(tweet.value("id").toString(), tweet.toVariantMap());
        }

        QVariantMap normalizedTweet = normalizeTweetV2(tweetData, usersById, mediaByKey, tweetsById);
        emit showStatusSuccessful(normalizedTweet);
    } else {
        emit showStatusError("Piepmatz couldn't understand Twitter's response! (Show Status)");
    }
}

void TwitterApi::handleShowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleShowUserError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit showUserError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleShowUserFinished()
{
    qDebug() << "TwitterApi::handleShowUserFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit showUserSuccessful(responseObject.toVariantMap());
    } else {
        emit showUserError("Piepmatz couldn't understand Twitter's response! (Show User)");
    }
}

void TwitterApi::handleShowUserByIdError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleShowUserByIdError:" << (int)error << reply->errorString();
    emit showUserByIdError(reply->errorString());
}

void TwitterApi::handleShowUserByIdFinished()
{
    qDebug() << "TwitterApi::handleShowUserByIdFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit showUserByIdSuccessful(responseObject.toVariantMap());
    } else {
        emit showUserByIdError("Piepmatz couldn't understand Twitter's response! (Show user by ID)");
    }
}

void TwitterApi::handleFollowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFollowUserError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit followUserError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleFollowUserFinished()
{
    qDebug() << "TwitterApi::handleFollowUserFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        // Sometimes, Twitter still says "following": true here - strange isn't it?
        responseObject.remove("following");
        responseObject.insert("following", QJsonValue(true));
        emit followUserSuccessful(responseObject.toVariantMap());
    } else {
        emit followUserError("Piepmatz couldn't understand Twitter's response! (Follow User)");
    }
}

void TwitterApi::handleUnfollowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnfollowUserError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit unfollowUserError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleUnfollowUserFinished()
{
    qDebug() << "TwitterApi::handleUnfollowUserFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        // Sometimes, Twitter still says "following": false here - strange isn't it?
        responseObject.remove("following");
        responseObject.insert("following", QJsonValue(false));
        emit unfollowUserSuccessful(responseObject.toVariantMap());
    } else {
        emit unfollowUserError("Piepmatz couldn't understand Twitter's response! (Unfollow user)");
    }
}

void TwitterApi::handleSearchTweetsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchTweetsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit searchTweetsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleSearchTweetsFinished()
{
    qDebug() << "TwitterApi::handleSearchTweetsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        // We try to remove duplicate tweets which come in due to retweets
        QJsonArray originalResultsArray = responseObject.value("statuses").toArray();
        QList<QString> foundStatusIds;
        QJsonArray resultsArray;
        for (int i = 0; i < originalResultsArray.size(); i++) {
            QJsonObject currentObject = originalResultsArray.at(i).toObject();
            QString currentStatusId;
            if (currentObject.contains("retweeted_status")) {
                currentStatusId = currentObject.value("retweeted_status").toObject().value("id_str").toString();
            } else {
                currentStatusId = currentObject.value("id_str").toString();
            }
            if (!foundStatusIds.contains(currentStatusId)) {
                resultsArray.append(currentObject);
                foundStatusIds.append(currentStatusId);
            }
        }
        emit searchTweetsSuccessful(resultsArray.toVariantList());
    } else {
        emit searchTweetsError("Piepmatz couldn't understand Twitter's response! (Search tweets)");
    }
}

void TwitterApi::handleSearchUsersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchUsersError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit searchUsersError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleSearchUsersFinished()
{
    qDebug() << "TwitterApi::handleSearchUsersFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit searchUsersSuccessful(responseArray.toVariantList());
    } else {
        emit searchUsersError("Piepmatz couldn't understand Twitter's response! (Search users)");
    }
}

void TwitterApi::handleSearchGeoError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchGeoError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit searchGeoError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleSearchGeoFinished()
{
    qDebug() << "TwitterApi::handleSearchGeoFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit searchGeoSuccessful(responseObject.toVariantMap());
    } else {
        emit searchGeoError("Piepmatz couldn't understand Twitter's response! (Search geo)");
    }
}

void TwitterApi::handleFavoriteError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFavoriteError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit favoriteError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleFavoriteFinished()
{
    qDebug() << "TwitterApi::handleFavoriteFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit favoriteSuccessful(responseObject.toVariantMap());
    } else {
        emit favoriteError("Piepmatz couldn't understand Twitter's response! (Favorite)");
    }
}

void TwitterApi::handleUnfavoriteError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnfavoriteError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit unfavoriteError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleUnfavoriteFinished()
{
    qDebug() << "TwitterApi::handleUnfavoriteFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit unfavoriteSuccessful(responseObject.toVariantMap());
    } else {
        emit unfavoriteError("Piepmatz couldn't understand Twitter's response! (Unfavorite)");
    }
}

void TwitterApi::handleFavoritesError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFavoritesError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit favoritesError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleFavoritesFinished()
{
    qDebug() << "TwitterApi::handleFavoritesFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit favoritesSuccessful(responseArray.toVariantList());
    } else {
        emit favoritesError("Piepmatz couldn't understand Twitter's response! (Favorites)");
    }
}

void TwitterApi::handleRetweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleRetweetError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit retweetError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleRetweetFinished()
{
    qDebug() << "TwitterApi::handleRetweetFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit retweetSuccessful(responseObject.toVariantMap());
    } else {
        emit retweetError("Piepmatz couldn't understand Twitter's response! (Retweet)");
    }
}

void TwitterApi::handleRetweetsForError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QString requestPath = reply->request().url().path();
    QRegExp statusRegex("(\\d+)\\.json");
    QString statusId;
    if (statusRegex.indexIn(requestPath) != -1) {
        statusId = statusRegex.cap(1);
    }
    qWarning() << "TwitterApi::handleRetweetUsersError:" << (int)error << reply->errorString() << reply->readAll() << statusId;
    emit retweetsForError(statusId, reply->errorString());
}

void TwitterApi::handleRetweetsForFinished()
{
    qDebug() << "TwitterApi::handleRetweetsForFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }
    QString requestPath = reply->request().url().path();
    QRegExp statusRegex("(\\d+)\\.json");
    QString statusId;
    if (statusRegex.indexIn(requestPath) != -1) {
        statusId = statusRegex.cap(1);
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit retweetsForSuccessful(statusId, responseArray.toVariantList());
    } else {
        emit retweetsForError(statusId, "Piepmatz couldn't understand Twitter's response! (Retweets for)");
    }
}

void TwitterApi::handleUnretweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnretweetError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit unretweetError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleUnretweetFinished()
{
    qDebug() << "TwitterApi::handleUnretweetFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit unretweetSuccessful(responseObject.toVariantMap());
    } else {
        emit unretweetError("Piepmatz couldn't understand Twitter's response! (Unretweet)");
    }
}

void TwitterApi::handleDestroyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDestroyError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit destroyError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleDestroyFinished()
{
    qDebug() << "TwitterApi::handleDestroyFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit destroySuccessful(responseObject.toVariantMap());
    } else {
        emit destroyError("Piepmatz couldn't understand Twitter's response! (Destroy)");
    }
}

void TwitterApi::handleDirectMessagesListError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDirectMessagesListError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit directMessagesListError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleDirectMessagesListFinished()
{
    qDebug() << "TwitterApi::handleDirectMessagesListFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit directMessagesListSuccessful(responseObject.toVariantMap());
    } else {
        emit directMessagesListError("Piepmatz couldn't understand Twitter's response! (Direct Messages List)");
    }
}

void TwitterApi::handleDirectMessagesNewError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDirectMessagesNewError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit directMessagesNewError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleDirectMessagesNewFinished()
{
    qDebug() << "TwitterApi::handleDirectMessagesNewFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit directMessagesNewSuccessful(responseObject.toVariantMap());
    } else {
        emit directMessagesNewError("Piepmatz couldn't understand Twitter's response! (Direct Messages New)");
    }
}

void TwitterApi::handleTrendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleTrendsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit trendsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleTrendsFinished()
{
    qDebug() << "TwitterApi::handleTrendsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit trendsSuccessful(responseArray.toVariantList());
    } else {
        emit trendsError("Piepmatz couldn't understand Twitter's response! (Trends)");
    }
}

void TwitterApi::handlePlacesForTrendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handlePlacesForTrendsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit placesForTrendsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handlePlacesForTrendsFinished()
{
    qDebug() << "TwitterApi::handlePlacesForTrendsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit placesForTrendsSuccessful(responseArray.toVariantList());
    } else {
        emit placesForTrendsError("Piepmatz couldn't understand Twitter's response! (Places for Trends)");
    }
}

void TwitterApi::handleUserListsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUserListsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit userListsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleUserListsFinished()
{
    qDebug() << "TwitterApi::handleUserListsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit userListsSuccessful(responseArray.toVariantList());
    } else {
        emit userListsError("Piepmatz couldn't understand Twitter's response! (User Lists)");
    }
}

void TwitterApi::handleListsMembershipsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListsMembershipsError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit listsMembershipsError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleListsMembershipsFinished()
{
    qDebug() << "TwitterApi::handleListsMembershipsFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit listsMembershipsSuccessful(responseObject.toVariantMap());
    } else {
        emit listsMembershipsError("Piepmatz couldn't understand Twitter's response! (Lists Memberships)");
    }
}

void TwitterApi::handleListMembersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListsMembersError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit listMembersError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleListMembersFinished()
{
    qDebug() << "TwitterApi::handleListsMembersFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit listMembersSuccessful(responseObject.toVariantMap());
    } else {
        emit listMembersError("Piepmatz couldn't understand Twitter's response! (List Members)");
    }
}

void TwitterApi::handleListTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListTimelineError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit listTimelineError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleListTimelineFinished()
{
    qDebug() << "TwitterApi::handleListTimelineFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit listTimelineSuccessful(responseArray.toVariantList(), false);
    } else {
        emit listTimelineError("Piepmatz couldn't understand Twitter's response! (List Timeline)");
    }
}

void TwitterApi::handleListTimelineLoadMoreFinished()
{
    qDebug() << "TwitterApi::handleListTimelineLoadMoreFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit listTimelineSuccessful(responseArray.toVariantList(), true);
    } else {
        emit listTimelineError("Piepmatz couldn't understand Twitter's response! (List Timeline load more)");
    }
}

void TwitterApi::handleSavedSearchesError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSavedSearchesError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit savedSearchesError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleSavedSearchesFinished()
{
    qDebug() << "TwitterApi::handleSavedSearchesFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit savedSearchesSuccessful(responseArray.toVariantList());
    } else {
        emit savedSearchesError("Piepmatz couldn't understand Twitter's response! (Saved Searches)");
    }
}

void TwitterApi::handleSaveSearchError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSaveSearchError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit saveSearchError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleSaveSearchFinished()
{
    qDebug() << "TwitterApi::handleSaveSearchFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit saveSearchSuccessful(responseObject.toVariantMap());
    } else {
        emit saveSearchError("Piepmatz couldn't understand Twitter's response! (Save search)");
    }
}

void TwitterApi::handleDestroySavedSearchError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDestroySavedSearchError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit destroySavedSearchError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleDestroySavedSearchFinished()
{
    qDebug() << "TwitterApi::handleDestroySavedSearchFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit destroySavedSearchSuccessful(responseObject.toVariantMap());
    } else {
        emit destroySavedSearchError("Piepmatz couldn't understand Twitter's response! (Destroy saved search)");
    }
}

void TwitterApi::handleGetOpenGraphError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleGetOpenGraphFinished:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit getOpenGraphError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleGetOpenGraphFinished()
{
    qDebug() << "TwitterApi::handleGetOpenGraphFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString requestAddress = reply->request().url().toString();

    QVariant contentTypeHeader = reply->header(QNetworkRequest::ContentTypeHeader);
    if (!contentTypeHeader.isValid()) {
        return;
    }
    qDebug() << "Open Graph content type header: " << contentTypeHeader.toString();
    if (contentTypeHeader.toString().indexOf("text/html", 0, Qt::CaseInsensitive) == -1) {
        qDebug() << requestAddress + " is not HTML, not checking Open Graph data...";
        return;
    }

    QString charset = "UTF-8";
    QRegularExpression charsetRegularExpression("charset\\s*\\=[\\s\\\"\\\']*([^\\s\\\"\\\'\\,>]*)");
    QRegularExpressionMatchIterator matchIterator = charsetRegularExpression.globalMatch(contentTypeHeader.toString());
    QStringList availableCharsets;
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch nextMatch = matchIterator.next();
        QString currentCharset = nextMatch.captured(1).toUpper();
        qDebug() << "Available Open Graph charset: " << currentCharset;
        availableCharsets.append(currentCharset);
    }
    if (availableCharsets.size() > 0 && !availableCharsets.contains("UTF-8")) {
        // If we haven't received the requested UTF-8, we simply use the last one which we received in the header
        charset = availableCharsets.last();
    }
    qDebug() << "Open Graph Charset for " << requestAddress << ": " << charset;

    QByteArray rawDocument = reply->readAll();
    QTextCodec *codec = QTextCodec::codecForName(charset.toUtf8());
    if (codec == nullptr){
      emit getOpenGraphError(requestAddress + " unsupported Charset: " + charset);
      return;
    }
    QString resultDocument = codec->toUnicode(rawDocument);

    QVariantMap openGraphData;
    QRegExp urlRegex("\\<meta\\s+property\\=\\\"og\\:url\\\"\\s+content\\=\\\"([^\\\"]+)\\\"");
    if (urlRegex.indexIn(resultDocument) != -1) {
        openGraphData.insert("url", urlRegex.cap(1));
    }
    QRegExp imageRegex("\\<meta\\s+property\\=\\\"og\\:image\\\"\\s+content\\=\\\"([^\\\"]+)\\\"");
    if (imageRegex.indexIn(resultDocument) != -1) {
        openGraphData.insert("image", imageRegex.cap(1));
    }
    QRegExp descriptionRegex("\\<meta\\s+property\\=\\\"og\\:description\\\"\\s+content\\=\\\"([^\\\"]+)\\\"");
    if (descriptionRegex.indexIn(resultDocument) != -1) {
        openGraphData.insert("description", descriptionRegex.cap(1));
    }
    QRegExp titleRegex("\\<meta\\s+property\\=\\\"og\\:title\\\"\\s+content\\=\\\"([^\\\"]+)\\\"");
    if (titleRegex.indexIn(resultDocument) != -1) {
        openGraphData.insert("title", titleRegex.cap(1));
    }

    if (openGraphData.isEmpty()) {
        emit getOpenGraphError(requestAddress + " does not contain Open Graph data");
    } else {
        // Always using request URL to be able to compare results
        openGraphData.insert("url", requestAddress);
        if (!openGraphData.contains("title")) {
            openGraphData.insert("title", openGraphData.value("url"));
        }
        qDebug() << "Open Graph data found for " + requestAddress;
        emit getOpenGraphSuccessful(openGraphData);
    }
}

void TwitterApi::handleGetSingleTweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleGetSingleTweetError:" << (int)error << reply->errorString();
    QString requestAddress = reply->request().url().toString();
    qDebug() << "TwitterApi::handleGetSingleTweetError, request address:" << requestAddress;
    QString requestTweetId = requestAddress.mid(requestAddress.indexOf("/status/") + 8);
    qDebug() << "TwitterApi::handleGetSingleTweetError, request status ID:" << requestTweetId;
    qDebug() << "TwitterApi::handleGetSingleTweetError, trying with Twitter APIv2";
    this->getSingleTweetWithConversationId(requestTweetId);
}

void TwitterApi::handleGetSingleTweetFinished()
{
    qDebug() << "TwitterApi::handleGetSingleTweetFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "getSingleTweetError! " << reply->errorString();
        return;
    }

    QString requestAddress = reply->request().url().toString();
    qDebug() << "Processing response for tweet page " << requestAddress;

    QVariant contentTypeHeader = reply->header(QNetworkRequest::ContentTypeHeader);
    qDebug() << "Content type header " << contentTypeHeader.toString();
    if (!contentTypeHeader.isValid()) {
        qDebug() << "Content Type response header is invalid, unable to check for conversation!";
        return;
    }
    if (contentTypeHeader.toString().indexOf("text/html", 0, Qt::CaseInsensitive) == -1) {
        qDebug() << requestAddress + " is not HTML, not checking tweet result data...";
        return;
    }

    QRegExp tweetIdRegex("status\\/(\\d+)");
    QString currentTweetId;
    if (tweetIdRegex.indexIn(requestAddress) != -1) {
        currentTweetId = tweetIdRegex.cap(1);
    }

    QString resultDocument(reply->readAll());
    QGumboDocument parsedResult = QGumboDocument::parse(resultDocument);
    QGumboNode root = parsedResult.rootNode();

    QVariantList relatedTweets;

    QGumboNodes metaNodes = root.getElementsByTagName(HtmlTag::META);
    for (QGumboNode &metaNode : metaNodes) {
        if (!metaNode.hasAttribute("itemProp")) {
            continue;
        }
        if (metaNode.getAttribute("itemProp") != "url") {
            continue;
        }
        QString tweetUrlCandidate = metaNode.getAttribute("content");
        QRegExp contentTweetIdRegex("status\\/(\\d+)$");
        if (contentTweetIdRegex.indexIn(tweetUrlCandidate) != -1) {
            QString otherTweetId = contentTweetIdRegex.cap(1);
            qDebug() << "Found Tweet ID: " << otherTweetId;
            if (!relatedTweets.contains(otherTweetId)) {
                relatedTweets.append(otherTweetId);
            }
        }
    }

    if (!relatedTweets.isEmpty()) {
        qDebug() << "Found other tweets, let's build a conversation!";
        TweetConversationHandler *conversationHandler = new TweetConversationHandler(this, currentTweetId, relatedTweets, this);
        connect(conversationHandler, SIGNAL(tweetConversationCompleted(QString, QVariantList)), this, SLOT(handleTweetConversationReceived(QString, QVariantList)));
        conversationHandler->buildConversation();
    }
}

void TwitterApi::handleGetSingleTweetWithConversationIdFinished()
{
    qDebug() << "TwitterApi::handleGetSingleTweetWithConversationIdFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "getSingleTweetWithConversationIdError! " << reply->errorString();
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QString conversationId = jsonDocument.object().value("data").toObject().value("conversation_id").toString();
        if (!conversationId.isEmpty()) {
            qDebug() << "Found conversation with ID: " << conversationId;
            this->getTweetConversation(conversationId);
        }
    }
}

void traverseConversation(QString &conversationId, QString rootId, QVariantList &relatedTweets, QVariantMap &conversationTweets) {
    if (!rootId.isEmpty() && !relatedTweets.contains(rootId)) {
        qDebug() << "[Conversation] Adding tweet to conversation, ID" << rootId;
        relatedTweets.append(rootId);
    }
    QListIterator<QVariant> conversationIterator(conversationTweets.values());
    while (conversationIterator.hasNext()) {
        QVariantMap currentTweet = conversationIterator.next().toMap();
        if (currentTweet.value("parent_id").toString() == rootId) {
            QString newRootId = currentTweet.value("id").toString();
            if (!relatedTweets.contains(newRootId)) {
                qDebug() << "[Conversation] Adding tweet to conversation, ID" << newRootId;
                relatedTweets.append(newRootId);
                qDebug() << "[Conversation] Traversing hierarchy with ID " << newRootId;
                traverseConversation(conversationId, newRootId, relatedTweets, conversationTweets);
            }
        }
    }
}

void TwitterApi::handleGetTweetConversationFinished()
{
    qDebug() << "TwitterApi::handleGetTweetConversationFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "getTweetConversationError! " << reply->errorString();
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());

    //qDebug().noquote() << jsonDocument.toJson();

    if (jsonDocument.isObject()) {
        QVariantMap responseObject = jsonDocument.object().toVariantMap();
        QVariantList conversationData = responseObject.value("data").toList();
        qDebug() << "[Conversation] Found tweet count" << conversationData.size();
        QVariantList includedTweets = responseObject.value("includes").toMap().value("tweets").toList();
        qDebug() << "[Conversation] Included tweet count" << includedTweets.size();
        conversationData.append(includedTweets);
        qDebug() << "[Conversation] Entire tweet count" << conversationData.size();

        // qDebug().noquote() << QJsonDocument::fromVariant(conversationData).toJson();

        QListIterator<QVariant> conversationIterator(conversationData);
        QVariantMap conversationTweets;
        QList<QString> rootIds;
        QString conversationId;
        QString conversationAuthorId;
        while (conversationIterator.hasNext()) {
            QVariantMap singleTweet = conversationIterator.next().toMap();
            conversationId = singleTweet.value("conversation_id").toString();
            QString currentTweetId = singleTweet.value("id").toString();

            if (currentTweetId == conversationId) {
                conversationAuthorId = singleTweet.value("author_id").toString();
            }

            qDebug() << "[Conversation] Processing tweet ID" << currentTweetId;

            bool isReply = false;
            QVariantList referencedTweets = singleTweet.value("referenced_tweets").toList();
            if (!referencedTweets.isEmpty()) {
                QListIterator<QVariant> referenceIterator(referencedTweets);
                while (referenceIterator.hasNext()) {
                    QVariantMap singleReference = referenceIterator.next().toMap();
                    if (singleReference.value("type").toString() == "replied_to") {
                        isReply = true;
                        QString inReplyToId = singleReference.value("id").toString();
                        singleTweet.insert("parent_id", inReplyToId);
                        if (!rootIds.contains(inReplyToId)) {
                            qDebug() << "[Conversation] Adding root candidate with ID" << inReplyToId;
                            rootIds.append(inReplyToId);
                        }
                    }
                }
            }

            if (isReply && rootIds.contains(currentTweetId)) {
                qDebug() << "[Conversation] Removing root candidate with ID" << currentTweetId;
                rootIds.removeAll(currentTweetId);
            }

            conversationTweets.insert(currentTweetId, singleTweet);
        }

        QVariantList relatedTweets;
        if (rootIds.size() == 1 && rootIds.at(0) == conversationId) {
            qDebug() << "[Conversation] Single root tweet is conversation root - checking for thread by original author" << rootIds.at(0);
            relatedTweets.append(rootIds.at(0));
            QString currentTweetId = rootIds.at(0);
            conversationIterator.toFront();
            QString childTweetId;
            while (conversationIterator.hasNext()) {
                QVariantMap singleTweet = conversationIterator.next().toMap();
                if (singleTweet.value("author_id").toString() != conversationAuthorId) {
                    continue;
                }
                childTweetId.clear();
                QVariantList referencedTweets = singleTweet.value("referenced_tweets").toList();
                if (!referencedTweets.isEmpty()) {
                    QListIterator<QVariant> referenceIterator(referencedTweets);
                    while (referenceIterator.hasNext()) {
                        QVariantMap singleReference = referenceIterator.next().toMap();
                        if (singleReference.value("type").toString() == "replied_to" && singleReference.value("id").toString() == currentTweetId) {
                            childTweetId = singleTweet.value("id").toString();
                            break;
                        }
                    }
                }
                if (!childTweetId.isEmpty()) {
                    qDebug() << "[Conversation] Child tweet from original author found, continuing thread construction:" << childTweetId;
                    relatedTweets.append(childTweetId);
                    currentTweetId = childTweetId;
                    conversationIterator.toFront();
                }
            }
        }

        QListIterator<QString> rootIdIterator(rootIds);
        while (rootIdIterator.hasNext()) {
            QString rootId = rootIdIterator.next();
            qDebug() << "[Conversation] Processing root ID" << rootId;
            traverseConversation(conversationId, rootId, relatedTweets, conversationTweets);
        }

        if (relatedTweets.size() > 1) {
            qDebug() << "[Conversation] Found other tweets, let's build a conversation!";
            TweetConversationHandler *conversationHandler = new TweetConversationHandler(this, conversationId, relatedTweets, this);
            connect(conversationHandler, SIGNAL(tweetConversationCompleted(QString, QVariantList)), this, SLOT(handleTweetConversationReceived(QString, QVariantList)));
            conversationHandler->buildConversation();
        }

    }
}

void TwitterApi::handleTweetConversationReceived(QString tweetId, QVariantList receivedTweets)
{
    emit tweetConversationReceived(tweetId, receivedTweets);
}

void TwitterApi::handleGetIpInfoError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleGetIpInfoError:" << (int)error << reply->errorString();
    QVariantMap parsedErrorResponse = parseErrorResponse(reply->errorString(), reply->readAll());
    emit getIpInfoError(parsedErrorResponse.value("message").toString());
}

void TwitterApi::handleGetIpInfoFinished()
{
    qDebug() << "TwitterApi::handleGetIpInfoFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit getIpInfoSuccessful(responseObject.toVariantMap());
    } else {
        emit getIpInfoError("Piepmatz couldn't understand Twitter's response!");
    }
}
