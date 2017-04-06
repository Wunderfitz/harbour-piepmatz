#include "accountmodel.h"
#include "o1twitterglobals.h"
#include "o0globals.h"
#include "o0settingsstore.h"
#include "o0requestparameter.h"

#include <QFile>
#include <QUuid>

AccountModel::AccountModel()
    : networkConfigurationManager(new QNetworkConfigurationManager(this))
    , o1(new O1Twitter(this))
    , manager(new QNetworkAccessManager(this))
{
    obtainEncryptionKey();

    O0SettingsStore *settings = new O0SettingsStore(encryptionKey);
    o1->setStore(settings);
    o1->setClientId(TWITTER_CLIENT_ID);
    o1->setClientSecret(TWITTER_CLIENT_SECRET);
    connect(o1, SIGNAL(pinRequestError(QString)), this, SLOT(handlePinRequestError(QString)));
    connect(o1, SIGNAL(pinRequestSuccessful(QUrl)), this, SLOT(handlePinRequestSuccessful(QUrl)));
    connect(o1, SIGNAL(linkingFailed()), this, SLOT(handleLinkingFailed()));
    connect(o1, SIGNAL(linkingSucceeded()), this, SLOT(handleLinkingSucceeded()));

    requestor = new O1Requestor(manager, o1, this);
    twitterApi = new TwitterApi(requestor, this);

    connect(twitterApi, &TwitterApi::verifyCredentialsError, this, &AccountModel::handleVerifyCredentialsError);
    connect(twitterApi, &TwitterApi::verifyCredentialsSuccessful, this, &AccountModel::handleVerifyCredentialsSuccessful);
}

QVariant AccountModel::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(availableAccounts.value(index.row()));
    }
    return QVariant();
}

void AccountModel::obtainPinUrl()
{
    if (networkConfigurationManager->isOnline()) {
        o1->obtainPinUrl();
    } else {
        emit pinRequestError("I'm sorry, your device is offline!");
    }
}

void AccountModel::enterPin(const QString &pin)
{
    qDebug() << "PIN entered: " + pin;
    if (networkConfigurationManager->isOnline()) {
        o1->verifyPin(pin);
    } else {
        emit linkingFailed("I'm sorry, your device is offline!");
    }
}

bool AccountModel::isLinked()
{
    return o1->linked();
}

void AccountModel::verifyCredentials()
{
    twitterApi->verifyCredentials();
}

void AccountModel::unlink()
{
    o1->unlink();
}

QVariantMap AccountModel::getCurrentAccount()
{
    qDebug() << "AccountModel::getCurrentAccount" << this->availableAccounts.value(0).value("screen_name").toString();
    return this->availableAccounts.value(0);
}

TwitterApi *AccountModel::getTwitterApi()
{
    return this->twitterApi;
}

void AccountModel::handlePinRequestError(const QString &errorMessage)
{
    emit pinRequestError("I'm sorry, there was an error: " + errorMessage);
}

void AccountModel::handlePinRequestSuccessful(const QUrl &url)
{
    emit pinRequestSuccessful(url.toString());
}

void AccountModel::handleLinkingFailed()
{
    qDebug() << "Linking failed! :(";
    emit linkingFailed("Linking error");
}

void AccountModel::handleLinkingSucceeded()
{
    qDebug() << "Linking successful! :)";
    emit linkingSuccessful();
}

void AccountModel::handleVerifyCredentialsSuccessful(const QVariantMap &result)
{
    beginResetModel();
    availableAccounts.clear();
    availableAccounts.append(result);
    endResetModel();
    emit credentialsVerified();
}

void AccountModel::handleVerifyCredentialsError(const QString &errorMessage)
{
    emit verificationError(errorMessage);
}

void AccountModel::obtainEncryptionKey()
{
    // We try to use the unique device ID as encryption key. If we can't determine this ID, a default key is used...
    // Unique device ID determination copied from the QtSystems module of the Qt Toolkit
    if (encryptionKey.isEmpty()) {
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/product_uuid"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 36) {
                encryptionKey = id;
            }
            file.close();
        }
    }
    if (encryptionKey.isEmpty()) {
        QFile file(QStringLiteral("/etc/machine-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                encryptionKey = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    if (encryptionKey.isEmpty()) {
        QFile file(QStringLiteral("/etc/unique-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                encryptionKey = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    if (encryptionKey.isEmpty()) {
        QFile file(QStringLiteral("/var/lib/dbus/machine-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                encryptionKey = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    QUuid uid(encryptionKey); //make sure this can be made into a valid QUUid
    if (uid.isNull()) {
         encryptionKey = QString(TWITTER_STORE_DEFAULT_ENCRYPTION_KEY);
    }
    qDebug() << "Using encryption key: " + encryptionKey;
}

int AccountModel::rowCount(const QModelIndex&) const {
    return availableAccounts.size();
}
