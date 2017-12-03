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

const char SETTINGS_LAST_MENTION[] = "mentions/lastId";

MentionsModel::MentionsModel(TwitterApi *twitterApi, QString &screenName) : settings("harbour-piepmatz", "settings")
{
    this->twitterApi = twitterApi;
    this->screenName = screenName;
    resetStatus();

    connect(twitterApi, &TwitterApi::mentionsTimelineError, this, &MentionsModel::handleUpdateMentionsError);
    connect(twitterApi, &TwitterApi::mentionsTimelineSuccessful, this, &MentionsModel::handleUpdateMentionsSuccessful);
    connect(twitterApi, &TwitterApi::retweetTimelineError, this, &MentionsModel::handleUpdateRetweetsError);
    connect(twitterApi, &TwitterApi::retweetTimelineSuccessful, this, &MentionsModel::handleUpdateRetweetsSuccessful);
    connect(twitterApi, &TwitterApi::followersError, this, &MentionsModel::handleFollowersError);
    connect(twitterApi, &TwitterApi::followersSuccessful, this, &MentionsModel::handleFollowersSuccessful);
    connect(twitterApi, &TwitterApi::verifyCredentialsError, this, &MentionsModel::handleVerifyCredentialsError);
    connect(twitterApi, &TwitterApi::verifyCredentialsSuccessful, this, &MentionsModel::handleVerifyCredentialsSuccessful);

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
        qDebug() << "[MentionsModel] Updating all mentions...";
        resetStatus();
        // Do the merge and check work...
        beginResetModel();
        mentions.clear();
        mentions.append(rawMentions);
        if (!rawMentions.isEmpty()) {
            QString storedMentionId = settings.value(SETTINGS_LAST_MENTION).toString();
            QString lastMentionId = rawMentions.first().toMap().value("id_str").toString();
            if (!storedMentionId.isEmpty() && storedMentionId != lastMentionId) {
                emit newMentionsFound();
            }
            settings.setValue(SETTINGS_LAST_MENTION, lastMentionId);
        }
        endResetModel();
        emit updateMentionsFinished();
    }
}

void MentionsModel::resetStatus()
{
    qDebug() << "MentionsModel::resetStatus";
    this->updateInProgress = false;
    this->mentionsUpdated = false;
    this->retweetsUpdated = false;
    this->followersUpdated = false;
    this->credentialsUpdated = false;
}

