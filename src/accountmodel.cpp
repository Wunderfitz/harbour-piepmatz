#include "accountmodel.h"
#include "o1twitterglobals.h"

AccountModel::AccountModel()
{
    o1 = new O1Twitter(this);
    o1->setClientId(TWITTER_CLIENT_ID);
    o1->setClientSecret(TWITTER_CLIENT_SECRET);
    connect(o1, SIGNAL(pinRequestError(QString)), this, SLOT(handlePinRequestError(QString)));
    connect(o1, SIGNAL(pinRequestSuccessful(QUrl)), this, SLOT(handlePinRequestSuccessful(QUrl)));
}
QVariant AccountModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        QMap<QString,QVariant> resultMap;
        Account* account = availableAccounts.value(index.row());
        //resultMap.insert("id", QVariant(availableAccounts->getId()));
        return QVariant(resultMap);
    }
    return QVariant();
}

void AccountModel::obtainPinUrl()
{
    o1->obtainPinUrl();
}

void AccountModel::handlePinRequestError(const QString &errorMessage)
{
    emit pinRequestError(errorMessage);
}

void AccountModel::handlePinRequestSuccessful(const QUrl &url)
{
    emit pinRequestSuccessful(url.toString());
}

int AccountModel::rowCount(const QModelIndex&) const {
    return availableAccounts.size();
}
