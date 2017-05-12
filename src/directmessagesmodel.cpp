#include "directmessagesmodel.h"

#include <QListIterator>

DirectMessagesModel::DirectMessagesModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::directMessagesListError, this, &DirectMessagesModel::handleDirectMessagesListError);
    connect(twitterApi, &TwitterApi::directMessagesListSuccessful, this, &DirectMessagesModel::handleDirectMessagesListSuccessful);
    connect(twitterApi, &TwitterApi::showUserError, this, &DirectMessagesModel::handleShowUserError);
    connect(twitterApi, &TwitterApi::showUserSuccessful, this, &DirectMessagesModel::handleShowUserSuccessful);
}

int DirectMessagesModel::rowCount(const QModelIndex &) const
{
    return contacts.size();
}

QVariant DirectMessagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(contacts.value(index.row()));
    }
    return QVariant();
}

void DirectMessagesModel::update()
{
    qDebug() << "DirectMessagesModel::update";
    involvedUsers.clear();
    messages.clear();
    iterations = 0;
    involvedUsers.append(userId);
    twitterApi->directMessagesList();
    // Get the last messages
    // Hydrate the users
    // Build up the contacts accordingly :)
}

void DirectMessagesModel::setUserId(const QString &userId)
{
    qDebug() << "DirectMessagesModel::setUserId" << userId;
    this->userId = userId;
}

void DirectMessagesModel::handleDirectMessagesListSuccessful(const QVariantMap &result)
{
    qDebug() << "DirectMessagesModel::handleDirectMessagesListSuccessful";
    QVariantList events = result.value("events").toList();
    QListIterator<QVariant> messagesIterator(events);
    while (messagesIterator.hasNext()) {
        QVariantMap singleEvent = messagesIterator.next().toMap();
        QVariantMap singleMessage = singleEvent.value("message_create").toMap();
        QString senderId = singleMessage.value("sender_id").toString();
        if (!involvedUsers.contains(senderId)) {
            involvedUsers.append(senderId);
        }
        QString recipientId = singleMessage.value("target").toMap().value("recipient_id").toString();
        if (!involvedUsers.contains(recipientId)) {
            involvedUsers.append(recipientId);
        }
    }
    messages.append(events);
    QString nextCursor = result.value("next_cursor").toString();
    if (iterations < MAX_ITERATIONS && !nextCursor.isEmpty()) {
        iterations++;
        twitterApi->directMessagesList(nextCursor);
    } else {
        compileContacts();
    }
}

void DirectMessagesModel::handleDirectMessagesListError(const QString &errorMessage)
{
    qDebug() << "DirectMessagesModel::handleDirectMessagesListError";
    emit updateFailed(errorMessage);
}

void DirectMessagesModel::handleShowUserSuccessful(const QVariantMap &result)
{
    qDebug() << "DirectMessagesModel::handleShowUserSuccessful" << result.value("name").toString();
}

void DirectMessagesModel::handleShowUserError(const QString &errorMessage)
{
    qDebug() << "DirectMessagesModel::handleShowUserError";
    emit updateFailed(errorMessage);
}

void DirectMessagesModel::compileContacts()
{
    qDebug() << "DirectMessagesModel::compileContacts";
    qDebug() << "Retrieved direct messages: " + QString::number(messages.size());
    QListIterator<QString> usersIterator(involvedUsers);
    while (usersIterator.hasNext()) {
        twitterApi->showUserById(usersIterator.next());
    }
}
