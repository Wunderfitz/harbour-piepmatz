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
#ifndef DOWNLOADRESPONSEHANDLER_H
#define DOWNLOADRESPONSEHANDLER_H

#include "twitterapi.h"
#include <QObject>
#include <QString>

class DownloadResponseHandler : public QObject
{
    Q_OBJECT
public:
    DownloadResponseHandler(const QString &fileName, TwitterApi *twitterApi);

public slots:
    void handleDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void handleDownloadError(QNetworkReply::NetworkError error);
    void handleDownloadFinished();

private:
    QString fileName;
    TwitterApi *twitterApi;
};

#endif // DOWNLOADRESPONSEHANDLER_H
