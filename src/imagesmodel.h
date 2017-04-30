#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include "imagessearchworker.h"
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
    Q_INVOKABLE void tweetWithSelectedImages(const QString &text);
    Q_INVOKABLE void replyToTweetWithSelectedImages(const QString &text, const QString &replyToStatusId);
    Q_INVOKABLE bool isTweetWithImagesInProgress();

signals:
    void searchFinished();
    void imagesSelected();
    void uploadStarted();
    void uploadCompleted();
    void uploadFailed(const QString &errorMessage);
    void uploadProgress(const int percentCompleted);

private slots:
    void handleSearchFinished();
    void handleImageUploadSuccessful(const QString &fileName, const QVariantMap &result);
    void handleImageUploadError(const QString &fileName, const QString &errorMessage);
    void handleImageUploadStatus(const QString &fileName, qint64 bytesSent, qint64 bytesTotal);

private:

    void uploadSelectedImages();

    QVariantList images;
    ImagesSearchWorker *workerThread;
    TwitterApi *twitterApi;
    bool searchInProgress;

    QVariantList selectedImages;
    qint64 selectedImagesSize;
    QMap<QString, QVariantMap> uploadedImages;
    QMap<QString, qint64> uploadImagesBytesSent;
    QString tweetText;
    QString replyToStatusId;
    bool tweetWithImagesInProgress;
};

#endif // IMAGESMODEL_H
