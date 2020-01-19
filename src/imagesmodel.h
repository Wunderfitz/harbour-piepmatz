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
#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include "imagessearchworker.h"
#include "imageprocessor.h"
#include "twitterapi.h"
#include <QAbstractListModel>
#include <QFileInfo>
#include <QVariantList>
#include <QMap>

class ImagesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    ImagesModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();
    Q_INVOKABLE void setSelectedImages(const QVariantList &selectedImages);
    Q_INVOKABLE QVariantList getSelectedImages();
    Q_INVOKABLE void clearModel();
    Q_INVOKABLE void tweetWithSelectedImages(const QString &text, const QString &placeId = QString());
    Q_INVOKABLE void replyToTweetWithSelectedImages(const QString &text, const QString &replyToStatusId, const QString &placeId = QString());
    Q_INVOKABLE bool isTweetWithImagesInProgress();
    Q_INVOKABLE void setImageDescription(const QString &fileName, const QString &description);
    Q_INVOKABLE QString getImageDescription(const QString &fileName);

signals:
    void searchFinished();
    void imagesSelected();
    void uploadStarted();
    void uploadCompleted();
    void uploadFailed(const QString &errorMessage);
    void uploadProgress(const int percentCompleted);

private slots:
    void handleSearchFinished();
    void handleImageProcessingComplete();
    void handleImageUploadSuccessful(const QString &fileName, const QVariantMap &result);
    void handleImageUploadError(const QString &fileName, const QString &errorMessage);
    void handleImageUploadStatus(const QString &fileName, qint64 bytesSent, qint64 bytesTotal);
    void handleImageDescriptionUploadSuccessful(const QString &mediaId);
    void handleImageDescriptionUploadError(const QString &mediaId, const QString &errorMessage);

private:

    void uploadSelectedImages();
    void processUploadCompleted();

    QVariantList images;
    ImagesSearchWorker *workerThread;
    ImageProcessor *imageProcessor;

    TwitterApi *twitterApi;
    bool searchInProgress;

    QVariantList selectedImages;
    qint64 selectedImagesSize;
    QMap<QString, QString> imageDescriptions;
    QMap<QString, QVariantMap> uploadedImages;
    QVariantList uploadedImageDescriptions;
    QMap<QString, qint64> uploadImagesBytesSent;
    QString tweetText;
    QString replyToStatusId;
    QString tweetPlaceId;
    bool tweetWithImagesInProgress;
};

#endif // IMAGESMODEL_H
