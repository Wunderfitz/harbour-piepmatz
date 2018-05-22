#ifndef IMAGEMETADATARESPONSEHANDLER_H
#define IMAGEMETADATARESPONSEHANDLER_H

#include "twitterapi.h"
#include <QObject>
#include <QString>

class ImageMetadataResponseHandler : public QObject
{
    Q_OBJECT
public:
    ImageMetadataResponseHandler(const QString &mediaId, TwitterApi *twitterApi);

public slots:
    void handleImageMetadataUploadError(QNetworkReply::NetworkError error);
    void handleImageMetadataUploadFinished();

private:
    QString mediaId;
    TwitterApi *twitterApi;
};

#endif // IMAGEMETADATARESPONSEHANDLER_H
