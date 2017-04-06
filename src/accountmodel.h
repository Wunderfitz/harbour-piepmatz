#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QNetworkConfigurationManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include "o1twitter.h"
#include "o1requestor.h"
#include "twitterapi.h"

class AccountModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AccountModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void obtainPinUrl();
    Q_INVOKABLE void enterPin(const QString &pin);
    Q_INVOKABLE bool isLinked();
    Q_INVOKABLE void verifyCredentials();
    Q_INVOKABLE void unlink();
    Q_INVOKABLE QVariantMap getCurrentAccount();

    TwitterApi *getTwitterApi();

signals:
    void pinRequestError(const QString &errorMessage);
    void pinRequestSuccessful(const QString &url);
    void linkingFailed(const QString &errorMessage);
    void linkingSuccessful();
    void credentialsVerified();
    void verificationError(const QString &errorMessage);

public slots:
    void handlePinRequestError(const QString &errorMessage);
    void handlePinRequestSuccessful(const QUrl &url);
    void handleLinkingFailed();
    void handleLinkingSucceeded();
    void handleVerifyCredentialsSuccessful(const QVariantMap &result);
    void handleVerifyCredentialsError(const QString &errorMessage);

private:
    QList<QVariantMap> availableAccounts;
    QNetworkConfigurationManager * const networkConfigurationManager;
    QString encryptionKey;
    O1Twitter * const o1;
    QNetworkAccessManager * const manager;
    O1Requestor *requestor;
    TwitterApi *twitterApi;

    void obtainEncryptionKey();

};

#endif // ACCOUNTMODEL_H
