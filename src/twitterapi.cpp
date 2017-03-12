#include "twitterapi.h"

TwitterApi::TwitterApi(O1Requestor* requestor, QObject* parent) : QObject(parent) {
    this->requestor = requestor;
}

void TwitterApi::verifyCredentials()
{
    qDebug() << "TwitterApi::verifyCredentials";
    QUrl url = QUrl(API_ACCOUNT_VERIFY_CREDENTIALS);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleVerifyCredentialsError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleVerifyCredentialsSuccessful()));
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

void TwitterApi::tweet(const QString &text)
{
    qDebug() << "TwitterApi::tweet";
    QUrl url = QUrl(API_STATUSES_UPDATE);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    QByteArray parameterName("status");
    requestParameters.append(O0RequestParameter(parameterName, text.toUtf8()));
    QByteArray postData = O1::createQueryParameters(requestParameters);

    QNetworkReply *reply = requestor->post(request, requestParameters, postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleTweetError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleTweetFinished()));
}

void TwitterApi::homeTimeline()
{
    qDebug() << "TwitterApi::homeTimeline";
    QUrl url = QUrl(API_STATUSES_HOME_TIMELINE);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("exclude_replies", "false");
    urlQuery.addQueryItem("count", "200");
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("exclude_replies"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("count"), QByteArray("200")));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHomeTimelineError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleHomeTimelineFinished()));
}

void TwitterApi::showStatus(const QString &statusId)
{
    qDebug() << "TwitterApi::show" << statusId;
    QUrl url = QUrl(API_STATUSES_SHOW);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("tweet_mode", "extended");
    urlQuery.addQueryItem("include_entities", "true");
    urlQuery.addQueryItem("trim_user", "false");
    urlQuery.addQueryItem("id", statusId);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);

    QList<O0RequestParameter> requestParameters = QList<O0RequestParameter>();
    requestParameters.append(O0RequestParameter(QByteArray("tweet_mode"), QByteArray("extended")));
    requestParameters.append(O0RequestParameter(QByteArray("include_entities"), QByteArray("true")));
    requestParameters.append(O0RequestParameter(QByteArray("trim_user"), QByteArray("false")));
    requestParameters.append(O0RequestParameter(QByteArray("id"), statusId.toUtf8()));
    QNetworkReply *reply = requestor->get(request, requestParameters);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleShowStatusError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleShowStatusFinished()));
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
        emit homeTimelineSuccessful(responseArray.toVariantList());
    } else {
        emit homeTimelineError("Piepmatz couldn't understand Twitter's response!");
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
