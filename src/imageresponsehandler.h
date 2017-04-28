#ifndef IMAGERESPONSEHANDLER_H
#define IMAGERESPONSEHANDLER_H

#include "twitterapi.h"
#include <QObject>
#include <QString>

class ImageResponseHandler : public QObject
{
    Q_OBJECT
public:
    ImageResponseHandler(const QString &fileName, TwitterApi *twitterApi);

public slots:
    void handleImageUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QString fileName;
    TwitterApi *twitterApi;
};

#endif // IMAGERESPONSEHANDLER_H
