#include "imagemetadataresponsehandler.h"

#include <QDebug>

ImageMetadataResponseHandler::ImageMetadataResponseHandler(const QString &mediaId, TwitterApi *twitterApi)
{
    this->mediaId = mediaId;
    this->twitterApi = twitterApi;
}

void ImageMetadataResponseHandler::handleImageMetadataUploadError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "ImageMetadataResponseHandler::handleImageUploadError:" << (int)error << reply->errorString() << reply->readAll();
    emit twitterApi->imageDescriptionUploadError(mediaId, reply->errorString());
}

void ImageMetadataResponseHandler::handleImageMetadataUploadFinished()
{
    qDebug() << "ImageMetadataResponseHandler::handleImageUploadFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    emit twitterApi->imageDescriptionUploadSuccessful(mediaId);
}

