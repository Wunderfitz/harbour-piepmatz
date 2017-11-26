#ifndef DOWNLOADRESPONSEHANDLER_H
#define DOWNLOADRESPONSEHANDLER_H

#include "twitterapi.h"
#include <QObject>
#include <QString>

class DownloadResponseHandler : public QObject
{
    Q_OBJECT
public:
    DownloadResponseHandler(const QString &fileName, TwitterApi *twitterApi);

public slots:
    void handleDownloadProgress(qint64 bytesSent, qint64 bytesTotal);
    void handleDownloadError(QNetworkReply::NetworkError error);
    void handleDownloadFinished();

private:
    QString fileName;
    TwitterApi *twitterApi;
};

#endif // DOWNLOADRESPONSEHANDLER_H
