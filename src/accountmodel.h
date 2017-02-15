#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
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

signals:
    void pinRequestError(const QString &errorMessage);
    void pinRequestSuccessful(const QString &url);

public slots:
    void handlePinRequestError(const QString &errorMessage);
    void handlePinRequestSuccessful(const QUrl &url);

private:
    QList<Account*> availableAccounts;
    O1Twitter *o1;
};

#endif // ACCOUNTMODEL_H
