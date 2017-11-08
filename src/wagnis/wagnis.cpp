#include "wagnis.h"

#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QUrlQuery>
#include <QUuid>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>

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
    // TODO: Implement the critical stuff here... ;)
    return false;
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
    idHash.result().toHex();

    QString uidHash = QString::fromUtf8(idHash.result().toHex());
    qDebug() << "Hash: " + uidHash;
    wagnisId = uidHash.left(4) + "-" + uidHash.mid(4,4) + "-" + uidHash.mid(8,4) + "-" + uidHash.mid(12,4);
    qDebug() << "[Wagnis] ID: " + wagnisId;
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

size_t calcDecodeLength(const char* b64input) {
  size_t len = strlen(b64input), padding = 0;
  if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
    padding = 2;
  else if (b64input[len-1] == '=') //last char is =
    padding = 1;
  return (len*3)/4 - padding;
}

void Base64Decode(const char* b64message, unsigned char** buffer, size_t* length) {
  BIO *bio, *b64;
  int decodeLen = calcDecodeLength(b64message);
  *buffer = (unsigned char*)malloc(decodeLen + 1);
  (*buffer)[decodeLen] = '\0';
  bio = BIO_new_mem_buf(b64message, -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);
  *length = BIO_read(bio, *buffer, strlen(b64message));
  BIO_free_all(bio);
}

RSA* createPublicRSA(std::string key) {
  RSA *rsa = NULL;
  BIO *keybio;
  const char* c_string = key.c_str();
  keybio = BIO_new_mem_buf((void*)c_string, -1);
  if (keybio==NULL) {
      return 0;
  }
  rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
  return rsa;
}

bool RSAVerifySignature( RSA* rsa,
                         unsigned char* MsgHash,
                         size_t MsgHashLen,
                         const char* Msg,
                         size_t MsgLen,
                         bool* Authentic) {
  *Authentic = false;
  EVP_PKEY* pubKey  = EVP_PKEY_new();
  EVP_PKEY_assign_RSA(pubKey, rsa);
  EVP_MD_CTX* m_RSAVerifyCtx = EVP_MD_CTX_create();
  if (EVP_DigestVerifyInit(m_RSAVerifyCtx,NULL, EVP_sha256(),NULL,pubKey)<=0) {
    return false;
  }
  if (EVP_DigestVerifyUpdate(m_RSAVerifyCtx, Msg, MsgLen) <= 0) {
    return false;
  }
  int AuthStatus = EVP_DigestVerifyFinal(m_RSAVerifyCtx, MsgHash, MsgHashLen);
  if (AuthStatus==1) {
    *Authentic = true;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return true;
  } else if(AuthStatus==0){
    *Authentic = false;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return true;
  } else{
    *Authentic = false;
    EVP_MD_CTX_cleanup(m_RSAVerifyCtx);
    return false;
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

    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
    if (jsonDocument.isObject()) {
        QJsonObject responseObject = jsonDocument.object();
        QVariantMap registrationInformation = responseObject.toVariantMap();
        QString registrationData = registrationInformation.value("registration").toString();
        QString registrationSignature = registrationInformation.value("signature").toString();
        qDebug() << "Payload: " << registrationData;
        qDebug() << "Signature: " << registrationSignature;

        // TODO: Signature validation comes here... Send signals for success and failure!
        //int verified = RSA_verify(NID_sha256, registrationData.toStdString().c_str(), registrationData.size(), registrationSignature.toStdString().c_str(), registrationSignature.size(),???);

        RSA* publicRSA = createPublicRSA(PUBLIC_KEY);
        unsigned char* encMessage;
        size_t encMessageLength;
        bool authentic;
        Base64Decode(registrationSignature.toStdString().c_str(), &encMessage, &encMessageLength);
        bool result = RSAVerifySignature(publicRSA, encMessage, encMessageLength, registrationData.toStdString().c_str(), registrationData.toStdString().length(), &authentic);
        qDebug() << "Signatur OK? " + QString::number(result & authentic);
    }
}

void Wagnis::handleGetApplicationRegistrationError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    qWarning() << "Wagnis::handleGetApplicationRegistrationError:" << (int)error << reply->errorString() << reply->readAll();
    emit registrationError(QString::number((int)error) + "Return code: " + " - " + reply->errorString());
}
