#ifndef O1TWITTER_H
#define O1TWITTER_H

#include "o0export.h"
#include "o1.h"

/// Twitter OAuth 1.0 client
class O0_EXPORT O1Twitter: public O1 {
    Q_OBJECT

public:
    explicit O1Twitter(QObject *parent = 0): O1(parent) {
        setRequestTokenUrl(QUrl("https://api.twitter.com/oauth/request_token"));
        setAuthorizeUrl(QUrl("https://api.twitter.com/oauth/authenticate"));
        setAccessTokenUrl(QUrl("https://api.twitter.com/oauth/access_token"));
    }
    void obtainPinUrl();

signals:
    void pinRequestError(const QString &errorMessage);
    void pinRequestSuccessful(const QUrl &url);

private slots:
    virtual void onPinRequestError(QNetworkReply::NetworkError error);
    virtual void onPinRequestFinished();
};

#endif // O1TWITTER_H
