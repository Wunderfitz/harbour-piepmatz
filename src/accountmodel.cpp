#include "accountmodel.h"
#include "o1twitterglobals.h"
#include "o0settingsstore.h"

#include <QFile>
#include <QUuid>

AccountModel::AccountModel()
{
    networkConfigurationManager = new QNetworkConfigurationManager(this);
    obtainEncryptionKey();

    o1 = new O1Twitter(this);
    O0SettingsStore *settings = new O0SettingsStore(encryptionKey);
    o1->setStore(settings);
    o1->setClientId(TWITTER_CLIENT_ID);
    o1->setClientSecret(TWITTER_CLIENT_SECRET);
    connect(o1, SIGNAL(pinRequestError(QString)), this, SLOT(handlePinRequestError(QString)));
    connect(o1, SIGNAL(pinRequestSuccessful(QUrl)), this, SLOT(handlePinRequestSuccessful(QUrl)));
    connect(o1, SIGNAL(linkingFailed()), this, SLOT(handleLinkingFailed()));
    connect(o1, SIGNAL(linkingSucceeded()), this, SLOT(handleLinkingSucceeded()));
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
    if (networkConfigurationManager->isOnline()) {
        o1->obtainPinUrl();
    } else {
        emit pinRequestError("I'm sorry, your device is offline!");
    }
}

void AccountModel::enterPin(const QString &pin)
{
    qDebug() << "PIN entered: " + pin;
    o1->verifyPin(pin);
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
}

void AccountModel::handleLinkingSucceeded()
{
    qDebug() << "Linking successful! :)";
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
