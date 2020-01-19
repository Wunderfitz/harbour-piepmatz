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
#include "membershiplistsmodel.h"

MembershipListsModel::MembershipListsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
    connect(twitterApi, &TwitterApi::listsMembershipsSuccessful, this, &MembershipListsModel::handleMembershipListsSuccessful);
    connect(twitterApi, &TwitterApi::listsMembershipsError, this, &MembershipListsModel::handleMembershipListsError);
}

int MembershipListsModel::rowCount(const QModelIndex &) const
{
    return membershipLists.size();
}

QVariant MembershipListsModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(membershipLists.value(index.row()));
    }
    return QVariant();
}

void MembershipListsModel::update()
{
    qDebug() << "MembershipListsModel::update";
    emit updateStarted();
    this->updateInProgress = true;
    this->twitterApi->listsMemberships();
}

void MembershipListsModel::handleMembershipListsSuccessful(const QVariantMap &result)
{
    qDebug() << "MembershipListsModel::handleMembershipListsSuccessful";
    if (this->updateInProgress) {
        beginResetModel();
        this->membershipLists.clear();
        this->membershipLists.append(result.value("lists").toList());
        endResetModel();
        emit membershipListsRetrieved();
        this->updateInProgress = false;
        qDebug() << "Membership lists model updated";
    }
}

void MembershipListsModel::handleMembershipListsError(const QString &errorMessage)
{
    qDebug() << "MembershipListsModel::handleMembershipListsError" << errorMessage;
    emit membershipListsError();
    this->updateInProgress = false;
}
