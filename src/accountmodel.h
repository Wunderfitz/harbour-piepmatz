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
#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QNetworkConfigurationManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QSettings>
#include "o1twitter.h"
#include "o1requestor.h"
#include "twitterapi.h"
#include "locationinformation.h"
#include "dbusinterface.h"
#include "dbusadaptor.h"
//#include "wagnis/wagnis.h"

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
    Q_INVOKABLE QVariantList getOtherAccounts();
    Q_INVOKABLE void registerNewAccount();
    Q_INVOKABLE void removeCurrentAccount();
    Q_INVOKABLE void switchAccount(const QString &newAccountName);
    Q_INVOKABLE QString getImagePath();
    Q_INVOKABLE void setImagePath(const QString &imagePath);
    Q_INVOKABLE bool getUseEmoji();
    Q_INVOKABLE void setUseEmoji(const bool &useEmoji);
    Q_INVOKABLE bool getUseLoadingAnimations();
    Q_INVOKABLE void setUseLoadingAnimations(const bool &useAnimations);
    Q_INVOKABLE bool getUseSwipeNavigation();
    Q_INVOKABLE void setUseSwipeNavigation(const bool &useSwipeNavigation);
    Q_INVOKABLE bool getDisplayImageDescriptions();
    Q_INVOKABLE void setDisplayImageDescriptions(const bool &displayImageDescriptions);
    Q_INVOKABLE bool getUseSecretIdentity();
    Q_INVOKABLE void setUseSecretIdentity(const bool &useSecretIdentity);
    Q_INVOKABLE bool getUseOpenWith();
    Q_INVOKABLE void setUseOpenWith(const bool &useOpenWith);
    Q_INVOKABLE QString getSecretIdentityName();
    Q_INVOKABLE void setSecretIdentityName(const QString &secretIdentityName);
    Q_INVOKABLE QString getFontSize();
    Q_INVOKABLE void setFontSize(const QString &fontSize);
    Q_INVOKABLE bool isWiFi();
    Q_INVOKABLE QString getLinkPreviewMode();
    Q_INVOKABLE void setLinkPreviewMode(const QString &linkPreviewMode);
    Q_INVOKABLE bool hasSecretIdentity();
    Q_INVOKABLE void searchEmoji(const QString &queryString);

    TwitterApi *getTwitterApi();
    LocationInformation *getLocationInformation();
    DBusAdaptor *getDBusAdaptor();
    //Wagnis *getWagnis();

signals:
    void pinRequestError(const QString &errorMessage);
    void pinRequestSuccessful(const QString &url);
    void linkingFailed(const QString &errorMessage);
    void linkingSuccessful();
    void credentialsVerified();
    void verificationError(const QString &errorMessage);
    void imageStyleChanged();
    void swipeNavigationChanged();
    void accountSwitched();
    void fontSizeChanged(const QString &fontSize);
    void connectionTypeChanged(const bool &isWifi);
    void linkPreviewModeChanged(const QString &linkPreviewMode);
    void emojiSearchSuccessful(const QVariantList &result);

public slots:
    void handlePinRequestError(const QString &errorMessage);
    void handlePinRequestSuccessful(const QUrl &url);
    void handleLinkingFailed();
    void handleLinkingSucceeded();
    void handleVerifyCredentialsSuccessful(const QVariantMap &result);
    void handleVerifyCredentialsError(const QString &errorMessage);
    void handleNetworkConfigurationChanged(const QNetworkConfiguration &config);
    void handleEmojiSearchCompleted(const QString &queryString, const QVariantList &resultList);

private:
    QList<QVariantMap> availableAccounts;
    QNetworkConfigurationManager * const networkConfigurationManager;
    QString encryptionKey;
    O1Twitter * const o1;
    QNetworkAccessManager * const manager;
    O1Requestor *requestor;
    TwitterApi *twitterApi;
    LocationInformation * const locationInformation;
    //Wagnis * const wagnis;
    QSettings settings;
    QVariantList otherAccounts;
    bool secretIdentity;
    O1Requestor *secretIdentityRequestor = nullptr;
    DBusInterface *dbusInterface;
    EmojiSearchWorker emojiSearchWorker;

    void obtainEncryptionKey();
    void initializeEnvironment();
    void readOtherAccounts();
    void initializeSecretIdentity();
    void initializeOpenWith();
    void removeOpenWith();

};

#endif // ACCOUNTMODEL_H
