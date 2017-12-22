/*
    Copyright (C) 2017 Sebastian J. Wolf

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
#include "mentionsmodel.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>

const char SETTINGS_LAST_MENTION[] = "mentions/lastId";
const char SETTINGS_LAST_FOLLOWER_COUNT[] = "lastFollowerCount";
const char SETTINGS_LAST_KNOWN_FOLLOWERS[] = "lastKnownFollowers";
// We generate this amount of named follower entries maximum...
const int SETTINGS_MAX_NAMED_FOLLOWERS = 25;

MentionsModel::MentionsModel(TwitterApi *twitterApi, QString &screenName) : settings("harbour-piepmatz", "settings")
{
    this->twitterApi = twitterApi;
    this->screenName = screenName;
    resetStatus();
    initializeDatabase();

    connect(twitterApi, &TwitterApi::mentionsTimelineError, this, &MentionsModel::handleUpdateMentionsError);
    connect(twitterApi, &TwitterApi::mentionsTimelineSuccessful, this, &MentionsModel::handleUpdateMentionsSuccessful);
    connect(twitterApi, &TwitterApi::retweetTimelineError, this, &MentionsModel::handleUpdateRetweetsError);
    connect(twitterApi, &TwitterApi::retweetTimelineSuccessful, this, &MentionsModel::handleUpdateRetweetsSuccessful);
    connect(twitterApi, &TwitterApi::followersError, this, &MentionsModel::handleFollowersError);
    connect(twitterApi, &TwitterApi::followersSuccessful, this, &MentionsModel::handleFollowersSuccessful);
    connect(twitterApi, &TwitterApi::verifyCredentialsError, this, &MentionsModel::handleVerifyCredentialsError);
    connect(twitterApi, &TwitterApi::verifyCredentialsSuccessful, this, &MentionsModel::handleVerifyCredentialsSuccessful);

}

MentionsModel::~MentionsModel()
{
    qDebug() << "MentionsModel::destroy";
    database.close();
}

int MentionsModel::rowCount(const QModelIndex &) const
{
    return mentions.size();
}

QVariant MentionsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(mentions.value(index.row()));
    }
    return QVariant();
}

void MentionsModel::update()
{
    qDebug() << "MentionsModel::update";
    this->updateInProgress = true;
    twitterApi->mentionsTimeline();
    twitterApi->retweetTimeline();
    twitterApi->followers(this->screenName);
    twitterApi->verifyCredentials();
}

void MentionsModel::handleUpdateMentionsSuccessful(const QVariantList &result)
{
    qDebug() << "MentionsModel::handleUpdateMentionsSuccessful";

    if (updateInProgress) {
        this->mentionsUpdated = true;
        this->rawMentions = result;
        processRawMentions();
        handleUpdateSuccessful();
    }
}

void MentionsModel::handleUpdateMentionsError(const QString &errorMessage)
{
    qDebug() << "MentionsModel::handleUpdateMentionsError";
    handleUpdateError(errorMessage);
}

void MentionsModel::handleUpdateRetweetsSuccessful(const QVariantList &result)
{
    qDebug() << "MentionsModel::handleUpdateRetweetsSuccessful";
    if (updateInProgress) {
        this->retweetsUpdated = true;
        this->rawRetweets = result;
        handleUpdateSuccessful();
    }
}

void MentionsModel::handleUpdateRetweetsError(const QString &errorMessage)
{
    qDebug() << "MentionsModel::handleUpdateMentionsError";
    handleUpdateError(errorMessage);
}

void MentionsModel::handleFollowersSuccessful(const QVariantMap &result)
{
    qDebug() << "MentionsModel::handleFollowersSuccessful";
    if (updateInProgress) {
        this->followersUpdated = true;
        this->rawFollowers = result;
        processRawFollowers();
        handleUpdateSuccessful();
    }
}

void MentionsModel::handleFollowersError(const QString &errorMessage)
{
    qDebug() << "MentionsModel::handleFollowersError";
    handleUpdateError(errorMessage);
}

void MentionsModel::handleVerifyCredentialsSuccessful(const QVariantMap &result)
{
    qDebug() << "MentionsModel::handleVerifyCredentialsSuccessful";
    if (updateInProgress) {
        this->credentialsUpdated = true;
        this->myAccount = result;
        processCredentials();
        handleUpdateSuccessful();
    }
}

void MentionsModel::handleVerifyCredentialsError(const QString &errorMessage)
{
    qDebug() << "MentionsModel::handleVerifyCredentialsError";
    handleUpdateError(errorMessage);
}

void MentionsModel::handleUpdateError(const QString &errorMessage)
{
    qDebug() << "MentionsModel::handleUpdateError";
    resetStatus();
    emit updateMentionsError(errorMessage);
}

void MentionsModel::handleUpdateSuccessful()
{
    if (mentionsUpdated && retweetsUpdated && followersUpdated && credentialsUpdated) {
        if (this->newNamedFollowerCount > this->newGeneralFollowerCount) {
            emit newFollowersFound(this->newNamedFollowerCount);
        } else {
            if (this->newGeneralFollowerCount > 0) {
                emit newFollowersFound(this->newGeneralFollowerCount);
            }
        }
        qDebug() << "[MentionsModel] Updating all mentions...";
        resetStatus();
        // Do the merge and check work...
        beginResetModel();
        mentions.clear();
        mentions.append(rawMentions);
        endResetModel();
        emit updateMentionsFinished();
    }
}

void MentionsModel::resetStatus()
{
    qDebug() << "MentionsModel::resetStatus";
    this->newNamedFollowerCount = 0;
    this->newGeneralFollowerCount = 0;
    this->updateInProgress = false;
    this->mentionsUpdated = false;
    this->retweetsUpdated = false;
    this->followersUpdated = false;
    this->credentialsUpdated = false;
}

void MentionsModel::initializeDatabase()
{
    qDebug() << "MentionsModel::initializeDatabase";
    QString databaseDirectory = getDirectory(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz");
    QString databaseFilePath = databaseDirectory + "/cache.db";
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(databaseFilePath);
    if (database.open()) {
        qDebug() << "SQLite database " + databaseFilePath + " successfully opened";
        QStringList existingTables = database.tables();
        createFollowersTable(existingTables);
        createRetweetsTable(existingTables);
        createRetweetUsersTable(existingTables);
        createUsersTable(existingTables);
    } else {
        qDebug() << "Error opening SQLite database " + databaseFilePath;
    }
}

QString MentionsModel::getDirectory(const QString &directoryString)
{
    qDebug() << "MentionsModel::getDirectory";
    QString myDirectoryString = directoryString;
    QDir myDirectory(directoryString);
    if (!myDirectory.exists()) {
        qDebug() << "Creating directory " + directoryString;
        if (myDirectory.mkdir(directoryString)) {
            qDebug() << "Directory " + directoryString + " successfully created!";
        } else {
            qDebug() << "Error creating directory " + directoryString + "!";
            myDirectoryString = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }
    return myDirectoryString;
}

void MentionsModel::createFollowersTable(const QStringList &existingTables)
{
    if (!existingTables.contains("followers")) {
        QSqlQuery databaseQuery(database);
        databaseQuery.prepare("create table followers (id text primary key, name text, screen_name text, image_url text, sqltime TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
        if (databaseQuery.exec()) {
            qDebug() << "Followers table successfully created!";
        } else {
            qDebug() << "Error creating followers table!";
            return;
        }
    }
}

void MentionsModel::createRetweetsTable(const QStringList &existingTables)
{
    if (!existingTables.contains("retweets")) {
        QSqlQuery databaseQuery(database);
        databaseQuery.prepare("CREATE TABLE `retweets` (`id` TEXT, sqltime TIMESTAMP NOT NULL, PRIMARY KEY(id));");
        if (databaseQuery.exec()) {
            qDebug() << "Retweets table successfully created!";
        } else {
            qDebug() << "Error creating retweets table!";
            return;
        }
    }
}

void MentionsModel::createRetweetUsersTable(const QStringList &existingTables)
{
    if (!existingTables.contains("retweet_users")) {
        QSqlQuery databaseQuery(database);
        databaseQuery.prepare("CREATE TABLE `retweet_users` (`tweet_id` TEXT, `user_id` TEXT, sqltime TIMESTAMP NOT NULL, PRIMARY KEY(tweet_id, user_id));");
        if (databaseQuery.exec()) {
            qDebug() << "Retweet users table successfully created!";
        } else {
            qDebug() << "Error creating retweet users table!";
            return;
        }
    }
}

void MentionsModel::createUsersTable(const QStringList &existingTables)
{
    if (!existingTables.contains("users")) {
        QSqlQuery databaseQuery(database);
        databaseQuery.prepare("CREATE TABLE `users` (`id` TEXT,`name` TEXT, `screen_name` TEXT, `image_url` TEXT, PRIMARY KEY(id));");
        if (databaseQuery.exec()) {
            qDebug() << "Users table successfully created!";
        } else {
            qDebug() << "Error creating users table!";
            return;
        }
    }
}

void MentionsModel::processRawMentions()
{
    qDebug() << "MentionsModel::processRawMentions";
    if (!rawMentions.isEmpty()) {
        QString storedMentionId = settings.value(SETTINGS_LAST_MENTION).toString();
        if (!storedMentionId.isEmpty()) {
            QListIterator<QVariant> rawMentionsIterator(rawMentions);
            int newMentions = 0;
            while (rawMentionsIterator.hasNext()) {
                QVariantMap nextMention = rawMentionsIterator.next().toMap();
                if (nextMention.value("id_str").toString() != storedMentionId) {
                    newMentions++;
                } else {
                    break;
                }
            }
            if (newMentions > 0) {
                if (newMentions >= rawMentions.size()) {
                    emit newMentionsFound(0);
                } else {
                    emit newMentionsFound(newMentions);
                }
            }
        }
        settings.setValue(SETTINGS_LAST_MENTION, rawMentions.first().toMap().value("id_str").toString());
    }
}

void MentionsModel::processCredentials()
{
    qDebug() << "MentionsModel::processCredentials";
    int lastFollowerCount = settings.value(SETTINGS_LAST_FOLLOWER_COUNT).toInt();
    int currentFollowerCount = myAccount.value("followers_count").toInt();
    if (lastFollowerCount > 0 && lastFollowerCount < currentFollowerCount) {
        this->newGeneralFollowerCount = currentFollowerCount - lastFollowerCount;
    }
    settings.setValue(SETTINGS_LAST_FOLLOWER_COUNT, currentFollowerCount);
}

void MentionsModel::processRawFollowers()
{
    qDebug() << "MentionsModel::processRawFollowers";

    QSqlQuery databaseQuery(database);
    databaseQuery.prepare("insert into followers values((:id),(:name),(:screen_name),(:image_url), CURRENT_TIMESTAMP)");

    QVariantList currentFollowers = rawFollowers.value("users").toList();
    QStringList lastKnownFollowers = settings.value(SETTINGS_LAST_KNOWN_FOLLOWERS).toStringList();
    if (!lastKnownFollowers.isEmpty()) {
        QListIterator<QVariant> currentFollowersIterator(currentFollowers);
        int i = 0;
        while (currentFollowersIterator.hasNext()) {
            QVariantMap currentFollower = currentFollowersIterator.next().toMap();
            if (lastKnownFollowers.contains(currentFollower.value("screen_name").toString())) {
                // This means that we found the current follower in our stored list,
                // The index represents the count of new followers
                break;
            } else {
                databaseQuery.bindValue(":id", currentFollower.value("id_str").toString());
                databaseQuery.bindValue(":name", currentFollower.value("name").toString());
                databaseQuery.bindValue(":screen_name", currentFollower.value("screen_name").toString());
                databaseQuery.bindValue(":image_url", currentFollower.value("profile_image_url_https").toString());
                if (databaseQuery.exec()) {
                    qDebug() << "Successfully wrote new follower " + currentFollower.value("screen_name").toString() + " to database.";
                } else {
                    qDebug() << "Error writing new follower " + currentFollower.value("screen_name").toString() + ": " + databaseQuery.lastError().text();
                }
            }
            i++;
        }
        this->newNamedFollowerCount = i;
    }

    QStringList currentLastFollowers;
    for (int j = 0; j < SETTINGS_MAX_NAMED_FOLLOWERS; j++) {
        if (currentFollowers.size() < j) {
            break;
        }
        QString lastFollower = currentFollowers.at(j).toMap().value("screen_name").toString();
        currentLastFollowers.append(lastFollower);
    }
    settings.setValue(SETTINGS_LAST_KNOWN_FOLLOWERS, currentLastFollowers);

}

