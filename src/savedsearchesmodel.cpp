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
#include "savedsearchesmodel.h"

SavedSearchesModel::SavedSearchesModel(TwitterApi *twitterApi)
    : updateInProgress(false)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::savedSearchesError, this, &SavedSearchesModel::handleSavedSearchesError);
    connect(twitterApi, &TwitterApi::savedSearchesSuccessful, this, &SavedSearchesModel::handleSavedSearchesSuccessful);
    connect(twitterApi, &TwitterApi::saveSearchError, this, &SavedSearchesModel::handleSaveSearchError);
    connect(twitterApi, &TwitterApi::saveSearchSuccessful, this, &SavedSearchesModel::handleSaveSearchSuccessful);
    connect(twitterApi, &TwitterApi::destroySavedSearchError, this, &SavedSearchesModel::handleDestroySavedSearchError);
    connect(twitterApi, &TwitterApi::destroySavedSearchSuccessful, this, &SavedSearchesModel::handleDestroySavedSearchSuccessful);
}

int SavedSearchesModel::rowCount(const QModelIndex &) const
{
    return savedSearches.size();
}

QVariant SavedSearchesModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(savedSearches.value(index.row()));
    }
    return QVariant();
}

void SavedSearchesModel::update()
{
    qDebug() << "SavedSearchesModel::update";
    updateInProgress = true;
    twitterApi->savedSearches();
}

void SavedSearchesModel::saveSearch(const QString &query)
{
    qDebug() << "SavedSearchesModel::saveSearch" << query;
    twitterApi->saveSearch(query);
}

void SavedSearchesModel::removeSavedSearch(const QString &id)
{
    qDebug() << "SavedSearchesModel::removeSavedSearch" << id;
    twitterApi->destroySavedSearch(id);
}

void SavedSearchesModel::handleSavedSearchesSuccessful(const QVariantList &result)
{
    qDebug() << "SavedSearchesModel::handleSavedSearchesSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    if (updateInProgress) {
        beginResetModel();
        savedSearches.clear();
        savedSearches.append(result);
        endResetModel();
        updateInProgress = false;
        emit updateFinished();
    } else {
        qDebug() << "Saved Searches API called from somewhere else...";
    }

}

void SavedSearchesModel::handleSavedSearchesError(const QString &errorMessage)
{
    qDebug() << "SavedSearchesModel::handleSavedSearchesError" << errorMessage;
    if (updateInProgress) {
        updateInProgress = false;
        emit updateError(errorMessage);
    } else {
        qDebug() << "Saved Searches API called from somewhere else...";
    }
}

void SavedSearchesModel::handleSaveSearchSuccessful(const QVariantMap &result)
{
    qDebug() << "SavedSearchesModel::handleSaveSearchSuccessful" << result.value("id_str").toString();
    emit saveSuccessful(result.value("query").toString());
    this->update();
}

void SavedSearchesModel::handleSaveSearchError(const QString &errorMessage)
{
    qDebug() << "SavedSearchesModel::handleSaveSearchError" << errorMessage;
    emit saveError(errorMessage);
}

void SavedSearchesModel::handleDestroySavedSearchSuccessful(const QVariantMap &result)
{
    qDebug() << "SavedSearchesModel::handleDestroySavedSearchSuccessful" << result.value("id_str").toString();
    this->update();
}

void SavedSearchesModel::handleDestroySavedSearchError(const QString &errorMessage)
{
    qDebug() << "SavedSearchesModel::handleDestroySavedSearchError" << errorMessage;
    emit removeError(errorMessage);
}
