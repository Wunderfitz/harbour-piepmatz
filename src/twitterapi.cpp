/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

TwitterApi::TwitterApi(O1Requestor* requestor, QNetworkAccessManager *manager, Wagnis *wagnis, QObject* parent) : QObject(parent) {
    this->requestor = requestor;
    this->manager = manager;
    this->wagnis = wagnis;
}

void TwitterApi::verifyCredentials()
{
    qDebug() << "TwitterApi::verifyCredentials";

    if (!wagnis->hasFeature("contribution") && wagnis->getRemainingTime() == 0) {
        emit verifyCredentialsError("You haven't completed the registration process!");
        return;
    }

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

void TwitterApi::helpConfiguration()
{
    qDebug() << "TwitterApi::helpConfiguration";
    QUrl url = QUrl(API_HELP_CONFIGURATION);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHelpConfigurationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleHelpConfigurationSuccessful()));
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
        emit verifyCredentialsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleVerifyCredentialsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleVerifyCredentialsError:" << (int)error << reply->errorString() << reply->readAll();
    emit verifyCredentialsError(reply->errorString());
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
        emit accountSettingsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleAccountSettingsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleAccountSettingsError:" << (int)error << reply->errorString() << reply->readAll();
    emit accountSettingsError(reply->errorString());
}

void TwitterApi::handleHelpConfigurationSuccessful()
{
    qDebug() << "TwitterApi::handleHelpConfigurationSuccessful";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        emit helpConfigurationSuccessful(jsonDocument.object().toVariantMap());
    } else {
        emit helpConfigurationError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleHelpConfigurationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHelpConfigurationError:" << (int)error << reply->errorString() << reply->readAll();
    emit helpConfigurationError(reply->errorString());
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
        emit helpPrivacyError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleHelpPrivacyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHelpPrivacyError:" << (int)error << reply->errorString() << reply->readAll();
    emit helpPrivacyError(reply->errorString());
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
        emit helpTosError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleHelpTosError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHelpTosError:" << (int)error << reply->errorString() << reply->readAll();
    emit helpTosError(reply->errorString());
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

void TwitterApi::homeTimeline(const QString &maxId)
{
    qDebug() << "TwitterApi::homeTimeline" << maxId;
    QUrl url = QUrl(API_STATUSES_HOME_TIMELINE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("exclude_replies", "false");
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
    requestParameters.append(O0RequestParameter(QByteArray("exclude_replies"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    if (!maxId.isEmpty()) {
        requestParameters.append(O0RequestParameter(QByteArray("max_id"), maxId.toUtf8()));
    }
    QNetworkReply *reply = requestor->get(request, requestParameters);

    if (maxId.isEmpty()) {
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

void TwitterApi::showStatus(const QString &statusId)
{
    // Very weird, some statusIds contain a query string. Why?
    QString sanitizedStatus = statusId;
    int qm = statusId.indexOf(QLatin1Char('?'));
    if (qm >= 0) {
        sanitizedStatus = statusId.left(qm);
    }
    qDebug() << "TwitterApi::showStatus" << sanitizedStatus;
    QUrl url = QUrl(API_STATUSES_SHOW);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("trim_user", "false");
    urlQuery.addQueryItem("id", sanitizedStatus);
    urlQuery.addQueryItem("include_ext_alt_text", "true");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("trim_user"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("id"), sanitizedStatus.toUtf8()));
    requestParameters.append(O0RequestParameter(QByteArray("include_ext_alt_text"), QByteArray("true")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

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

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleShowUserError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleShowUserFinished()));
}

void TwitterApi::userTimeline(const QString &screenName)
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
    QNetworkReply *reply = requestor->get(request, requestParameters);

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

void TwitterApi::friends(const QString &screenName)
{
    qDebug() << "TwitterApi::friends" << screenName;
    QUrl url = QUrl(API_FRIENDS_LIST);
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
    QUrl url = QUrl(address);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Wayland; SailfishOS) Piepmatz (Not Firefox/52.0)");
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
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Wayland; SailfishOS) Piepmatz (Not Firefox/52.0)");
    request.setRawHeader(QByteArray("Accept-Charset"), QByteArray("utf-8"));
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));
    request.setRawHeader(QByteArray("Cache-Control"), QByteArray("max-age=0"));
    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetSingleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetSingleTweetFinished()));
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

void TwitterApi::handleTweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleTweetError:" << (int)error << reply->errorString() << reply->readAll();
    emit tweetError(reply->errorString());
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
        emit tweetError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleHomeTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleHomeTimelineError:" << (int)error << reply->errorString() << reply->readAll();
    emit homeTimelineError(reply->errorString());
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
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit homeTimelineSuccessful(responseArray.toVariantList(), false);
    } else {
        emit homeTimelineError("Piepmatz couldn't understand Twitter's response!");
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
    if (jsonDocument.isArray()) {
        QJsonArray responseArray = jsonDocument.array();
        emit homeTimelineSuccessful(responseArray.toVariantList(), true);
    } else {
        emit homeTimelineError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleMentionsTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleMentionsTimelineError:" << (int)error << reply->errorString() << reply->readAll();
    emit mentionsTimelineError(reply->errorString());
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
        emit mentionsTimelineError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleRetweetTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleRetweetTimelineError:" << (int)error << reply->errorString() << reply->readAll();
    emit mentionsTimelineError(reply->errorString());
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
        emit retweetTimelineError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleUserTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUserTimelineError:" << (int)error << reply->errorString() << reply->readAll();
    emit userTimelineError(reply->errorString());
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
        emit userTimelineError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleFollowersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFollowersError:" << (int)error << reply->errorString() << reply->readAll();
    emit followersError(reply->errorString());
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
        emit followersError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleFriendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFriendsError:" << (int)error << reply->errorString() << reply->readAll();
    emit friendsError(reply->errorString());
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
        emit friendsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleShowStatusError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleShowStatusError:" << (int)error << reply->errorString() << reply->readAll();
    emit showStatusError(reply->errorString());
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
        emit showStatusSuccessful(responseObject.toVariantMap());
    } else {
        emit showStatusError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleShowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleShowUserError:" << (int)error << reply->errorString() << reply->readAll();
    emit showUserError(reply->errorString());
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
        emit showUserError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleFollowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFollowUserError:" << (int)error << reply->errorString() << reply->readAll();
    emit followUserError(reply->errorString());
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
        emit followUserError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleUnfollowUserError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnfollowUserError:" << (int)error << reply->errorString() << reply->readAll();
    emit unfollowUserError(reply->errorString());
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
        emit unfollowUserError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleSearchTweetsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchTweetsError:" << (int)error << reply->errorString() << reply->readAll();
    emit searchTweetsError(reply->errorString());
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
        emit searchTweetsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleSearchUsersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchUsersError:" << (int)error << reply->errorString() << reply->readAll();
    emit searchUsersError(reply->errorString());
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
        emit searchUsersError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleSearchGeoError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSearchGeoError:" << (int)error << reply->errorString() << reply->readAll();
    emit searchGeoError(reply->errorString());
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
        emit searchGeoError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleFavoriteError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFavoriteError:" << (int)error << reply->errorString() << reply->readAll();
    emit favoriteError(reply->errorString());
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
        emit favoriteError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleUnfavoriteError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnfavoriteError:" << (int)error << reply->errorString() << reply->readAll();
    emit unfavoriteError(reply->errorString());
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
        emit unfavoriteError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleFavoritesError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleFavoritesError:" << (int)error << reply->errorString() << reply->readAll();
    emit favoritesError(reply->errorString());
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
        emit favoritesError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleRetweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleRetweetError:" << (int)error << reply->errorString() << reply->readAll();
    emit retweetError(reply->errorString());
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
        emit retweetError("Piepmatz couldn't understand Twitter's response!");
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
        emit retweetsForError(statusId, "Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleUnretweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUnretweetError:" << (int)error << reply->errorString() << reply->readAll();
    emit unretweetError(reply->errorString());
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
        emit unretweetError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleDestroyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDestroyError:" << (int)error << reply->errorString() << reply->readAll();
    emit destroyError(reply->errorString());
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
        emit destroyError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleDirectMessagesListError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDirectMessagesListError:" << (int)error << reply->errorString() << reply->readAll();
    emit directMessagesListError(reply->errorString());
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
        emit directMessagesListError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleDirectMessagesNewError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDirectMessagesNewError:" << (int)error << reply->errorString() << reply->readAll();
    emit directMessagesNewError(reply->errorString());
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
        emit directMessagesNewError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleTrendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleTrendsError:" << (int)error << reply->errorString() << reply->readAll();
    emit trendsError(reply->errorString());
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
        emit trendsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handlePlacesForTrendsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handlePlacesForTrendsError:" << (int)error << reply->errorString() << reply->readAll();
    emit placesForTrendsError(reply->errorString());
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
        emit placesForTrendsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleUserListsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleUserListsError:" << (int)error << reply->errorString() << reply->readAll();
    emit userListsError(reply->errorString());
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
        emit userListsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleListsMembershipsError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListsMembershipsError:" << (int)error << reply->errorString() << reply->readAll();
    emit listsMembershipsError(reply->errorString());
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
        emit listsMembershipsError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleListMembersError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListsMembersError:" << (int)error << reply->errorString() << reply->readAll();
    emit listMembersError(reply->errorString());
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
        emit listMembersError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleListTimelineError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleListTimelineError:" << (int)error << reply->errorString() << reply->readAll();
    emit listTimelineError(reply->errorString());
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
        emit listTimelineError("Piepmatz couldn't understand Twitter's response!");
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
        emit listTimelineError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleSavedSearchesError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSavedSearchesError:" << (int)error << reply->errorString() << reply->readAll();
    emit savedSearchesError(reply->errorString());
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
        emit savedSearchesError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleSaveSearchError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleSaveSearchError:" << (int)error << reply->errorString() << reply->readAll();
    emit saveSearchError(reply->errorString());
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
        emit saveSearchError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleDestroySavedSearchError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleDestroySavedSearchError:" << (int)error << reply->errorString() << reply->readAll();
    emit destroySavedSearchError(reply->errorString());
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
        emit destroySavedSearchError("Piepmatz couldn't understand Twitter's response!");
    }
}

void TwitterApi::handleGetOpenGraphError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleGetOpenGraphFinished:" << (int)error << reply->errorString() << reply->readAll();
    emit getOpenGraphError(reply->errorString());
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
    if (contentTypeHeader.toString().indexOf("text/html", 0, Qt::CaseInsensitive) == -1) {
        qDebug() << requestAddress + " is not HTML, not checking Open Graph data...";
        return;
    }

    QVariantMap openGraphData;

    QString resultDocument;
    if (contentTypeHeader.toString().indexOf("charset=iso-8859-1", 0, Qt::CaseInsensitive) != -1) {
        resultDocument = QString::fromLatin1(reply->readAll());
    } else {
        resultDocument = QString::fromUtf8(reply->readAll());
    }

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
    qWarning() << "TwitterApi::handleGetSingleTweetError:" << (int)error << reply->errorString() << reply->readAll();
}

void TwitterApi::handleGetSingleTweetFinished()
{
    qDebug() << "TwitterApi::handleGetSingleTweetFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString requestAddress = reply->request().url().toString();

    QVariant contentTypeHeader = reply->header(QNetworkRequest::ContentTypeHeader);
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

    // === DEBUG ===
    ContentExtractor contentExtractor(this, &root);
    contentExtractor.parse();
    // === DEBUG ===


    QGumboNodes tweetNodes = root.getElementsByClassName("tweet");
    QVariantList relatedTweets;
    for (QGumboNode &tweetNode : tweetNodes) {
        QStringList tweetClassList = tweetNode.classList();
        if (!tweetClassList.contains("promoted-tweet")) {
            QString otherTweetId = tweetNode.getAttribute("data-tweet-id");
            if (!otherTweetId.isEmpty()) {
                qDebug() << "Found Tweet ID: " << otherTweetId;
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

void TwitterApi::handleTweetConversationReceived(QString tweetId, QVariantList receivedTweets)
{
    emit tweetConversationReceived(tweetId, receivedTweets);
}

void TwitterApi::handleGetIpInfoError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleGetIpInfoError:" << (int)error << reply->errorString() << reply->readAll();
    emit getIpInfoError(reply->errorString());
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
