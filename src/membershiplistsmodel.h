#ifndef MEMBERSHIPLISTSMODEL_H
#define MEMBERSHIPLISTSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class MembershipListsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MembershipListsModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private:
    QVariantList membershipLists;
    TwitterApi *twitterApi;

};
#endif // MEMBERSHIPLISTSMODEL_H
