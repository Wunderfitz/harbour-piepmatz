#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SearchModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void search(const QString &query);

signals:
    void searchFinished();
    void searchError(const QString &errorMessage);

public slots:
    void handleSearchTweetsSuccessful(const QVariantList &result);
    void handleSearchTweetsError(const QString &errorMessage);

private:
    QVariantList searchResults;
    TwitterApi *twitterApi;
    bool searchInProgress;
};

#endif // SEARCHMODEL_H
