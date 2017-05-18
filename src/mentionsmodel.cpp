#include "mentionsmodel.h"

const char SETTINGS_LAST_MENTION[] = "mentions/lastId";

MentionsModel::MentionsModel(TwitterApi *twitterApi) : settings("harbour-piepmatz", "settings")
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::mentionsTimelineError, this, &MentionsModel::handleUpdateMentionsError);
    connect(twitterApi, &TwitterApi::mentionsTimelineSuccessful, this, &MentionsModel::handleUpdateMentionsSuccessful);
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
    if (!result.isEmpty()) {
        QString storedMentionId = settings.value(SETTINGS_LAST_MENTION).toString();
        QString lastMentionId = result.first().toMap().value("id_str").toString();
        if (!storedMentionId.isEmpty() && storedMentionId != lastMentionId) {
            emit newMentionsFound();
        }
        settings.setValue(SETTINGS_LAST_MENTION, lastMentionId);
    }
    endResetModel();
    emit updateMentionsFinished();

}

void MentionsModel::handleUpdateMentionsError(const QString &errorMessage)
{
    emit updateMentionsError(errorMessage);
}

