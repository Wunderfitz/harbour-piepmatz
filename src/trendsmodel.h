#ifndef TRENDSMODEL_H
#define TRENDSMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class TrendsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TrendsModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

signals:
    void trendsRetrieved(const QString &place);

public slots:
    void handleGetIpInfoSuccessful(const QVariantMap &result);
    void handlePlacesForTrendsSuccessful(const QVariantList &result);
    void handleTrendsSuccessful(const QVariantList &result);

private:
    QVariantList trends;
    TwitterApi *twitterApi;
};

#endif // TRENDSMODEL_H
