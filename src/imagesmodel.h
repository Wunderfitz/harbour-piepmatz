#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include "imagessearchworker.h"
#include "twitterapi.h"
#include <QAbstractListModel>
#include <QFileInfo>
#include <QVariantList>

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
    Q_INVOKABLE void uploadSelectedImages();

signals:
    void searchFinished();
    void imagesSelected();

private slots:
    void handleSearchFinished();

private:
    QVariantList images;
    QVariantList selectedImages;
    ImagesSearchWorker *workerThread;
    TwitterApi *twitterApi;
    bool searchInProgress;
};

#endif // IMAGESMODEL_H
