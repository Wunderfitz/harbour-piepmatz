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

private:
    QVariantList ownLists;
    TwitterApi *twitterApi;

};

#endif // OWNLISTSMODEL_H
