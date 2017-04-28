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
