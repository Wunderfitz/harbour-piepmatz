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
        QString escapedFileName = getTempDirectory() + "/" + selectedImageFileName.replace("/", "_");
        qDebug() << "Processing file " + escapedFileName;
        myImage.save(escapedFileName, "JPG");
        this->temporaryFiles.append(escapedFileName);
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
