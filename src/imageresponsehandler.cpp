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
