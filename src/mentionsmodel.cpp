#include "mentionsmodel.h"

MentionsModel::MentionsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::homeTimelineError, this, &MentionsModel::handleUpdateMentionsError);
    connect(twitterApi, &TwitterApi::homeTimelineSuccessful, this, &MentionsModel::handleUpdateMentionsSuccessful);
}

int MentionsModel::rowCount(const QModelIndex &) const
{
    return mentions.size();
}

QVariant MentionsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(mentions.value(index.row()));
    }
    return QVariant();
}

void MentionsModel::update()
{
    twitterApi->mentionsTimeline();
}

void MentionsModel::handleUpdateMentionsSuccessful(const QVariantList &result)
{
    qDebug() << "MentionsModel::handleUpdateMentionsSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    beginResetModel();
    mentions.clear();
    mentions.append(result);
    endResetModel();
    emit updateMentionsFinished();

}

void MentionsModel::handleUpdateMentionsError(const QString &errorMessage)
{
    emit updateMentionsError(errorMessage);
}

