#ifndef OWNLISTSMODEL_H
#define OWNLISTSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class OwnListsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    OwnListsModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

signals:
    void ownListsRetrieved();
    void ownListsError();
    void updateStarted();

public slots:
    void handleUserListsSuccessful(const QVariantList &result);
    void handleUserListsError(const QString &errorMessage);

private:
    QVariantList ownLists;
    TwitterApi *twitterApi;

    bool updateInProgress = false;

};

#endif // OWNLISTSMODEL_H
