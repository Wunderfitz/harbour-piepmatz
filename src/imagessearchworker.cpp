#include "imagessearchworker.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
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
        QFileInfo fileInformation(picturesDirectoryIterator.next());
        availableImages.append(fileInformation);
    }
    QString sdCardDirectory = getSDCardDirectory();
    if (sdCardDirectory != QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)) {
        qDebug() << "Reading pictures from SD card";
        QDirIterator sdCardPicturesDirectoryIterator(sdCardDirectory, supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
        while (sdCardPicturesDirectoryIterator.hasNext()) {
            QFileInfo fileInformation(sdCardPicturesDirectoryIterator.next());
            availableImages.append(fileInformation);
        }
    } else {
        qDebug() << "No SD card found :(";
    }
    qSort(availableImages.begin(), availableImages.end(), reverseDateTime);
    emit searchFinished();
}

QString ImagesSearchWorker::getSDCardDirectory()
{
    qDebug() << "ImagesSearchWorker::getSDCardDirectory";
    // Is SD card root existing?
    QDir sdCardRoot("/media/sdcard");
    if (sdCardRoot.exists()) {
        qDebug() << "SD card root existing :)";
        QDirIterator sdCardDirectoryIterator("/media/sdcard", QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::NoIteratorFlags);
        if (sdCardDirectoryIterator.hasNext()) {
            qDebug() << "SD card seems to be in the device :)";
            QDir sdCardPictures(sdCardDirectoryIterator.next() + "/Pictures/");
            if (sdCardPictures.exists()) {
                qDebug() << "SD card pictures folder detected: " + sdCardPictures.absolutePath();
                return sdCardPictures.absolutePath();
            }
        }
    }
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
}
