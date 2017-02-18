#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QNetworkConfigurationManager>
#include "account.h"
#include "o1twitter.h"
#include "o1requestor.h"

class AccountModel : public QAbstractListModel
{
    Q_OBJECT
public:
    AccountModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void obtainPinUrl();
    Q_INVOKABLE void enterPin(const QString &pin);

signals:
    void pinRequestError(const QString &errorMessage);
    void pinRequestSuccessful(const QString &url);

public slots:
    void handlePinRequestError(const QString &errorMessage);
    void handlePinRequestSuccessful(const QUrl &url);
    void handleLinkingFailed();
    void handleLinkingSucceeded();

private:
    QList<Account*> availableAccounts;
    QNetworkConfigurationManager *networkConfigurationManager;
    QString encryptionKey;
    O1Twitter *o1;

    void obtainEncryptionKey();
};

#endif // ACCOUNTMODEL_H
