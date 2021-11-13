/*
    Copyright (C) 2017-20 Sebastian J. Wolf

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
#include <QNetworkConfiguration>
#include <QTextStream>
#include <QProcess>
#include <QSysInfo>

const char SETTINGS_IMAGE_PATH[] = "settings/imagePath";
const char SETTINGS_USE_EMOJI[] = "settings/useEmojis";
const char SETTINGS_USE_LOADING_ANIMATIONS[] = "settings/useLoadingAnimations";
const char SETTINGS_USE_SWIPE_NAVIGATION[] = "settings/useSwipeNavigation";
const char SETTINGS_USE_SECRET_IDENTITY[] = "settings/useSecretIdentity";
const char SETTINGS_USE_OPEN_WITH[] = "settings/useOpenWith";
const char SETTINGS_SECRET_IDENTITY_NAME[] = "settings/secretIdentityName";
const char SETTINGS_DISPLAY_IMAGE_DESCRIPTIONS[] = "settings/displayImageDescriptions";
const char SETTINGS_FONT_SIZE[] = "settings/fontSize";
const char SETTINGS_LINK_PREVIEW_MODE[] = "settings/linkPreviewMode";

AccountModel::AccountModel()
    : networkConfigurationManager(new QNetworkConfigurationManager(this))
    , o1(new O1Twitter(this))
    , manager(new QNetworkAccessManager(this))
    , locationInformation(new LocationInformation(this))
    //, wagnis(new Wagnis(manager, "harbour-piepmatz", "1.4", this))
    , settings("harbour-piepmatz", "settings")
{
    obtainEncryptionKey();
    initializeEnvironment();
    connect(&emojiSearchWorker, SIGNAL(searchCompleted(QString, QVariantList)), this, SLOT(handleEmojiSearchCompleted(QString, QVariantList)));
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

    readOtherAccounts();
    requestor = new O1Requestor(manager, o1, this);
    this->initializeSecretIdentity();
    if (this->getUseOpenWith()) {
        this->initializeOpenWith();
    } else {
        this->removeOpenWith();
    }
    this->dbusInterface = new DBusInterface(this);

    //twitterApi = new TwitterApi(requestor, manager, wagnis, this);
    twitterApi = new TwitterApi(requestor, manager, secretIdentityRequestor, this);

    connect(twitterApi, &TwitterApi::verifyCredentialsError, this, &AccountModel::handleVerifyCredentialsError);
    connect(twitterApi, &TwitterApi::verifyCredentialsSuccessful, this, &AccountModel::handleVerifyCredentialsSuccessful);

    connect(networkConfigurationManager, &QNetworkConfigurationManager::configurationChanged, this, &AccountModel::handleNetworkConfigurationChanged);
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
    //qDebug() << "AccountModel::getCurrentAccount" << this->availableAccounts.value(0).value("screen_name").toString();
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

bool AccountModel::getUseSecretIdentity()
{
    return settings.value(SETTINGS_USE_SECRET_IDENTITY, false).toBool();
}

void AccountModel::setUseSecretIdentity(const bool &useSecretIdentity)
{
    settings.setValue(SETTINGS_USE_SECRET_IDENTITY, useSecretIdentity);
}

bool AccountModel::getUseOpenWith()
{
    return settings.value(SETTINGS_USE_OPEN_WITH, true).toBool();
}

void AccountModel::setUseOpenWith(const bool &useOpenWith)
{
    settings.setValue(SETTINGS_USE_OPEN_WITH, useOpenWith);
    if (useOpenWith) {
        this->initializeOpenWith();
    } else {
        this->removeOpenWith();
    }
}

QString AccountModel::getSecretIdentityName()
{
    return settings.value(SETTINGS_SECRET_IDENTITY_NAME, "").toString();
}

void AccountModel::setSecretIdentityName(const QString &secretIdentityName)
{
    settings.setValue(SETTINGS_SECRET_IDENTITY_NAME, secretIdentityName);
}

QString AccountModel::getFontSize()
{
    return settings.value(SETTINGS_FONT_SIZE, "piepmatz").toString();
}

void AccountModel::setFontSize(const QString &fontSize)
{
    settings.setValue(SETTINGS_FONT_SIZE, fontSize);
    emit fontSizeChanged(fontSize);
}

bool AccountModel::isWiFi()
{
    QList<QNetworkConfiguration> activeConfigurations = networkConfigurationManager->allConfigurations(QNetworkConfiguration::Active);
    QListIterator<QNetworkConfiguration> configurationIterator(activeConfigurations);
    while (configurationIterator.hasNext()) {
        QNetworkConfiguration activeConfiguration = configurationIterator.next();
        if (activeConfiguration.bearerType() == QNetworkConfiguration::BearerWLAN || activeConfiguration.bearerType() == QNetworkConfiguration::BearerEthernet) {
            qDebug() << "[AccountModel] WiFi ON!";
            return true;
        }
    }
    qDebug() << "[AccountModel] WiFi OFF!";
    return false;
}

QString AccountModel::getLinkPreviewMode()
{
    return settings.value(SETTINGS_LINK_PREVIEW_MODE, "always").toString();
}

void AccountModel::setLinkPreviewMode(const QString &linkPreviewMode)
{
    settings.setValue(SETTINGS_LINK_PREVIEW_MODE, linkPreviewMode);
    emit linkPreviewModeChanged(linkPreviewMode);
}

bool AccountModel::hasSecretIdentity()
{
    return this->secretIdentity;
}

TwitterApi *AccountModel::getTwitterApi()
{
    return this->twitterApi;
}

LocationInformation *AccountModel::getLocationInformation()
{
    return this->locationInformation;
}

DBusAdaptor *AccountModel::getDBusAdaptor()
{
    return this->dbusInterface->getDBusAdaptor();
}

//Wagnis *AccountModel::getWagnis()
//{
//    return this->wagnis;
//}

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

void AccountModel::handleNetworkConfigurationChanged(const QNetworkConfiguration &config)
{
    qDebug() << "Network configuration changed: " << config.bearerTypeName() << config.state();
    emit connectionTypeChanged(this->isWiFi());
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

void AccountModel::initializeSecretIdentity()
{
    qDebug() << "AccountModel::initializeSecretIdentity";
    secretIdentity = false;
    secretIdentityRequestor = nullptr;
    if (this->getUseSecretIdentity()) {
        QString secretIdentity = this->getSecretIdentityName();
        qDebug() << "Using secret identity " << secretIdentity;
        QString accountFileName = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/harbour-piepmatz-" + secretIdentity + ".conf";
        qDebug() << "Using file name " << accountFileName;
        QSettings *secretIdentitySettings = new QSettings(accountFileName, QSettings::IniFormat, this);
        O0SettingsStore *secretIdentitySettingsStore = new O0SettingsStore(secretIdentitySettings, encryptionKey, this);
        O1Twitter *o1SecretIdentity = new O1Twitter(this);
        o1SecretIdentity->setStore(secretIdentitySettingsStore);
        o1SecretIdentity->setClientId(TWITTER_CLIENT_ID);
        o1SecretIdentity->setClientSecret(TWITTER_CLIENT_SECRET);
        if (o1SecretIdentity->linked()) {
            qDebug() << "Secret identity successfully initialized!";
            secretIdentityRequestor = new O1Requestor(manager, o1SecretIdentity, this);
            secretIdentity = true;
        } else {
            qDebug() << "ERROR initializing secret identity!";
        }
    }
}

void AccountModel::initializeOpenWith()
{
    qDebug() << "AccountModel::initializeOpenWith";

    const QStringList sailfishOSVersion = QSysInfo::productVersion().split(".");
    int sailfishOSMajorVersion = sailfishOSVersion.value(0).toInt();
    int sailfishOSMinorVersion = sailfishOSVersion.value(1).toInt();

    const QString applicationsLocation(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
    const QString openUrlFilePath(applicationsLocation + "/open-url.desktop");
    if (sailfishOSMajorVersion < 4 || ( sailfishOSMajorVersion == 4 && sailfishOSMinorVersion < 2 )) {
        if (QFile::exists(openUrlFilePath)) {
            qDebug() << "Standard open URL file exists, good!";
        } else {
            qDebug() << "Copying standard open URL file to " << openUrlFilePath;
            QFile::copy("/usr/share/applications/open-url.desktop", openUrlFilePath);
            QProcess::startDetached("update-desktop-database " + applicationsLocation);
        }
    } else {
        if (QFile::exists(openUrlFilePath)) {
            qDebug() << "Old open URL file exists, that needs to go away...!";
            QFile::remove(openUrlFilePath);
            QProcess::startDetached("update-desktop-database " + applicationsLocation);
        }
        // Something special for Verla...
        if (sailfishOSMajorVersion == 4 && sailfishOSMinorVersion == 2) {
            qDebug() << "Creating open URL file at " << openUrlFilePath;
            QFile openUrlFile(openUrlFilePath);
            if (openUrlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream fileOut(&openUrlFile);
                fileOut.setCodec("UTF-8");
                fileOut << QString("[Desktop Entry]").toUtf8() << "\n";
                fileOut << QString("Type=Application").toUtf8() << "\n";
                fileOut << QString("Name=Browser").toUtf8() << "\n";
                fileOut << QString("Icon=icon-launcher-browser").toUtf8() << "\n";
                fileOut << QString("NoDisplay=true").toUtf8() << "\n";
                fileOut << QString("X-MeeGo-Logical-Id=sailfish-browser-ap-name").toUtf8() << "\n";
                fileOut << QString("X-MeeGo-Translation-Catalog=sailfish-browser").toUtf8() << "\n";
                fileOut << QString("MimeType=text/html;x-scheme-handler/http;x-scheme-handler/https;").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Service=org.sailfishos.browser.ui").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Object-Path=/ui").toUtf8() << "\n";
                fileOut << QString("X-Maemo-Method=org.sailfishos.browser.ui.openUrl").toUtf8() << "\n";
                fileOut.flush();
                openUrlFile.close();
                QProcess::startDetached("update-desktop-database " + applicationsLocation);
            }
        }
    }

    QString desktopFilePath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/harbour-piepmatz-open-url.desktop";
    QFile desktopFile(desktopFilePath);
    if (desktopFile.exists()) {
        qDebug() << "Piepmatz open-with file existing, removing...";
        desktopFile.remove();
        QProcess::startDetached("update-desktop-database " + applicationsLocation);
    }
    qDebug() << "Creating Open-With file at " << desktopFile.fileName();
    if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream fileOut(&desktopFile);
        fileOut.setCodec("UTF-8");
        fileOut << QString("[Desktop Entry]").toUtf8() << "\n";
        fileOut << QString("Type=Application").toUtf8() << "\n";
        fileOut << QString("Name=Piepmatz").toUtf8() << "\n";
        fileOut << QString("Icon=harbour-piepmatz").toUtf8() << "\n";
        fileOut << QString("NotShowIn=X-MeeGo;").toUtf8() << "\n";
        if (sailfishOSMajorVersion < 4 || ( sailfishOSMajorVersion == 4 && sailfishOSMinorVersion < 1 )) {
            fileOut << QString("MimeType=text/html;x-scheme-handler/http;x-scheme-handler/https;").toUtf8() << "\n";
        } else {
            fileOut << QString("MimeType=x-url-handler/twitter.com;x-url-handler/mobile.twitter.com;x-url-handler/m.twitter.com;").toUtf8() << "\n";
        }
        fileOut << QString("X-Maemo-Service=de.ygriega.piepmatz").toUtf8() << "\n";
        fileOut << QString("X-Maemo-Method=de.ygriega.piepmatz.openUrl").toUtf8() << "\n";
        fileOut << QString("Hidden=true;").toUtf8() << "\n";
        fileOut.flush();
        desktopFile.close();
        QProcess::startDetached("update-desktop-database " + QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
    }
    QString dbusPathName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/dbus-1/services";
    QDir dbusPath(dbusPathName);
    if (!dbusPath.exists()) {
        qDebug() << "Creating D-Bus directory " << dbusPathName;
        dbusPath.mkpath(dbusPathName);
    }
    QString dbusServiceFileName = dbusPathName + "/de.ygriega.piepmatz.service";
    QFile dbusServiceFile(dbusServiceFileName);
    if (!dbusServiceFile.exists()) {
        qDebug() << "Creating D-Bus service file at " << dbusServiceFile.fileName();
        if (dbusServiceFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream fileOut(&dbusServiceFile);
            fileOut.setCodec("UTF-8");
            fileOut << QString("[D-BUS Service]").toUtf8() << "\n";
            fileOut << QString("Name=de.ygriega.piepmatz").toUtf8() << "\n";
            fileOut << QString("Exec=/usr/bin/invoker -s --type=silica-qt5 /usr/bin/harbour-piepmatz").toUtf8() << "\n";
            fileOut.flush();
            dbusServiceFile.close();
        }
    }
}

void AccountModel::removeOpenWith()
{
    qDebug() << "AccountModel::removeOpenWith";
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/harbour-piepmatz-open-url.desktop");
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/dbus-1/services/de.ygriega.piepmatz.service");
    QProcess::startDetached("update-desktop-database " + QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation));
}

int AccountModel::rowCount(const QModelIndex&) const {
    return availableAccounts.size();
}

void AccountModel::handleEmojiSearchCompleted(const QString &queryString, const QVariantList &resultList)
{
    qDebug() << "TwitterApi::handleEmojiSearchCompleted" << queryString;
    emit emojiSearchSuccessful(resultList);

}

void AccountModel::searchEmoji(const QString &queryString)
{
    qDebug() << "TwitterApi::searchEmoji" << queryString;
    while (this->emojiSearchWorker.isRunning()) {
        this->emojiSearchWorker.requestInterruption();
    }
    this->emojiSearchWorker.setParameters(queryString);
    this->emojiSearchWorker.start();
}
