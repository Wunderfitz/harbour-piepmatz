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
#ifndef DIRECTMESSAGESMODEL_H
#define DIRECTMESSAGESMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariantList>
#include <QSettings>
#include "twitterapi.h"

const int MAX_ITERATIONS = 2;

class DirectMessagesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DirectMessagesModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();
    Q_INVOKABLE void setUserId(const QString &userId);
    Q_INVOKABLE QVariantList getMessagesForUserId(const QString &userId);

signals:
    void updateMessagesError(const QString &errorMessage);
    void updateMessagesStarted();
    void updateMessagesFinished();
    void newMessagesFound();

private slots:
    void handleDirectMessagesListSuccessful(const QVariantMap &result);
    void handleDirectMessagesListError(const QString &errorMessage);
    void handleDirectMessagesNewSuccessful(const QVariantMap &result);
    void handleDirectMessagesNewError(const QString &errorMessage);
    void handleShowUserSuccessful(const QVariantMap &result);
    void handleShowUserError(const QString &errorMessage);

private:
    TwitterApi *twitterApi;
    QSettings settings;
    int iterations;
    QList<QString> involvedUsers;
    QList<QString> invalidUsers;
    QVariantList messages;
    QVariantMap users;
    QString userId;
    bool incrementalUpdate;
    QVariantMap currentMessage;

    QVariantList contacts;

    void hydrateUsers();
    void compileContacts();
};

#endif // DIRECTMESSAGESMODEL_H
