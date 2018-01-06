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
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(membershipLists.value(index.row()));
    }
    return QVariant();
}

void MembershipListsModel::update()
{
    qDebug() << "MembershipListsModel::update";
    this->updateInProgress = true;
    this->twitterApi->listsMemberships();
}

void MembershipListsModel::handleMembershipListsSuccessful(const QVariantMap &result)
{
    qDebug() << "MembershipListsModel::handleMembershipListsSuccessful";
    if (!result.isEmpty() && this->updateInProgress) {
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
