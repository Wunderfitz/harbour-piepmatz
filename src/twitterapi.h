#ifndef TWITTERAPI_H
#define TWITTERAPI_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include "o1requestor.h"
#include "o0requestparameter.h"
#include "o0globals.h"

const char API_ACCOUNT_VERIFY_CREDENTIALS[] = "https://api.twitter.com/1.1/account/verify_credentials.json";
const char API_STATUSES_UPDATE[] = "https://api.twitter.com/1.1/statuses/update.json";

class TwitterApi : public QObject {

    Q_OBJECT
public:
    TwitterApi(O1Requestor* requestor, QObject* parent = 0);

    Q_INVOKABLE void verifyCredentials();
    Q_INVOKABLE void tweet(const QString &text);

signals:
    void verifyCredentialsSuccessful(const QVariantMap &result);
    void verifyCredentialsError(const QString &errorMessage);
    void tweetSuccessful(const QVariantMap &result);
    void tweetError(const QString &errorMessage);

private:
    O1Requestor *requestor;

private slots:
    void handleVerifyCredentialsSuccessful();
    void handleVerifyCredentialsError(QNetworkReply::NetworkError error);
    void handleTweetError(QNetworkReply::NetworkError error);
    void handleTweetFinished();

};

#endif // TWITTERAPI_H
