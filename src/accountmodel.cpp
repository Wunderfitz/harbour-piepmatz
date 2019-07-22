/*
    Copyright (C) 2017-19 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
#include "accountmodel.h"
#include "o1twitterglobals.h"
#include "o0globals.h"
#include "o0settingsstore.h"
#include "o0requestparameter.h"

#include <QFile>
#include <QUuid>
#include <QDir>
#include <QStandardPaths>

const char SETTINGS_IMAGE_PATH[] = "settings/imagePath";
const char SETTINGS_USE_EMOJI[] = "settings/useEmojis";
const char SETTINGS_USE_LOADING_ANIMATIONS[] = "settings/useLoadingAnimations";
const char SETTINGS_USE_SWIPE_NAVIGATION[] = "settings/useSwipeNavigation";
const char SETTINGS_DISPLAY_IMAGE_DESCRIPTIONS[] = "settings/displayImageDescriptions";

AccountModel::AccountModel()
    : networkConfigurationManager(new QNetworkConfigurationManager(this))
    , o1(new O1Twitter(this))
    , manager(new QNetworkAccessManager(this))
    , locationInformation(new LocationInformation(this))
    , wagnis(new Wagnis(manager, "harbour-piepmatz", "1.4", this))
    , settings("harbour-piepmatz", "settings")
{
    obtainEncryptionKey();
    initializeEnvironment();
}

QVariant AccountModel::data(const QModelIndex &index, int role) const {
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(availableAccounts.value(index.row()));
    }
    return QVariant();
}

void AccountModel::initializeEnvironment()
{
    O0SettingsStore *settings = new O0SettingsStore(encryptionKey);
    o1->setStore(settings);
    o1->setClientId(TWITTER_CLIENT_ID);
    o1->setClientSecret(TWITTER_CLIENT_SECRET);
    connect(o1, &O1Twitter::pinRequestError, this, &AccountModel::handlePinRequestError);
    connect(o1, &O1Twitter::pinRequestSuccessful, this, &AccountModel::handlePinRequestSuccessful);
    connect(o1, &O1Twitter::linkingFailed, this, &AccountModel::handleLinkingFailed);
    connect(o1, &O1Twitter::linkingSucceeded, this, &AccountModel::handleLinkingSucceeded);

    requestor = new O1Requestor(manager, o1, this);
    twitterApi = new TwitterApi(requestor, manager, wagnis, this);
    readOtherAccounts();

    connect(twitterApi, &TwitterApi::verifyCredentialsError, this, &AccountModel::handleVerifyCredentialsError);
    connect(twitterApi, &TwitterApi::verifyCredentialsSuccessful, this, &AccountModel::handleVerifyCredentialsSuccessful);
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

QVariantList AccountModel::getOtherAccounts()
{
    return this->otherAccounts;
}

void AccountModel::registerNewAccount()
{
    qDebug() << "AccountModel::registerNewAccount";
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz-" + this->availableAccounts.value(0).value("screen_name").toString() + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings-" + this->availableAccounts.value(0).value("screen_name").toString() + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache.db", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache-" + this->availableAccounts.value(0).value("screen_name").toString() + ".db");
    emit accountSwitched();
    this->initializeEnvironment();
}

void AccountModel::removeCurrentAccount()
{
    qDebug() << "AccountModel::removeCurrentAccount";
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz.conf");
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf");
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache.db");
    if (!this->otherAccounts.isEmpty()) {
        // We also move to another account...
        QString newAccountName = this->otherAccounts.value(0).toString();
        QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz-" + newAccountName + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz.conf");
        QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings-" + newAccountName + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf");
        QFile::rename(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache-" + newAccountName + ".db", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache.db");
    }
    emit accountSwitched();
    this->initializeEnvironment();
}

void AccountModel::switchAccount(const QString &newAccountName)
{
    qDebug() << "AccountModel::switchAccount" << newAccountName;
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz-" + this->availableAccounts.value(0).value("screen_name").toString() + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings-" + this->availableAccounts.value(0).value("screen_name").toString() + ".conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache.db", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache-" + this->availableAccounts.value(0).value("screen_name").toString() + ".db");

    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz-" + newAccountName + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz.conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings-" + newAccountName + ".conf", QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf");
    QFile::rename(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache-" + newAccountName + ".db", QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz/cache.db");
    emit accountSwitched();
    this->initializeEnvironment();

}

QString AccountModel::getImagePath()
{
    return settings.value(SETTINGS_IMAGE_PATH, QString()).toString();
}

void AccountModel::setImagePath(const QString &imagePath)
{
    settings.setValue(SETTINGS_IMAGE_PATH, imagePath);
    emit imageStyleChanged();
}

bool AccountModel::getUseEmoji()
{
    return settings.value(SETTINGS_USE_EMOJI, true).toBool();
}

void AccountModel::setUseEmoji(const bool &useEmoji)
{
    settings.setValue(SETTINGS_USE_EMOJI, useEmoji);
}

bool AccountModel::getUseLoadingAnimations()
{
    return settings.value(SETTINGS_USE_LOADING_ANIMATIONS, true).toBool();
}

void AccountModel::setUseLoadingAnimations(const bool &useAnimations)
{
    settings.setValue(SETTINGS_USE_LOADING_ANIMATIONS, useAnimations);
}

bool AccountModel::getUseSwipeNavigation()
{
    return settings.value(SETTINGS_USE_SWIPE_NAVIGATION, true).toBool();
}

void AccountModel::setUseSwipeNavigation(const bool &useSwipeNavigation)
{
    settings.setValue(SETTINGS_USE_SWIPE_NAVIGATION, useSwipeNavigation);
    emit swipeNavigationChanged();
}

bool AccountModel::getDisplayImageDescriptions()
{
    return settings.value(SETTINGS_DISPLAY_IMAGE_DESCRIPTIONS, true).toBool();
}

void AccountModel::setDisplayImageDescriptions(const bool &displayImageDescriptions)
{
    settings.setValue(SETTINGS_DISPLAY_IMAGE_DESCRIPTIONS, displayImageDescriptions);
}

TwitterApi *AccountModel::getTwitterApi()
{
    return this->twitterApi;
}

LocationInformation *AccountModel::getLocationInformation()
{
    return this->locationInformation;
}

Wagnis *AccountModel::getWagnis()
{
    return this->wagnis;
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
    readOtherAccounts();
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

void AccountModel::readOtherAccounts()
{
    qDebug() << "AccountModel::readOtherAccounts";
    this->otherAccounts.clear();
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz";
    QDir configDirectory(configPath);
    QStringList nameFilter("*.conf");
    QStringList accountFiles = configDirectory.entryList(nameFilter);
    QStringListIterator accountFilesIterator(accountFiles);
    while (accountFilesIterator.hasNext()) {
        QString accountFileName = accountFilesIterator.next();
        QRegExp accountFileMatcher("harbour\\-piepmatz\\-([\\w]+)\\.conf");
        if (accountFileMatcher.indexIn(accountFileName) != -1) {
            qDebug() << "Found other account: " + accountFileMatcher.cap(1);
            this->otherAccounts.append(accountFileMatcher.cap(1));
        }
    }
}

int AccountModel::rowCount(const QModelIndex&) const {
    return availableAccounts.size();
}
