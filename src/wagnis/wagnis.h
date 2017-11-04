#ifndef WAGNIS_H
#define WAGNIS_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariantMap>

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QNetworkAccessManager *manager, QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();
    Q_INVOKABLE bool isRegistered();

signals:

public slots:

private:
    void generateId();
    void getIpInfo();
    QString wagnisId;
    QVariantMap ipInfo;
    QNetworkAccessManager *manager;

private slots:
    void handleGetIpInfoError(QNetworkReply::NetworkError error);
    void handleGetIpInfoFinished();
};

#endif // WAGNIS_H
