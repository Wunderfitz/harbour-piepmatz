#ifndef WAGNIS_H
#define WAGNIS_H

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QVariantMap>

const char MIME_TYPE_JSON[] = "application/json";
const char API_REGISTRATION[] = "https://ssl.webpack.de/ygriega.de/wagnis/registration.php";
const char PUBLIC_KEY[] = "-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAMbkCojgMgKFZOgit5rDPPQC5jrvy7tC\n3teqxVVb054WemtJ++x8buc4ZgtxpmDJt9VgMS6ExQmC2808PNoyxD0CAwEAAQ==\n-----END PUBLIC KEY-----";

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QNetworkAccessManager *manager, const QString &applicationName, const QString applicationVersion, QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();
    Q_INVOKABLE void registerApplication();
    Q_INVOKABLE void getApplicationRegistration();
    Q_INVOKABLE bool isRegistered();
    Q_INVOKABLE bool hasFeature(const QString &featureName);

signals:
    void registrationError(const QString &errorMessage);
    void registrationValid(const QVariantMap &registrationInformation);
    void registrationInvalid();

public slots:

private:

    void generateId();
    void readRegistration();
    void validateRegistrationData(const QByteArray &registrationData, const bool &saveData);
    void getIpInfo();
    QString getRegistrationFileName();
    bool isSignatureValid(const QString &content, const QString &signature);

    QString applicationName;
    QString applicationVersion;
    QString wagnisId;
    QVariantMap ipInfo;
    QVariantMap validatedRegistration;
    QNetworkAccessManager *manager;

private slots:
    void handleGetIpInfoError(QNetworkReply::NetworkError error);
    void handleGetIpInfoFinished();
    void handleRegisterApplicationError(QNetworkReply::NetworkError error);
    void handleRegisterApplicationFinished();
    void handleGetApplicationRegistrationError(QNetworkReply::NetworkError error);
};

#endif // WAGNIS_H
