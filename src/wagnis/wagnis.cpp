#include "wagnis.h"

#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QUrlQuery>
#include <QUuid>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

Wagnis::Wagnis(QNetworkAccessManager *manager, const QString &applicationName, const QString applicationVersion, QObject *parent) : QObject(parent)
{
    qDebug() << "Initializing Wagnis...";
    this->manager = manager;
    this->applicationName = applicationName;
    this->applicationVersion = applicationVersion;
    getIpInfo();

    /* Load the human readable error strings for libcrypto */
    ERR_load_crypto_strings();
    /* Load all digest and cipher algorithms */
    OpenSSL_add_all_algorithms();
    /* Load config file, and other important initialisation */
    OPENSSL_config(NULL);

    generateId();
    readRegistration();

}

Wagnis::~Wagnis()
{
    qDebug() << "Shutting down Wagnis...";
    /* Removes all digests and ciphers */
    EVP_cleanup();
    /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
    CRYPTO_cleanup_all_ex_data();
    /* Remove error strings */
    ERR_free_strings();
}

QString Wagnis::getId()
{
    return this->wagnisId;
}

void Wagnis::registerApplication()
{
    qDebug() << "Wagnis::registerApplication";
    QUrl url = QUrl(API_REGISTRATION);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, MIME_TYPE_JSON);

    QJsonObject jsonPayloadObject;
    jsonPayloadObject.insert("id", wagnisId);
    jsonPayloadObject.insert("country", ipInfo.value("country").toString());
    jsonPayloadObject.insert("application", this->applicationName);
    jsonPayloadObject.insert("version", this->applicationVersion);

    QJsonDocument requestDocument(jsonPayloadObject);
    QByteArray jsonAsByteArray = requestDocument.toJson();
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonAsByteArray.size()));

    QNetworkReply *reply = manager->post(request, jsonAsByteArray);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRegisterApplicationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRegisterApplicationFinished()));
}

void Wagnis::getApplicationRegistration()
{
    qDebug() << "Wagnis::getApplicationRegistration";
    QUrl url = QUrl(API_REGISTRATION);
    QUrlQuery urlQuery = QUrlQuery();
    urlQuery.addQueryItem("id", this->wagnisId);
    urlQuery.addQueryItem("application", this->applicationName);
    url.setQuery(urlQuery);
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetApplicationRegistrationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRegisterApplicationFinished()));
}

bool Wagnis::isRegistered()
{
    return !validatedRegistration.isEmpty();
}

bool Wagnis::hasFeature(const QString &featureName)
{
    qDebug() << "Wagnis::hasFeature" << featureName;
    if (!validatedRegistration.isEmpty()) {
        QVariantList registeredFeatures = validatedRegistration.value("features").toList();
        QListIterator<QVariant> featuresIterator(registeredFeatures);
        while (featuresIterator.hasNext()) {
            if (featuresIterator.next().toMap().value("name") == featureName) {
                return true;
            }
        }
    }
    return false;
}

void Wagnis::sendSurvey(const QString &answer, const QString &otherId)
{
    qDebug() << "Wagnis::sendSurvey" << answer << otherId;
    QUrl url = QUrl(API_SURVEY);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, MIME_TYPE_JSON);

    QJsonObject jsonPayloadObject;
    jsonPayloadObject.insert("id", wagnisId);
    jsonPayloadObject.insert("application", this->applicationName);
    jsonPayloadObject.insert("answer", answer);
    if (!otherId.isEmpty()) {
        jsonPayloadObject.insert("other_id", otherId);
    }

    QJsonDocument requestDocument(jsonPayloadObject);
    QByteArray jsonAsByteArray = requestDocument.toJson();
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(jsonAsByteArray.size()));

    QNetworkReply *reply = manager->post(request, jsonAsByteArray);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleRegisterApplicationError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleRegisterApplicationFinished()));
}

QVariantMap Wagnis::getRegistrationData()
{
    qDebug() << "Wagnis::getRegistrationData";
    return this->validatedRegistration;
}

bool Wagnis::inTestingPeriod()
{
    qDebug() << "Wagnis::inTestingPeriod";
    return this->remainingSeconds > 0;
}

int Wagnis::getRemainingTime()
{
    qDebug() << "Wagnis::getRemainingTime";
    return this->remainingSeconds;
}

