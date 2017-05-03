#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QAbstractListModel>
#include <QSettings>
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
    Q_INVOKABLE void setCurrentTweetId(const QString &tweetId);

    CoverModel * const coverModel;

signals:
    void homeTimelineStartUpdate();
    void homeTimelineUpdated(int modelIndex);
    void homeTimelineError(const QString &errorMessage);

public slots:
    void handleHomeTimelineSuccessful(const QVariantList &result);
    void handleHomeTimelineError(const QString &errorMessage);

private:
    QVariantList timelineTweets;
    QSettings settings;
    TwitterApi *twitterApi;

};

#endif // TIMELINEMODEL_H
