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
#ifndef COVERMODEL_H
#define COVERMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class CoverModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CoverModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void setCoverTweets(const QVariantList &coverTweets);

signals:
    void coverInitialized();

private:
    QVariantList coverTweets;

};

#endif // COVERMODEL_H