void Wagnis::generateId()
{
    // We try to use the unique device ID. If we can't determine this ID, a random key is used...
    // Unique device ID determination copied from the QtSystems module of the Qt Toolkit
    QString temporaryUUID;
    if (temporaryUUID.isEmpty()) {
        QFile file(QStringLiteral("/sys/devices/virtual/dmi/id/product_uuid"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 36) {
                temporaryUUID = id;
            }
            file.close();
        }
    }
    if (temporaryUUID.isEmpty()) {
        QFile file(QStringLiteral("/etc/machine-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                temporaryUUID = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    if (temporaryUUID.isEmpty()) {
        QFile file(QStringLiteral("/etc/unique-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                temporaryUUID = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    if (temporaryUUID.isEmpty()) {
        QFile file(QStringLiteral("/var/lib/dbus/machine-id"));
        if (file.open(QIODevice::ReadOnly)) {
            QString id = QString::fromLocal8Bit(file.readAll().simplified().data());
            if (id.length() == 32) {
                temporaryUUID = id.insert(8,'-').insert(13,'-').insert(18,'-').insert(23,'-');
            }
            file.close();
        }
    }
    if (temporaryUUID.isEmpty()) {
        qDebug() << "FATAL: Unable to obtain unique device ID!";
        temporaryUUID = "n/a";
    }

    QCryptographicHash idHash(QCryptographicHash::Sha256);
    idHash.addData(temporaryUUID.toUtf8());
    idHash.addData(this->applicationName.toUtf8());
    idHash.addData(QString("Wagnis").toUtf8());
    idHash.result().toHex();

    QString uidHash = QString::fromUtf8(idHash.result().toHex());
    qDebug() << "Hash: " + uidHash;
    wagnisId = uidHash.left(4) + "-" + uidHash.mid(4,4) + "-" + uidHash.mid(8,4) + "-" + uidHash.mid(12,4);
    qDebug() << "[Wagnis] ID: " + wagnisId;
}

void Wagnis::readRegistration()
{
    qDebug() << "Wagnis::readRegistration";

    QFile registrationFile(getRegistrationFileName());
    if (registrationFile.exists() && registrationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[Wagnis] Reading registration file from " + registrationFile.fileName();
        QByteArray fileRawContent = registrationFile.readAll();
        validateRegistrationData(fileRawContent, false);
        registrationFile.close();
    }
}

/*
 * Signature validation by Ian Bull as outlined in his tutorial on
 * https://eclipsesource.com/de/blogs/2016/09/07/tutorial-code-signing-and-verification-with-openssl/
 * Thank you very much - in other programming languages this is a single line of code...
 */
RSA* createPublicRSA(std::string key)
{
  RSA *rsa = NULL;
  BIO *keybio;
  const char* c_string = key.c_str();
  keybio = BIO_new_mem_buf((void*)c_string, -1);
  if (keybio == NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
  return rsa;
}

bool verifySignature(RSA* rsa, unsigned char* rawSignature, size_t rawSignatureLength, const char* rawContent, size_t rawContentLength, bool* authentic)
{
  *authentic = false;
  EVP_PKEY* pubKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, rsa);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();
  if (EVP_DigestVerifyInit(m_RSAVerifyCtx, NULL, EVP_sha256(), NULL, pubKey) <= 0) {
    return false;
  }
  if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, rawContent, rawContentLength) <= 0) {
    return false;
  }
  int AuthStatus = EVP_DigestVerifyFinal(m_RSAVerifyCtx, rawSignature, rawSignatureLength);
  if (AuthStatus == 1) {
    *authentic = true;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return true;
  } else if(AuthStatus == 0){
    *authentic = false;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return true;
  } else{
    *authentic = false;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return false;
  }
}

void Wagnis::validateRegistrationData(const QByteArray &registrationData, const bool &saveData)
{
    qDebug() << "Wagnis::validateRegistrationData";
    QJsonDocument jsonDocument = QJsonDocument::fromJson(registrationData);
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        QVariantMap registrationInformation = responseObject.toVariantMap();
        QString registrationContent = registrationInformation.value("registration").toString();
        QString registrationSignature = registrationInformation.value("signature").toString();
        qDebug() << "[Wagnis] Payload: " << registrationContent;
        qDebug() << "[Wagnis] Signature: " << registrationSignature;

        QJsonDocument registrationContentJson = QJsonDocument::fromJson(registrationContent.toUtf8());
        bool wagnisIdVerified = false;
        if (registrationContentJson.isObject()) {
            QVariantMap wagnisInformation = registrationContentJson.object().toVariantMap();
            if ( wagnisInformation.value("id").toString() == this->wagnisId ) {
                qDebug() << "[Wagnis] ID verified!";
                wagnisIdVerified = true;
            }
        }

        if (wagnisIdVerified && isSignatureValid(registrationContent, registrationSignature)) {
            qDebug() << "[Wagnis] Registration valid!";
            this->validatedRegistration = registrationContentJson.object().toVariantMap();

            QDateTime registrationTime = QDateTime::fromString(this->validatedRegistration.value("timestamp").toString(), "yyyy-MM-dd hh:mm:ss");
            QDateTime currentTime = QDateTime::currentDateTime();
            this->remainingSeconds = 1209600 - registrationTime.secsTo(currentTime);
            if (this->remainingSeconds < 0 ) {
                this->remainingSeconds = 0;
            }

            emit registrationValid(this->validatedRegistration);
            if (saveData) {
                QFile registrationFile(getRegistrationFileName());
                if (registrationFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    qDebug() << "[Wagnis] Writing registration file to " + registrationFile.fileName();
                    registrationFile.write(registrationData);
                    registrationFile.close();
                }
            }
        } else {
            qDebug() << "[Wagnis] Registration INVALID!";
            emit registrationInvalid();
        }
    }
}

void Wagnis::getIpInfo()
{
    qDebug() << "Wagnis::getIpInfo";
    QUrl url = QUrl("https://ipinfo.io/json");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleGetIpInfoError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleGetIpInfoFinished()));
}

