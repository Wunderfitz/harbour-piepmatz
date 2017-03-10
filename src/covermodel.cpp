#include "covermodel.h"

CoverModel::CoverModel()
{

}

int CoverModel::rowCount(const QModelIndex &) const
{
    return this->coverTweets.size();
}

QVariant CoverModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
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
}
