#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QAbstractListModel>
#include "twitterapi.h"

class TimelineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TimelineModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

signals:
    void homeTimelineUpdated();
    void homeTimelineError(const QString &errorMessage);

public slots:
    void handleHomeTimelineSuccessful(const QVariantMap &result);
    void handleHomeTimelineError(const QString &errorMessage);

private:
    QList<QVariantMap> timelineTweets;
    TwitterApi *twitterApi;

};

#endif // TIMELINEMODEL_H
