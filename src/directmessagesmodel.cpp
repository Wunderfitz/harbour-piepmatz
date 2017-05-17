#include "directmessagesmodel.h"

#include <QListIterator>
#include <QMutableListIterator>

DirectMessagesModel::DirectMessagesModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
    this->incrementalUpdate = false;

    connect(twitterApi, &TwitterApi::directMessagesListError, this, &DirectMessagesModel::handleDirectMessagesListError);
    connect(twitterApi, &TwitterApi::directMessagesListSuccessful, this, &DirectMessagesModel::handleDirectMessagesListSuccessful);
    connect(twitterApi, &TwitterApi::directMessagesNewError, this, &DirectMessagesModel::handleDirectMessagesNewError);
    connect(twitterApi, &TwitterApi::directMessagesNewSuccessful, this, &DirectMessagesModel::handleDirectMessagesNewSuccessful);
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

QVariantList DirectMessagesModel::getMessagesForUserId(const QString &userId)
{
    QListIterator<QVariant> contactsIterator(contacts);
    while (contactsIterator.hasNext()) {
        QVariantMap currentContact = contactsIterator.next().toMap();
        if (currentContact.value("user").toMap().value("id_str") == userId ) {
            return currentContact.value("messages").toList();
        }
    }
    return QVariantList();
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

void DirectMessagesModel::handleDirectMessagesNewSuccessful(const QVariantMap &result)
{
    qDebug() << "DirectMessagesModel::handleDirectMessagesNewSuccessful";
    QVariantMap newMessage = result.value("event").toMap();
    QString messageRecipientId = newMessage.value("message_create").toMap().value("target").toMap().value("recipient_id").toString();
    QMutableListIterator<QVariant> contactsIterator(contacts);
    while (contactsIterator.hasNext()) {
        QVariantMap currentContact = contactsIterator.next().toMap();
        if (currentContact.value("user").toMap().value("id_str") == messageRecipientId ) {
            beginResetModel();
            QVariantList messagesList = currentContact.value("messages").toList();
            messagesList.append(newMessage);
            currentContact.insert("messages", messagesList);
            contactsIterator.setValue(currentContact);
            endResetModel();
            return;
        }
    }
    incrementalUpdate = true;
    currentMessage = newMessage;
    twitterApi->showUserById(messageRecipientId);
}

void DirectMessagesModel::handleDirectMessagesNewError(const QString &errorMessage)
{
    qDebug() << "DirectMessagesModel::handleDirectMessagesNewError" << errorMessage;
    // Nothing to do for now, UI will display a notification anyway
}

bool reverseTimestamp(const QVariant &contact1, const QVariant &contact2)
{
    QVariantList messages1 = contact1.toMap().value("messages").toList();
    QVariantList messages2 = contact2.toMap().value("messages").toList();
    qlonglong timestamp1 = 0;
    qlonglong timestamp2 = 0;

    if (!messages1.isEmpty()) {
        timestamp1 = messages1.last().toMap().value("created_timestamp").toLongLong();
    }
    if (!messages2.isEmpty()) {
        timestamp2 = messages2.last().toMap().value("created_timestamp").toLongLong();
    }

    return timestamp1 > timestamp2;
}

void DirectMessagesModel::handleShowUserSuccessful(const QVariantMap &result)
{
    qDebug() << "DirectMessagesModel::handleShowUserSuccessful" << result.value("id_str").toString() << result.value("name").toString();
    QString currentUserId = result.value("id_str").toString();

    if (incrementalUpdate) {
        qDebug() << "DirectMessagesModel::handleShowUserSuccessful - INCREMENTAL update";
        incrementalUpdate = false;
        QVariantList messagesList;
        messagesList.append(currentMessage);
        beginResetModel();
        QVariantMap contact;
        contact.insert("user", result);
        contact.insert("messages", messagesList);
        contacts.append(contact);
        qSort(contacts.begin(), contacts.end(), reverseTimestamp);
        endResetModel();
    } else {
        qDebug() << "DirectMessagesModel::handleShowUserSuccessful - FULL update";
        if (involvedUsers.contains(currentUserId)) {
            users.insert(currentUserId, result);
        }
        if (users.size() == involvedUsers.size()) {
            qDebug() << "DirectMessagesModel::handleShowUserSuccessful" << "All users hydrated, finally compiling messages list!";
            compileContacts();
        }
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
    qSort(contacts.begin(), contacts.end(), reverseTimestamp);
    endResetModel();
    emit updateMessagesFinished();
}
