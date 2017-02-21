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
        QJsonObject responseObject = jsonDocument.object();
        emit verifyCredentialsSuccessful(responseObject.toVariantMap());
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

void TwitterApi::handleTweetError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "TwitterApi::handleTweetError:" << (int)error << reply->errorString() << reply->readAll();
    emit tweetError(reply->errorString());
}

void TwitterApi::handleTweetFinished()
{
    qDebug() << "TwitterApi::handleVerifyCredentialsSuccessful";
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
