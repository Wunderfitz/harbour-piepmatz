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
#include "imagesmodel.h"

#include <QDebug>
#include <QList>
#include <QListIterator>
#include <QMapIterator>
#include <QFileInfo>

ImagesModel::ImagesModel(TwitterApi *twitterApi)
{
    workerThread = new ImagesSearchWorker();
    imageProcessor = new ImageProcessor();
    connect(workerThread, SIGNAL(searchFinished()), this, SLOT(handleSearchFinished()));
    connect(imageProcessor, SIGNAL(processingComplete()), this, SLOT(handleImageProcessingComplete()));

    connect(twitterApi, SIGNAL(imageUploadSuccessful(QString,QVariantMap)), this, SLOT(handleImageUploadSuccessful(QString,QVariantMap)));
    connect(twitterApi, SIGNAL(imageUploadError(QString,QString)), this, SLOT(handleImageUploadError(QString,QString)));
    connect(twitterApi, SIGNAL(imageUploadStatus(QString,qint64,qint64)), this, SLOT(handleImageUploadStatus(QString,qint64,qint64)));
    connect(twitterApi, SIGNAL(imageDescriptionUploadSuccessful(QString)), this, SLOT(handleImageDescriptionUploadSuccessful(QString)));
    connect(twitterApi, SIGNAL(imageDescriptionUploadError(QString,QString)), this, SLOT(handleImageDescriptionUploadError(QString,QString)));

    this->twitterApi = twitterApi;
}

int ImagesModel::rowCount(const QModelIndex &) const
{
    return images.size();
}

QVariant ImagesModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(images.value(index.row()));
    }
    return QVariant();
}

void ImagesModel::update()
{
    qDebug() << "ImagesModel::update";
    workerThread->start();
}

void ImagesModel::setSelectedImages(const QVariantList &selectedImages)
{
    qDebug() << "ImagesModel::setSelectedImages";
    this->selectedImages.clear();
    this->selectedImages.append(selectedImages);
    this->imageProcessor->setSelectedImages(selectedImages);
    emit imagesSelected();
}

QVariantList ImagesModel::getSelectedImages()
{
    return this->selectedImages;
}

void ImagesModel::uploadSelectedImages()
{
    qDebug() << "ImagesModel::uploadSelectedImages";
    emit uploadStarted();
    imageProcessor->start();
}

void ImagesModel::processUploadCompleted()
{
    qDebug() << "ImagesModel::processUploadCompleted";
    qDebug() << "Images uploaded: " << uploadedImages.size() << "/" << selectedImages.size() << ", descriptions: " << uploadedImageDescriptions.size() << "/" << imageDescriptions.size();
    if (uploadedImages.size() == selectedImages.size() && uploadedImageDescriptions.size() == imageDescriptions.size()) {
        qDebug() << "Finally sending tweet!";
        QMapIterator<QString, QVariantMap> uploadedImagesIterator(uploadedImages);
        QString mediaIds;
        while (uploadedImagesIterator.hasNext()) {
            if (!mediaIds.isEmpty()) {
                mediaIds += QLatin1Char(',');
            }
            mediaIds += uploadedImagesIterator.next().value().value("media_id_string").toString();
        }
        if (replyToStatusId.isEmpty()) {
            twitterApi->tweetWithImages(tweetText, mediaIds, tweetPlaceId);
        } else {
            twitterApi->replyToTweetWithImages(tweetText, replyToStatusId, mediaIds, tweetPlaceId);
        }
        this->clearModel();
        emit uploadCompleted();
    }
}

void ImagesModel::clearModel()
{
    qDebug() << "ImagesModel::clearModel";
    this->selectedImages.clear();
    this->imageDescriptions.clear();
    this->uploadedImageDescriptions.clear();
    this->imageProcessor->removeTemporaryFiles();
    this->tweetText.clear();
    this->replyToStatusId.clear();
    this->tweetWithImagesInProgress = false;
    this->selectedImagesSize = 0;
    this->uploadedImages.clear();
    this->uploadImagesBytesSent.clear();
}

void ImagesModel::tweetWithSelectedImages(const QString &text, const QString &placeId)
{
    this->tweetText = text;
    this->tweetWithImagesInProgress = true;
    this->tweetPlaceId = placeId;
    this->uploadSelectedImages();
}

