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
