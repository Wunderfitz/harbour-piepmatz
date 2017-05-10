#include "directmessagesmodel.h"

DirectMessagesModel::DirectMessagesModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
}

int DirectMessagesModel::rowCount(const QModelIndex &) const
{
    return contacts.size();
}

QVariant DirectMessagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(contacts.value(index.row()));
    }
    return QVariant();
}

void DirectMessagesModel::update()
{
    // Get the last messages
    // Hydrate the users
    // Build up the contacts accordingly :)
}
