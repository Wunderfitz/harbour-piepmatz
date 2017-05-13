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
    emit updateMessagesStarted();
    involvedUsers.clear();
    messages.clear();
    users.clear();
    iterations = 0;
    involvedUsers.append(userId);
    twitterApi->directMessagesList();
}

void DirectMessagesModel::setUserId(const QString &userId)
{
    qDebug() << "DirectMessagesModel::setUserId" << userId;
    this->userId = userId;
}

bool earlierMessage(const QVariant &message1, const QVariant &message2)
{
    QVariantMap messageMap1 = message1.toMap();
    QVariantMap messageMap2 = message2.toMap();
    return messageMap1.value("created_timestamp").toLongLong() < messageMap2.value("created_timestamp").toLongLong();
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
        qDebug() << "Retrieved direct messages: " + QString::number(messages.size());
        qSort(messages.begin(), messages.end(), earlierMessage);
        hydrateUsers();
    }
}

void DirectMessagesModel::handleDirectMessagesListError(const QString &errorMessage)
{
    qDebug() << "DirectMessagesModel::handleDirectMessagesListError";
    emit updateMessagesError(errorMessage);
}

void DirectMessagesModel::handleShowUserSuccessful(const QVariantMap &result)
{
    qDebug() << "DirectMessagesModel::handleShowUserSuccessful" << result.value("name").toString();
    QString currentUserId = result.value("id_str").toString();
    if (involvedUsers.contains(currentUserId)) {
        users.insert(currentUserId, result);
    }
    if (users.size() == involvedUsers.size()) {
        qDebug() << "DirectMessagesModel::handleShowUserSuccessful" << "All users hydrated, finally compiling messages list!";
        compileContacts();
    }
}

void DirectMessagesModel::handleShowUserError(const QString &errorMessage)
{
    qDebug() << "DirectMessagesModel::handleShowUserError";
    emit updateMessagesError(errorMessage);
}

void DirectMessagesModel::hydrateUsers()
{
    qDebug() << "DirectMessagesModel::hydrateUsers";
    QListIterator<QString> usersIterator(involvedUsers);
    while (usersIterator.hasNext()) {
        twitterApi->showUserById(usersIterator.next());
    }
}

void DirectMessagesModel::compileContacts()
{
    qDebug() << "DirectMessagesModel::compileContacts";
    beginResetModel();
    contacts.clear();
    QMap<QString,QVariantList> rawContacts;
    QListIterator<QString> involvedUserIterator(involvedUsers);
    while (involvedUserIterator.hasNext()) {
        rawContacts.insert(involvedUserIterator.next(), QVariantList());
    }

    QListIterator<QVariant> messagesIterator(messages);
    while (messagesIterator.hasNext()) {
        QVariant rawMessage = messagesIterator.next();
        QVariantMap currentMessage = rawMessage.toMap().value("message_create").toMap();
        QString senderId = currentMessage.value("sender_id").toString();
        QString recipientId = currentMessage.value("target").toMap().value("recipient_id").toString();
        if (senderId == userId) {
            QVariantList contactMessages = rawContacts.value(recipientId);
            contactMessages.append(rawMessage);
            rawContacts.insert(recipientId, contactMessages);
        }
        if (recipientId == userId) {
            QVariantList contactMessages = rawContacts.value(senderId);
            contactMessages.append(rawMessage);
            rawContacts.insert(senderId, contactMessages);
        }
    }
    QListIterator<QString> usersIterator(users.keys());
    while (usersIterator.hasNext()) {
        QString currentUserId = usersIterator.next();
        if (currentUserId == userId) {
            continue;
        }
        QVariantMap contact;
        contact.insert("user", users.value(currentUserId));
        contact.insert("messages", rawContacts.value(currentUserId));
        contacts.append(contact);
    }
    endResetModel();
    emit updateMessagesFinished();
}
