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
const char API_REGISTRATION[] = "https://werkwolf.eu/wagnis/v1/registration.php";
const char API_SURVEY[] = "https://ssl.webpack.de/ygriega.de/wagnis/v1/survey.php";
const char API_CONTRIBUTION[] = "https://werkwolf.eu/wagnis/v1/contribution.php";
const char PUBLIC_KEY[] = "-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqpEDAUc4s/FAIBEhiMXV\ndn9YZNhab8tEB1wOt57muqOFv2YMenLUJSKkph5S2eMI81o3LuFlxEcAGUZucibJ\nYuy4Xbz96nlqmRg5pwd8XBRwVGML2WuEBDGqaXzz/Xjg+KVQbBQSIPSJruKe2kkx\ninm514LnlSVpzs9P4k9/Uc9d7q6C6JBk64UO73Y2PYImG5W8s2eI38EMQt44NgiP\npfxrhFkZNpAg3vJ+U9LhXPePU3XcFiDvcWZS+XCWCYlVVqHAEp5mUTzNy41uMKnI\nXLRvMcLsFuQB8Ojv49MABvUDnfWIRYHeDvIDt50cl7wYKKk2nXEcJ9j5+LAv25X7\nEwIDAQAB\n-----END PUBLIC KEY-----";

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QNetworkAccessManager *manager, const QString &applicationName, const QString applicationVersion, QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();
    Q_INVOKABLE QString getPocId();
    Q_INVOKABLE QString getCandidateRegistrationData();
    Q_INVOKABLE void registerApplication();
    Q_INVOKABLE void resetRegistration();
    Q_INVOKABLE void getApplicationRegistration();
    Q_INVOKABLE bool isRegistered();
    Q_INVOKABLE bool hasFeature(const QString &featureName);
    Q_INVOKABLE void sendSurvey(const QString &answer, const QString &otherId = "");
    Q_INVOKABLE void validateContribution(const QString &contributionKey);
    Q_INVOKABLE void getSurvey();
    Q_INVOKABLE QVariantMap getRegistrationData();
    Q_INVOKABLE bool inTestingPeriod();
    Q_INVOKABLE int getRemainingTime();

signals:
    void registrationError(const QString &errorMessage);
    void registrationValid(const QVariantMap &registrationInformation);
    void registrationInvalid();
    void surveyRetrieved(const QString &surveyAnswer);
    void contributionValidated();
    void contributionValidationError(const QString &errorMessage);

public slots:

private:

    void generatePocId();
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
    QString wagnisPocId;
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
    void handleGetSurveyFinished();
    void handleGetSurveyError(QNetworkReply::NetworkError error);
    void handleValidateContributionFinished();
    void handleValidateContributionError(QNetworkReply::NetworkError error);
};

#endif // WAGNIS_H