void ImagesModel::replyToTweetWithSelectedImages(const QString &text, const QString &replyToStatusId, const QString &placeId)
{
    this->tweetText = text;
    this->replyToStatusId = replyToStatusId;
    this->tweetWithImagesInProgress = true;
    this->tweetPlaceId = placeId;
    this->uploadSelectedImages();
}

bool ImagesModel::isTweetWithImagesInProgress()
{
    return this->tweetWithImagesInProgress;
}

void ImagesModel::setImageDescription(const QString &fileName, const QString &description)
{
    if (!description.isEmpty()) {
        this->imageDescriptions.insert(fileName, description);
    }
}

QString ImagesModel::getImageDescription(const QString &fileName)
{
    return this->imageDescriptions.value(fileName, QString());
}

void ImagesModel::handleSearchFinished()
{
    qDebug() << "ImagesModel::handleSearchFinished";
    beginResetModel();
    images.clear();
    QList<QFileInfo> availableImages = workerThread->getAvailableImages();
    QListIterator<QFileInfo> availableImagesIterator(availableImages);
    while (availableImagesIterator.hasNext()) {
        QFileInfo availableImage = availableImagesIterator.next();
        images.append(availableImage.absoluteFilePath());
    }
    endResetModel();
    emit searchFinished();
}

void ImagesModel::handleImageProcessingComplete()
{
    QVariantList temporaryFiles = imageProcessor->getTemporaryFiles();
    QListIterator<QVariant> temporaryFilesIterator(temporaryFiles);
    while (temporaryFilesIterator.hasNext()) {
        QString temporaryFileName = temporaryFilesIterator.next().toString();
        QFileInfo temporaryFileInfo(temporaryFileName);
        selectedImagesSize += temporaryFileInfo.size();
        uploadImagesBytesSent.insert(temporaryFileName, 0);
    }
    qDebug() << "Total images size: " + QString::number(selectedImagesSize);

    temporaryFilesIterator.toFront();
    while (temporaryFilesIterator.hasNext()) {
        twitterApi->uploadImage(temporaryFilesIterator.next().toString());
    }
}

void ImagesModel::handleImageUploadSuccessful(const QString &fileName, const QVariantMap &result)
{
    QString mediaId = result.value("media_id_string").toString();
    qDebug() << "ImagesModel::handleImageUploadSuccessful" << fileName << mediaId;
    uploadedImages.insert(fileName, result);
    QString originalFile = imageProcessor->getFileMapping(fileName);
    if (this->imageDescriptions.contains(originalFile)) {
        twitterApi->uploadImageDescription(mediaId, this->imageDescriptions.value(originalFile, QString()));
    }
    this->processUploadCompleted();
}

void ImagesModel::handleImageUploadError(const QString &fileName, const QString &errorMessage)
{
    qDebug() << "ImagesModel::handleImageUploadError" << fileName;
    qDebug() << "Error uploading file " + fileName;
    this->clearModel();
    emit uploadFailed(errorMessage);
}

void ImagesModel::handleImageUploadStatus(const QString &fileName, qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "ImagesModel::handleImageUploadStatus" << fileName << QString::number(bytesSent) << QString::number(bytesTotal);
    if (bytesTotal == 0) {
        return;
    }
    uploadImagesBytesSent.insert(fileName, bytesSent);
    QMapIterator<QString, qint64> uploadStatusIterator(uploadImagesBytesSent);
    qint64 totalUploadedBytes = 0;
    while (uploadStatusIterator.hasNext()) {
        totalUploadedBytes += uploadStatusIterator.next().value();
    }
    qDebug() << "Uploaded bytes: " + QString::number(totalUploadedBytes) + " out of " + QString::number(selectedImagesSize);
    int percentCompleted = 100 * totalUploadedBytes / selectedImagesSize;
    emit uploadProgress(percentCompleted);
}

void ImagesModel::handleImageDescriptionUploadSuccessful(const QString &mediaId)
{
    qDebug() << "ImagesModel::handleImageDescriptionUploadSuccessful" << mediaId;
    this->uploadedImageDescriptions.append(mediaId);
    this->processUploadCompleted();
}

void ImagesModel::handleImageDescriptionUploadError(const QString &mediaId, const QString &errorMessage)
{
    qDebug() << "ImagesModel::handleImageDescriptionUploadError" << mediaId;
    qDebug() << "Error uploading image description: " + errorMessage;
    this->clearModel();
    emit uploadFailed(errorMessage);
}

