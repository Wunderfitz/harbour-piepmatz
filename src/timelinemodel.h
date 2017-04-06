#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"
#include "covermodel.h"

class TimelineModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TimelineModel(TwitterApi *twitterApi);
    ~TimelineModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

    CoverModel * const coverModel;

signals:
    void homeTimelineStartUpdate();
    void homeTimelineUpdated();
    void homeTimelineError(const QString &errorMessage);

public slots:
    void handleHomeTimelineSuccessful(const QVariantList &result);
    void handleHomeTimelineError(const QString &errorMessage);

private:
    QVariantList timelineTweets;
    TwitterApi *twitterApi;

};

#endif // TIMELINEMODEL_H
