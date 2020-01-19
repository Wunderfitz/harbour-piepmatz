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
#include "imageprocessor.h"

#include <QListIterator>
#include <QImageReader>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

ImageProcessor::ImageProcessor()
{

}

void ImageProcessor::setSelectedImages(const QVariantList &selectedImages)
{
    this->selectedImages.clear();
    this->fileMappings.clear();
    this->selectedImages.append(selectedImages);
}

QVariantList ImageProcessor::getTemporaryFiles()
{
    return this->temporaryFiles;
}

void ImageProcessor::removeTemporaryFiles()
{
    qDebug() << "ImageProcessor::removeTemporaryFiles";
    QListIterator<QVariant> temporaryFilesIterator(temporaryFiles);
    while (temporaryFilesIterator.hasNext()) {
        QString temporaryFileName = temporaryFilesIterator.next().toString();
        QFile temporaryFile(temporaryFileName);
        temporaryFile.remove();
    }
}

QString ImageProcessor::getFileMapping(const QString &fileName)
{
    return this->fileMappings.value(fileName, QString());
}

void ImageProcessor::processImages()
{
    qDebug() << "ImageProcessor::processImages";
    this->temporaryFiles.clear();
    QListIterator<QVariant> selectedImagesIterator(selectedImages);
    while (selectedImagesIterator.hasNext()) {
        QString selectedImageFileName = selectedImagesIterator.next().toString();
        QImageReader imageReader;
        imageReader.setFileName(selectedImageFileName);
        imageReader.setAutoTransform(true);
        QImage myImage = imageReader.read();
        QString newImageFileName = selectedImageFileName;
        QString escapedFileName = getTempDirectory() + QLatin1Char('/') + newImageFileName.replace(QLatin1Char('/'), QLatin1Char('_'));
        qDebug() << "Processing file " + escapedFileName;
        myImage.save(escapedFileName, "JPG");
        this->temporaryFiles.append(escapedFileName);
        qDebug() << "Image mapping: " << escapedFileName << " - " << selectedImageFileName;
        this->fileMappings.insert(escapedFileName, selectedImageFileName);
    }
    emit processingComplete();
}

QString ImageProcessor::getTempDirectory()
{
    qDebug() << "ImageProcessor::getTempDirectory";
    QString tempDirectoryString = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/harbour-piepmatz";
    return getDirectory(tempDirectoryString);
}

QString ImageProcessor::getDirectory(const QString &directoryString)
{
    qDebug() << "ImageProcessor::getDirectory";
    QString myDirectoryString = directoryString;
    QDir myDirectory(directoryString);
    if (!myDirectory.exists()) {
        qDebug() << "Creating directory " + directoryString;
        if (myDirectory.mkdir(directoryString)) {
            qDebug() << "Directory " + directoryString + " successfully created!";
        } else {
            qDebug() << "Error creating directory " + directoryString + "!";
            myDirectoryString = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }
    return myDirectoryString;
}
