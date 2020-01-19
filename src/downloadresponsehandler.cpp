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
#include "downloadresponsehandler.h"

#include <QStandardPaths>
#include <QFile>

DownloadResponseHandler::DownloadResponseHandler(const QString &fileName, TwitterApi *twitterApi)
{
    this->fileName = fileName;
    this->twitterApi = twitterApi;
}

void DownloadResponseHandler::handleDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "DownloadResponseHandler::handleImageUploadProgress" << this->fileName << QString::number(bytesSent) << QString::number(bytesTotal);
    int percentCompleted = 100 * bytesSent / bytesTotal;
    emit twitterApi->downloadStatus(fileName, percentCompleted);
}

void DownloadResponseHandler::handleDownloadError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "DownloadResponseHandler::handleImageUploadError:" << (int)error << reply->errorString() << reply->readAll();
    emit twitterApi->downloadError(fileName, reply->errorString());
}

void DownloadResponseHandler::handleDownloadFinished()
{
    qDebug() << "DownloadResponseHandler::handleImageUploadFinished" << this->fileName;
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QLatin1Char('/') + this->fileName;
    QFile downloadedFile(filePath);
    if (downloadedFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Writing downloaded file to " + downloadedFile.fileName();
        downloadedFile.write(reply->readAll());
        downloadedFile.close();
        emit twitterApi->downloadSuccessful(fileName, filePath);
    } else {
        emit twitterApi->downloadError(fileName, "Error storing file at " + filePath);
    }

}
