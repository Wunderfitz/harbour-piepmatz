#include "timelinemodel.h"

TimelineModel::TimelineModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
    this->coverModel = new CoverModel();

    connect(twitterApi, SIGNAL(homeTimelineError(QString)), this, SLOT(handleHomeTimelineError(QString)));
    connect(twitterApi, SIGNAL(homeTimelineSuccessful(QVariantList)), this, SLOT(handleHomeTimelineSuccessful(QVariantList)));
}

TimelineModel::~TimelineModel()
{
    delete this->coverModel;
}

int TimelineModel::rowCount(const QModelIndex &) const
{
    return timelineTweets.size();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(timelineTweets.value(index.row()));
    }
    return QVariant();
}

void TimelineModel::update()
{
    twitterApi->homeTimeline();
}

void TimelineModel::handleHomeTimelineSuccessful(const QVariantList &result)
{
    qDebug() << "TimelineModel::handleHomeTimelineSuccessful";
    beginResetModel();
    timelineTweets.clear();
    timelineTweets.append(result);
    endResetModel();

    QVariantList coverList;
    for (int i = 0; i < 4; i++) {
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

    emit homeTimelineUpdated();
}

void TimelineModel::handleHomeTimelineError(const QString &errorMessage)
{
    emit homeTimelineError(errorMessage);
}