QString Wagnis::getRegistrationFileName()
{
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/" + this->applicationName + "/" + "registration.json";
}

bool Wagnis::isSignatureValid(const QString &content, const QString &signature)
{
    qDebug() << "Wagnis::isSignatureValid";
    RSA* publicRSA = createPublicRSA(PUBLIC_KEY);
    QByteArray signatureigBase64 = signature.toLatin1();
    QByteArray rawData = QByteArray::fromBase64(signatureigBase64);
    bool authentic;
    bool result = verifySignature(publicRSA, reinterpret_cast<unsigned char*>(rawData.data()), rawData.length(), content.toStdString().c_str(), content.toStdString().length(), &authentic);
    return authentic & result;
}

void Wagnis::handleGetIpInfoError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Wagnis::handleGetIpInfoError:" << (int)error << reply->errorString() << reply->readAll();
}

void Wagnis::handleGetIpInfoFinished()
{
    qDebug() << "Wagnis::handleGetIpInfoFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        this->ipInfo = responseObject.toVariantMap();
        qDebug() << "[Wagnis] Country: " + ipInfo.value("country").toString();
    }
}

void Wagnis::handleRegisterApplicationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Wagnis::handleRegisterApplicationError:" << (int)error << reply->errorString() << reply->readAll();
    if (error == QNetworkReply::ContentConflictError) { // Conflict = Registration already there!
        qDebug() << "[Wagnis] Installation already registered!";
        this->getApplicationRegistration();
    } else {
        emit registrationError(QString::number((int)error) + "Return code: " + " - " + reply->errorString());
    }
}

void Wagnis::handleRegisterApplicationFinished()
{
    qDebug() << "Wagnis::handleRegisterApplicationFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QByteArray registrationReply = reply->readAll();
    validateRegistrationData(registrationReply, true);
}

void Wagnis::handleSendSurveyError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Wagnis::handleSendSurveyError:" << (int)error << reply->errorString() << reply->readAll();
    emit registrationError(QString::number((int)error) + "Return code: " + " - " + reply->errorString());
}

void Wagnis::handleSendSurveyFinished()
{
    qDebug() << "Wagnis::handleSendSurveyFinished";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        return;
    }

    QByteArray registrationReply = reply->readAll();
    validateRegistrationData(registrationReply, true);
}

void Wagnis::handleGetApplicationRegistrationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Wagnis::handleGetApplicationRegistrationError:" << (int)error << reply->errorString() << reply->readAll();
    emit registrationError(QString::number((int)error) + "Return code: " + " - " + reply->errorString());
}
