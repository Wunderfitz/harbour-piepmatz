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
#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SearchModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void search(const QString &query);

signals:
    void searchFinished();
    void searchError(const QString &errorMessage);

public slots:
    void handleSearchTweetsSuccessful(const QVariantList &result);
    void handleSearchTweetsError(const QString &errorMessage);

private:
    QVariantList searchResults;
    TwitterApi *twitterApi;
    bool searchInProgress;
};

#endif // SEARCHMODEL_H
