#include "downloadresponsehandler.h"

#include <QStandardPaths>
#include <QFile>

DownloadResponseHandler::DownloadResponseHandler(const QString &fileName, TwitterApi *twitterApi)
{
    this->fileName = fileName;
    this->twitterApi = twitterApi;
}

void DownloadResponseHandler::handleDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << "DownloadResponseHandler::handleImageUploadProgress";
    emit twitterApi->downloadStatus(fileName, bytesSent, bytesTotal);
}

void DownloadResponseHandler::handleDownloadError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "DownloadResponseHandler::handleImageUploadError:" << (int)error << reply->errorString() << reply->readAll();
    emit twitterApi->downloadError(fileName, reply->errorString());
}

void DownloadResponseHandler::handleDownloadFinished()
{
    qDebug() << "DownloadResponseHandler::handleImageUploadFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QString filePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + this->fileName;
    QFile downloadedFile(filePath);
    if (downloadedFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Writing downloaded file to " + downloadedFile.fileName();
        downloadedFile.write(reply->readAll());
        downloadedFile.close();
        emit twitterApi->downloadSuccessful(filePath);
    } else {
        emit twitterApi->downloadError(fileName, "Error storing file at " + filePath);
    }

}
