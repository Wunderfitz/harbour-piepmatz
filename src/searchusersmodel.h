#ifndef SEARCHUSERSMODEL_H
#define SEARCHUSERSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class SearchUsersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SearchUsersModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void search(const QString &query);

signals:
    void searchFinished();
    void searchError(const QString &errorMessage);

public slots:
    void handleSearchUsersSuccessful(const QVariantList &result);
    void handleSearchUsersError(const QString &errorMessage);

private:
    QVariantList searchResults;
    TwitterApi *twitterApi;
    bool searchInProgress;
};

#endif // SEARCHUSERSMODEL_H
