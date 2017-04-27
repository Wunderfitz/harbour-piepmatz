#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include "imagessearchworker.h"
#include <QAbstractListModel>
#include <QFileInfo>
#include <QVariantList>

class ImagesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    ImagesModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();
    Q_INVOKABLE void setSelectedImages(const QVariantList &selectedImages);
    Q_INVOKABLE QVariantList getSelectedImages();

signals:
    void searchFinished();
    void imagesSelected();

private slots:
    void handleSearchFinished();

private:
    QVariantList images;
    QVariantList selectedImages;
    ImagesSearchWorker *workerThread;

    bool searchInProgress;
};

#endif // IMAGESMODEL_H
