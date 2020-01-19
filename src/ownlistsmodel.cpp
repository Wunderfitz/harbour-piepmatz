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
#include "ownlistsmodel.h"

OwnListsModel::OwnListsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
    connect(twitterApi, &TwitterApi::userListsSuccessful, this, &OwnListsModel::handleUserListsSuccessful);
    connect(twitterApi, &TwitterApi::userListsError, this, &OwnListsModel::handleUserListsError);
}

int OwnListsModel::rowCount(const QModelIndex &) const
{
    return ownLists.size();
}

QVariant OwnListsModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(ownLists.value(index.row()));
    }
    return QVariant();
}

void OwnListsModel::update()
{
    qDebug() << "OwnListsModel::update";
    emit updateStarted();
    this->updateInProgress = true;
    this->twitterApi->userLists();
}

void OwnListsModel::handleUserListsSuccessful(const QVariantList &result)
{
    qDebug() << "OwnListsModel::handleUerListsSuccessful";
    if (this->updateInProgress) {
        beginResetModel();
        this->ownLists.clear();
        this->ownLists.append(result);
        endResetModel();
        emit ownListsRetrieved();
        this->updateInProgress = false;
        qDebug() << "Own lists model updated";
    }
}

void OwnListsModel::handleUserListsError(const QString &errorMessage)
{
    qDebug() << "OwnListsModel::handleUserListsError" << errorMessage;
    emit ownListsError();
    this->updateInProgress = false;
}
