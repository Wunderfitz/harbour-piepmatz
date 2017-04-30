#include "imagessearchworker.h"

#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QList>
#include <QStandardPaths>
#include <QStringList>

ImagesSearchWorker::ImagesSearchWorker()
{

}

QList<QFileInfo> ImagesSearchWorker::getAvailableImages()
{
    qDebug() << "ImagesSearchWorker::getAvailableImages";
    return availableImages;
}

bool reverseDateTime(const QFileInfo &fileInfo1, const QFileInfo &fileInfo2)
{
    return fileInfo1.lastModified() >  fileInfo2.lastModified();
}

void ImagesSearchWorker::performSearch()
{
    qDebug() << "ImagesSearchWorker::performSearch";
    availableImages.clear();
    QStringList supportedImageTypes;
    supportedImageTypes.append("*.jpg");
    supportedImageTypes.append("*.gif");
    supportedImageTypes.append("*.png");
    QDirIterator picturesDirectoryIterator(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (picturesDirectoryIterator.hasNext()) {
        // Collect image names, sort according to timestamp descending and append to images model
        QFileInfo fileInformation(picturesDirectoryIterator.next());
        availableImages.append(fileInformation);
    }
    qSort(availableImages.begin(), availableImages.end(), reverseDateTime);
    emit searchFinished();
}
