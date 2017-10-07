#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>
#include <QVariantList>

class ImageProcessor : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        processImages();
    }
public:
    ImageProcessor();
    void setSelectedImages(const QVariantList &selectedImages);
    QVariantList getTemporaryFiles();
    void removeTemporaryFiles();

signals:
    void processingComplete();

private:
    QVariantList selectedImages;
    QVariantList temporaryFiles;

    void processImages();
    QString getDirectory(const QString &directoryString);
    QString getTempDirectory();
};

#endif // IMAGEPROCESSOR_H
