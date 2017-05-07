#ifndef IMAGESSEARCHWORKER_H
#define IMAGESSEARCHWORKER_H

#include <QThread>
#include <QFileInfo>

class ImagesSearchWorker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        performSearch();
    }
public:
    ImagesSearchWorker();
    QList<QFileInfo> getAvailableImages();

signals:
    void searchFinished();

private:
    void performSearch();

    QList<QFileInfo> availableImages;
    QString getSDCardDirectory();
};

#endif // IMAGESSEARCHWORKER_H
