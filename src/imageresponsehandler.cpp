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
#include "imageresponsehandler.h"

#include <QDebug>

ImageResponseHandler::ImageResponseHandler(const QString &fileName, TwitterApi *twitterApi)
{
    this->fileName = fileName;
    this->twitterApi = twitterApi;
}

void ImageResponseHandler::handleImageUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "ImageResponseHandler::handleImageUploadProgress";
    emit twitterApi->imageUploadStatus(fileName, bytesSent, bytesTotal);
}

void ImageResponseHandler::handleImageUploadError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "ImageResponseHandler::handleImageUploadError:" << (int)error << reply->errorString() << reply->readAll();
    emit twitterApi->imageUploadError(fileName, reply->errorString());
}

void ImageResponseHandler::handleImageUploadFinished()
{
    qDebug() << "ImageResponseHandler::handleImageUploadFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        emit twitterApi->imageUploadSuccessful(fileName, responseObject.toVariantMap());
    } else {
        emit twitterApi->imageUploadError(fileName, "Piepmatz couldn't understand Twitter's response!");
    }
}
