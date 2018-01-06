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

    Q_INVOKABLE void update();

signals:
    void membershipListsRetrieved();
    void membershipListsError();
    void updateStarted();

public slots:
    void handleMembershipListsSuccessful(const QVariantMap &result);
    void handleMembershipListsError(const QString &errorMessage);

private:
    QVariantList membershipLists;
    TwitterApi *twitterApi;

    bool updateInProgress = false;

};
#endif // MEMBERSHIPLISTSMODEL_H
