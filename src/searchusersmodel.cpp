#include "searchusersmodel.h"

SearchUsersModel::SearchUsersModel(TwitterApi *twitterApi)
    : searchInProgress(false)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::searchUsersError, this, &SearchUsersModel::handleSearchUsersError);
    connect(twitterApi, &TwitterApi::searchUsersSuccessful, this, &SearchUsersModel::handleSearchUsersSuccessful);
}

int SearchUsersModel::rowCount(const QModelIndex &) const
{
    return searchResults.size();
}

QVariant SearchUsersModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(searchResults.value(index.row()));
    }
    return QVariant();
}

void SearchUsersModel::search(const QString &query)
{
    searchInProgress = true;
    QRegExp regex("(\\w+)");
    if (query.contains(regex) || query.isEmpty()) {
        twitterApi->searchUsers(query);
    } else {
        searchInProgress = false;
        emit searchFinished();
    }
}

void SearchUsersModel::handleSearchUsersSuccessful(const QVariantList &result)
{
    qDebug() << "SearchUsersModel::handleSearchUsersSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    if (searchInProgress) {
        beginResetModel();
        searchResults.clear();
        searchResults.append(result);
        endResetModel();
        searchInProgress = false;
        emit searchFinished();
    } else {
        qDebug() << "Search API called from somewhere else...";
    }

}

void SearchUsersModel::handleSearchUsersError(const QString &errorMessage)
{
    qDebug() << "SearchUsersModel::handleSearchUsersError";
    if (searchInProgress) {
        searchInProgress = false;
        emit searchError(errorMessage);
    } else {
        qDebug() << "Search API called from somewhere else...";
    }
}
