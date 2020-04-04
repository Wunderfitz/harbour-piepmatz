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
#include "emojisearchworker.h"

EmojiSearchWorker::~EmojiSearchWorker()
{
    qDebug() << "EmojiSearchWorker::destroy";
    database.close();
}

EmojiSearchWorker::EmojiSearchWorker(QObject *parent) : QThread(parent)
{
    qDebug() << "Initializing Emoji database";
    QSqlDatabase::removeDatabase("emojis");
    database = QSqlDatabase::addDatabase("QSQLITE", "emojis");
    database.setDatabaseName("/usr/share/harbour-piepmatz/db/emojis.db");
}

void EmojiSearchWorker::setParameters(const QString &queryString)
{
    this->queryString = queryString;
}

void EmojiSearchWorker::performSearch()
{
    qDebug() << "EmojiSearchWorker::performSearch" << this->queryString;
    QVariantList resultList;

    if (database.open()) {
        QSqlQuery query(database);
        query.prepare("select * from emojis where description match (:queryString) limit 25");
        query.bindValue(":queryString", queryString + "*");
        query.exec();
        while (query.next()) {
            if (isInterruptionRequested()) {
                break;
            }
            QVariantMap foundEmoji;
            foundEmoji.insert("file_name", query.value(0).toString());
            foundEmoji.insert("emoji", query.value(1).toString());
            foundEmoji.insert("emoji_version", query.value(2).toString());
            foundEmoji.insert("description", query.value(3).toString());
            resultList.append(foundEmoji);
        }
        database.close();
    } else {
        qDebug() << "Unable to perform a query on database" << database.lastError().databaseText();
    }

    emit searchCompleted(queryString, resultList);
}
