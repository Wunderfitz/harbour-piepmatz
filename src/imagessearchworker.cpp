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
    supportedImageTypes.append("*.jpeg");
    supportedImageTypes.append("*.gif");
    supportedImageTypes.append("*.png");
    QDirIterator picturesDirectoryIterator(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (picturesDirectoryIterator.hasNext()) {
        QFileInfo fileInformation(picturesDirectoryIterator.next());
        availableImages.append(fileInformation);
    }
    QDirIterator downloadDirectoryIterator(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (downloadDirectoryIterator.hasNext()) {
        QFileInfo fileInformation(downloadDirectoryIterator.next());
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
    // Some people store stuff in Adroid directories...
    QDir androidRoot(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/android_storage");
    if (androidRoot.exists()) {
        qDebug() << "OK, seems that we have some Android stuff on the device...";
        QDir androidPictures(androidRoot.absolutePath() + "/Pictures");
        if (androidPictures.exists()) {
            qDebug() << "Searching Android Pictures folder...";
            QDirIterator androidPicturesDirectoryIterator(androidPictures.absolutePath(), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
            while (androidPicturesDirectoryIterator.hasNext()) {
                QFileInfo fileInformation(androidPicturesDirectoryIterator.next());
                availableImages.append(fileInformation);
            }
        }
        QDir androidDownload(androidRoot.absolutePath() + "/Download");
        if (androidDownload.exists()) {
            qDebug() << "Searching Android Download folder...";
            QDirIterator androidDownloadDirectoryIterator(androidDownload.absolutePath(), supportedImageTypes, QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
            while (androidDownloadDirectoryIterator.hasNext()) {
                QFileInfo fileInformation(androidDownloadDirectoryIterator.next());
                availableImages.append(fileInformation);
            }
        }
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
