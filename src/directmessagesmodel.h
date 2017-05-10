#ifndef DIRECTMESSAGESMODEL_H
#define DIRECTMESSAGESMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class DirectMessagesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DirectMessagesModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

private:
    TwitterApi *twitterApi;
    QVariantList contacts;
};

#endif // DIRECTMESSAGESMODEL_H
