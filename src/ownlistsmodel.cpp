#include "ownlistsmodel.h"

OwnListsModel::OwnListsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
}

int OwnListsModel::rowCount(const QModelIndex &) const
{
    return ownLists.size();
}

QVariant OwnListsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(ownLists.value(index.row()));
    }
    return QVariant();
}
