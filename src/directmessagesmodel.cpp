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
#include "directmessagesmodel.h"

#include <QListIterator>
#include <QMutableListIterator>
#include <QRegExp>

const char SETTINGS_LAST_MESSAGE[] = "messages/lastId";

DirectMessagesModel::DirectMessagesModel(TwitterApi *twitterApi) : settings("harbour-piepmatz", "settings")
{
    this->twitterApi = twitterApi;
    this->incrementalUpdate = false;

    connect(twitterApi, &TwitterApi::directMessagesListError, this, &DirectMessagesModel::handleDirectMessagesListError);
    connect(twitterApi, &TwitterApi::directMessagesListSuccessful, this, &DirectMessagesModel::handleDirectMessagesListSuccessful);
    connect(twitterApi, &TwitterApi::directMessagesNewError, this, &DirectMessagesModel::handleDirectMessagesNewError);
    connect(twitterApi, &TwitterApi::directMessagesNewSuccessful, this, &DirectMessagesModel::handleDirectMessagesNewSuccessful);
    connect(twitterApi, &TwitterApi::showUserByIdError, this, &DirectMessagesModel::handleShowUserError);
    connect(twitterApi, &TwitterApi::showUserByIdSuccessful, this, &DirectMessagesModel::handleShowUserSuccessful);
}

int DirectMessagesModel::rowCount(const QModelIndex &) const
{
    return contacts.size();
}

QVariant DirectMessagesModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(contacts.value(index.row()));
    }
    return QVariant();
}

void DirectMessagesModel::update()
{
    qDebug() << "DirectMessagesModel::update";
    emit updateMessagesStarted();
    involvedUsers.clear();
    invalidUsers.clear();
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
    bool firstOtherMessageFound = false;
    QVariantList events = result.value("events").toList();
    QListIterator<QVariant> messagesIterator(events);
    while (messagesIterator.hasNext()) {
        QVariantMap singleEvent = messagesIterator.next().toMap();
        QVariantMap singleMessage = singleEvent.value("message_create").toMap();
        QString senderId = singleMessage.value("sender_id").toString();
        QString recipientId = singleMessage.value("target").toMap().value("recipient_id").toString();
        if (iterations == 0 && !firstOtherMessageFound && recipientId == this->userId) {
            QString storedMessageId = settings.value(SETTINGS_LAST_MESSAGE).toString();
            QString lastMessageId = singleEvent.value("id").toString();
            if (!storedMessageId.isEmpty() && storedMessageId != lastMessageId) {
                emit newMessagesFound();
            }
            settings.setValue(SETTINGS_LAST_MESSAGE, lastMessageId);
            firstOtherMessageFound = true;
        }
        if (!involvedUsers.contains(senderId)) {
            involvedUsers.append(senderId);
        }
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
            qSort(contacts.begin(), contacts.end(), reverseTimestamp);
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
    if (incrementalUpdate) {
        emit updateMessagesError(errorMessage);
    } else {
        // Not so pretty, but we only send an error message now. In the future, we need to return the user ID as well...
        QRegExp regex("user_id\\=(\\d+)");
        if (errorMessage.contains(regex)) {
            qDebug() << "Removing user " << regex.cap(1);
            invalidUsers.append(regex.cap(1));
            involvedUsers.removeAll(regex.cap(1));
            if (users.size() == involvedUsers.size()) {
                qDebug() << "DirectMessagesModel::handleShowUserError" << "After this error, all users are hydrated, we can compile the messages list!";
                compileContacts();
            }
        } else {
            emit updateMessagesError(errorMessage);
        }
    }
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
        if (invalidUsers.contains(senderId)) {
            qDebug() << "DirectMessagesModel::compileContacts - Direct messages contains content from the invalid user: " + recipientId;
            continue;
        }
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
