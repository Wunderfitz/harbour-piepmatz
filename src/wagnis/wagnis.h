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
#include <QJsonDocument>

const char MIME_TYPE_JSON[] = "application/json";
const char API_REGISTRATION[] = "https://ssl.webpack.de/ygriega.de/wagnis/v1/registration.php";
const char API_SURVEY[] = "https://ssl.webpack.de/ygriega.de/wagnis/v1/survey.php";
const char PUBLIC_KEY[] = "-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAJn498EnLBpy8qSfJ0xyflZA+rqKEtw+\nHESIOu9xCeORLXFn1UZMS+cl7hzf4n4V8BsfEBHSyYyxeNVQR0J+DdcCAwEAAQ==\n-----END PUBLIC KEY-----";

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QNetworkAccessManager *manager, const QString &applicationName, const QString applicationVersion, QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();
    Q_INVOKABLE QString getNewId();
    Q_INVOKABLE QString getCandidateRegistrationData();
    Q_INVOKABLE void registerApplication();
    Q_INVOKABLE void resetRegistration();
    Q_INVOKABLE void getApplicationRegistration();
    Q_INVOKABLE bool isRegistered();
    Q_INVOKABLE bool hasFeature(const QString &featureName);
    Q_INVOKABLE void sendSurvey(const QString &answer, const QString &otherId = "");
    Q_INVOKABLE QVariantMap getRegistrationData();
    Q_INVOKABLE bool inTestingPeriod();
    Q_INVOKABLE int getRemainingTime();

signals:
    void registrationError(const QString &errorMessage);
    void registrationValid(const QVariantMap &registrationInformation);
    void registrationInvalid();

public slots:

private:

    void generateNewId();
    void generateId();
    QStringList getImeis();
    QString getSerialNumber();
    QString getWifiMacAddress();
    QJsonDocument getRegistrationDocument();
    void readRegistration();
    void validateRegistrationData(const QByteArray &registrationData, const bool &saveData);
    void getIpInfo();
    QString getRegistrationFileName();
    bool isSignatureValid(const QString &content, const QString &signature);

    QString applicationName;
    QString applicationVersion;
    QString wagnisNewId;
    QString wagnisId;
    QVariantMap ipInfo;
    QVariantMap validatedRegistration;
    int remainingSeconds = 0;
    QNetworkAccessManager *manager;

private slots:
    void handleGetIpInfoError(QNetworkReply::NetworkError error);
    void handleGetIpInfoFinished();
    void handleRegisterApplicationError(QNetworkReply::NetworkError error);
    void handleRegisterApplicationFinished();
    void handleSendSurveyError(QNetworkReply::NetworkError error);
    void handleSendSurveyFinished();
    void handleGetApplicationRegistrationError(QNetworkReply::NetworkError error);
};

#endif // WAGNIS_H
