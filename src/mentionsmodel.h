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
#ifndef MENTIONSMODEL_H
#define MENTIONSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "twitterapi.h"
#include "accountmodel.h"

class MentionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MentionsModel(TwitterApi *twitterApi, AccountModel &accountModel);
    ~MentionsModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

signals:
    void updateMentionsFinished();
    void updateMentionsError(const QString &errorMessage);
    void newMentionsFound(const int newMentions);
    void newFollowersFound(const int newFollowers);
    void newRetweetsFound(const int newRetweets);

public slots:
    void handleUpdateMentionsSuccessful(const QVariantList &result);
    void handleUpdateMentionsError(const QString &errorMessage);
    void handleUpdateRetweetsSuccessful(const QVariantList &result);
    void handleUpdateRetweetsError(const QString &errorMessage);
    void handleRetweetsForSuccessful(const QString &statusId, const QVariantList &result);
    void handleRetweetsForError(const QString &statusId, const QString &errorMessage);
    void handleFollowersSuccessful(const QVariantMap &result);
    void handleFollowersError(const QString &errorMessage);
    void handleVerifyCredentialsSuccessful(const QVariantMap &result);
    void handleVerifyCredentialsError(const QString &errorMessage);
    void handleAccountSwitched();

private:

    void handleUpdateError(const QString &errorMessage);
    void handleUpdateSuccessful();
    void resetStatus();

    void initializeDatabase();
    QString getDirectory(const QString &directoryString);
    void createFollowersTable(const QStringList &existingTables);

    void processRawMentions();
    void processCredentials();
    void processRawFollowers();
    void processRawRetweets();
    void getFollowersFromDatabase();

    QVariantList mentions;
    QSettings settings;
    QSqlDatabase database;
    TwitterApi *twitterApi;
    AccountModel *accountModel;

    QVariantList followersFromDatabase;

    QString screenName;
    bool updateInProgress;
    bool mentionsUpdated;
    QVariantList rawMentions;
    bool retweetsUpdated;
    QVariantList rawRetweets;
    bool followersUpdated;
    QVariantMap rawFollowers;
    bool credentialsUpdated;
    QVariantMap myAccount;
    int newNamedFollowerCount;
    int newGeneralFollowerCount;
    int retweetsCount;
    int retweetsForCompletedCount;
};

#endif // MENTIONSMODEL_H
