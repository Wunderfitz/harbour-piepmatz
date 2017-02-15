#include "o1twitter.h"
#include "o0globals.h"

#include <QUrlQuery>

void O1Twitter::obtainPinUrl()
{
    setToken("");
    setTokenSecret("");
    setExtraTokens(QVariantMap());

    // Get any query parameters for the request
    QUrlQuery requestData;
    O0RequestParameter param("", "");
    foreach(param, requestParameters())
      requestData.addQueryItem(QString(param.name), QUrl::toPercentEncoding(QString(param.value)));

    // Get the request url and add parameters
    QUrl requestUrl = requestTokenUrl();
    requestUrl.setQuery(requestData);

    // Create request
    QNetworkRequest request(requestUrl);

    // Create initial token request
    QList<O0RequestParameter> headers;
    headers.append(O0RequestParameter(O2_OAUTH_CALLBACK, "oob"));
    headers.append(O0RequestParameter(O2_OAUTH_CONSUMER_KEY, clientId().toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_NONCE, nonce()));
    headers.append(O0RequestParameter(O2_OAUTH_TIMESTAMP, QString::number(QDateTime::currentDateTimeUtc().toTime_t()).toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_VERSION, "1.0"));
    headers.append(O0RequestParameter(O2_OAUTH_SIGNATURE_METHOD, signatureMethod().toLatin1()));
    headers.append(O0RequestParameter(O2_OAUTH_SIGNATURE, generateSignature(headers, request, requestParameters(), QNetworkAccessManager::PostOperation)));

    // Clear request token
    requestToken_.clear();
    requestTokenSecret_.clear();

    // Post request
    request.setRawHeader(O2_HTTP_AUTHORIZATION_HEADER, buildAuthorizationHeader(headers));
    request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
    QNetworkReply *reply = manager_->post(request, QByteArray());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onPinRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPinRequestFinished()));
}

void O1Twitter::onPinRequestError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "O1Twitter::onPinRequestError:" << (int)error << reply->errorString() << reply->readAll();
    emit pinRequestError(reply->errorString());
}

void O1Twitter::onPinRequestFinished()
{
    qDebug() << "O1Twitter::onPinRequestFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "O1Twitter::onPinRequestFinished: " << reply->errorString();
        return;
    }

    // Get request token and secret
    QByteArray data = reply->readAll();
    QMap<QString, QString> response = parseResponse(data);
    requestToken_ = response.value(O2_OAUTH_TOKEN, "");
    requestTokenSecret_ = response.value(O2_OAUTH_TOKEN_SECRET, "");
    setToken(requestToken_);
    setTokenSecret(requestTokenSecret_);

    // Checking for "oauth_callback_confirmed" is present and set to true
    QString oAuthCbConfirmed = response.value(O2_OAUTH_CALLBACK_CONFIRMED, "false");
    if (requestToken_.isEmpty() || requestTokenSecret_.isEmpty() || (oAuthCbConfirmed == "false")) {
        qWarning() << "O1Twitter::onPinRequestFinished: No oauth_token, oauth_token_secret or oauth_callback_confirmed in response :" << data;
        emit pinRequestError("No oauth_token, oauth_token_secret or oauth_callback_confirmed in PIN request response");
        return;
    }

    // Continue authorization flow in the browser
    QUrl url(authorizeUrl());
    QUrlQuery query(url);
    query.addQueryItem(O2_OAUTH_TOKEN, requestToken_);
    url.setQuery(query);
    qDebug() << "O1Twitter::onPinRequestFinished: PIN request URL successfully generated." << url;
    emit pinRequestSuccessful(url);
}
