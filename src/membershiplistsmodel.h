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
#ifndef MEMBERSHIPLISTSMODEL_H
#define MEMBERSHIPLISTSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class MembershipListsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MembershipListsModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

signals:
    void membershipListsRetrieved();
    void membershipListsError();
    void updateStarted();

public slots:
    void handleMembershipListsSuccessful(const QVariantMap &result);
    void handleMembershipListsError(const QString &errorMessage);

private:
    QVariantList membershipLists;
    TwitterApi *twitterApi;

    bool updateInProgress = false;

};
#endif // MEMBERSHIPLISTSMODEL_H
