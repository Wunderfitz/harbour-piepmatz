#include "imagesmodel.h"
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

ImagesModel::ImagesModel()
{

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
    QStringList supportedImageTypes;
    supportedImageTypes.append("*.jpg");
    supportedImageTypes.append("*.gif");
    supportedImageTypes.append("*.png");
    QDirIterator picturesDirectoryIterator(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (picturesDirectoryIterator.hasNext()) {
        // Collect image names, sort according to timestamp descending and append to images model
        QFileInfo fileInformation(picturesDirectoryIterator.next());
        qDebug() << fileInformation.absoluteFilePath() + ", last modified: " + fileInformation.lastModified().toString();
    }
}
