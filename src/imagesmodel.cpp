#include "imagesmodel.h"

#include <QDebug>
#include <QList>
#include <QListIterator>
#include <QFileInfo>

ImagesModel::ImagesModel()
{
    workerThread = new ImagesSearchWorker();
    connect(workerThread, SIGNAL(searchFinished()), this, SLOT(handleSearchFinished()));
}

int ImagesModel::rowCount(const QModelIndex &) const
{
    return images.size();
}

QVariant ImagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
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
}

QVariantList ImagesModel::getSelectedImages()
{
    return this->selectedImages;
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

