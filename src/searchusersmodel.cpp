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
#include "searchusersmodel.h"

SearchUsersModel::SearchUsersModel(TwitterApi *twitterApi)
    : searchInProgress(false)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::searchUsersError, this, &SearchUsersModel::handleSearchUsersError);
    connect(twitterApi, &TwitterApi::searchUsersSuccessful, this, &SearchUsersModel::handleSearchUsersSuccessful);
}

int SearchUsersModel::rowCount(const QModelIndex &) const
{
    return searchResults.size();
}

QVariant SearchUsersModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(searchResults.value(index.row()));
    }
    return QVariant();
}

void SearchUsersModel::search(const QString &query)
{
    searchInProgress = true;
    QRegExp regex("(\\w+)");
    if (query.contains(regex) || query.isEmpty()) {
        twitterApi->searchUsers(query);
    } else {
        searchInProgress = false;
        emit searchFinished();
    }
}

void SearchUsersModel::handleSearchUsersSuccessful(const QVariantList &result)
{
    qDebug() << "SearchUsersModel::handleSearchUsersSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    if (searchInProgress) {
        beginResetModel();
        searchResults.clear();
        searchResults.append(result);
        endResetModel();
        searchInProgress = false;
        emit searchFinished();
    } else {
        qDebug() << "Search API called from somewhere else...";
    }

}

void SearchUsersModel::handleSearchUsersError(const QString &errorMessage)
{
    qDebug() << "SearchUsersModel::handleSearchUsersError";
    if (searchInProgress) {
        searchInProgress = false;
        emit searchError(errorMessage);
    } else {
        qDebug() << "Search API called from somewhere else...";
    }
}
