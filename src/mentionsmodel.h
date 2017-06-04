/*
    Copyright (C) 2017 Sebastian J. Wolf

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
#ifndef MENTIONSMODEL_H
#define MENTIONSMODEL_H


#include <QAbstractListModel>
#include <QVariantList>
#include <QSettings>
#include "twitterapi.h"

class MentionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    MentionsModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

signals:
    void updateMentionsFinished();
    void updateMentionsError(const QString &errorMessage);
    void newMentionsFound();

public slots:
    void handleUpdateMentionsSuccessful(const QVariantList &result);
    void handleUpdateMentionsError(const QString &errorMessage);

private:
    QVariantList mentions;
    QSettings settings;
    TwitterApi *twitterApi;
};

#endif // MENTIONSMODEL_H
