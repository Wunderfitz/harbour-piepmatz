#include "searchmodel.h"

SearchModel::SearchModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, SIGNAL(searchTweetsError(QString)), this, SLOT(handleSearchTweetsError(QString)));
    connect(twitterApi, SIGNAL(searchTweetsSuccessful(QVariantList)), this, SLOT(handleSearchTweetsSuccessful(QVariantList)));
}

int SearchModel::rowCount(const QModelIndex &) const
{
    return searchResults.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(searchResults.value(index.row()));
    }
    return QVariant();
}

void SearchModel::search(const QString &query)
{
    twitterApi->searchTweets(query);
}

void SearchModel::handleSearchTweetsSuccessful(const QVariantList &result)
{
    qDebug() << "SearchModel::handleSearchTweetsSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    beginResetModel();
    searchResults.clear();
    searchResults.append(result);
    endResetModel();

    emit searchFinished();
}

void SearchModel::handleSearchTweetsError(const QString &errorMessage)
{
    emit searchError(errorMessage);
}
