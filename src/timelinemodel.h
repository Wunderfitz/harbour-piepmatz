/*
    Copyright (C) 2017-20 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
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
    Q_INVOKABLE void loadMore();
    Q_INVOKABLE void setCurrentTweetId(const QString &tweetId);

    CoverModel * const coverModel;

signals:
    void homeTimelineStartUpdate();
    void homeTimelineUpdated(int modelIndex);
    void homeTimelineError(const QString &errorMessage);
    void homeTimelineEndReached();

public slots:
    void handleHomeTimelineSuccessful(const QVariantList &result, const bool incrementalUpdate);
    void handleHomeTimelineError(const QString &errorMessage);

private:
    QVariantList timelineTweets;
    QSettings settings;
    TwitterApi *twitterApi;

};

#endif // TIMELINEMODEL_H
