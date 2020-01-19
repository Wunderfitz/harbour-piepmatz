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
#include "covermodel.h"

CoverModel::CoverModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

int CoverModel::rowCount(const QModelIndex &) const
{
    return this->coverTweets.size();
}

QVariant CoverModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(coverTweets.value(index.row()));
    }
    return QVariant();
}

void CoverModel::setCoverTweets(const QVariantList &coverTweets)
{
    beginResetModel();
    this->coverTweets.clear();
    this->coverTweets.append(coverTweets);
    endResetModel();
    emit coverInitialized();
}
