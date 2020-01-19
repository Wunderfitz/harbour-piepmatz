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
#include "timelinemodel.h"

#include <QListIterator>

const char SETTINGS_CURRENT_TWEET[] = "tweets/currentId";

TimelineModel::TimelineModel(TwitterApi *twitterApi)
    : coverModel(new CoverModel(this)), settings("harbour-piepmatz", "settings")
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::homeTimelineError, this, &TimelineModel::handleHomeTimelineError);
    connect(twitterApi, &TwitterApi::homeTimelineSuccessful, this, &TimelineModel::handleHomeTimelineSuccessful);
}

TimelineModel::~TimelineModel()
{
}

int TimelineModel::rowCount(const QModelIndex &) const
{
    return timelineTweets.size();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(timelineTweets.value(index.row()));
    }
    return QVariant();
}

void TimelineModel::update()
{
    qDebug() << "TimelineModel::update";
    emit homeTimelineStartUpdate();
    twitterApi->homeTimeline();
}

void TimelineModel::loadMore()
{
    qDebug() << "TimelineModel::loadMore";
    emit homeTimelineStartUpdate();
    QString maxId;
    if (!timelineTweets.isEmpty()) {
        QVariantMap lastItem = timelineTweets.last().toMap();
        maxId = lastItem.value("id_str").toString();
    }
    twitterApi->homeTimeline(maxId);
}

void TimelineModel::setCurrentTweetId(const QString &tweetId)
{
    qDebug() << "TimelineModel::setCurrentTweetId" << tweetId;
    settings.setValue(SETTINGS_CURRENT_TWEET, tweetId);
}

void TimelineModel::handleHomeTimelineSuccessful(const QVariantList &result, const bool incrementalUpdate)
{
    qDebug() << "TimelineModel::handleHomeTimelineSuccessful";
    beginResetModel();
    if (incrementalUpdate) {
        qDebug() << "User wanted to load more tweets for the timeline";
        if (result.size() > 1) {
            QVariantList incrementalUpdateResult = result;
            incrementalUpdateResult.removeFirst();
            timelineTweets.append(incrementalUpdateResult);
        } else {
            emit homeTimelineEndReached();
        }
    } else {
        qDebug() << "Complete timeline update";
        if (result.isEmpty()) {
            emit homeTimelineEndReached();
        } else {
            timelineTweets.clear();
            timelineTweets.append(result);
        }
    }
    endResetModel();

    QListIterator<QVariant> tweetIterator(timelineTweets);
    int i = 0;
    int modelIndex = 0;
    QString lastTweetId = settings.value(SETTINGS_CURRENT_TWEET).toString();
    while (tweetIterator.hasNext()) {
        QMap<QString,QVariant> singleTweet = tweetIterator.next().toMap();
        if (singleTweet.value("id_str").toString() == lastTweetId) {
            modelIndex = i;
        }
        i++;
    }

    QVariantList coverList;
    int maxTweets = qMin(6, timelineTweets.size());
    for (int i = 0; i < maxTweets; i++) {
        QMap<QString, QVariant> coverTweet;
        QMap<QString, QVariant> originalTweet = timelineTweets.at(i).toMap();
        if (originalTweet.contains("retweeted_status")) {
            originalTweet = originalTweet.value("retweeted_status").toMap();
        }
        QMap<QString,QVariant> userInfo = originalTweet.value("user").toMap();
        coverTweet.insert("image", userInfo.value("profile_image_url_https"));
        coverTweet.insert("name", userInfo.value("name"));
        coverTweet.insert("text", originalTweet.value("full_text"));
        coverList.append(coverTweet);
    }
    this->coverModel->setCoverTweets(coverList);

    emit homeTimelineUpdated(modelIndex);
}

void TimelineModel::handleHomeTimelineError(const QString &errorMessage)
{
    emit homeTimelineError(errorMessage);
}
