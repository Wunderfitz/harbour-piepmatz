#include "membershiplistsmodel.h"

MembershipListsModel::MembershipListsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
}

int MembershipListsModel::rowCount(const QModelIndex &) const
{
    return membershipLists.size();
}

QVariant MembershipListsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(membershipLists.value(index.row()));
    }
    return QVariant();
}
