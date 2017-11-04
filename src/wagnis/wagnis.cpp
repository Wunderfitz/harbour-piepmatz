#include "wagnis.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <QDebug>
#include <QFile>
#include <QUuid>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>

Wagnis::Wagnis(QNetworkAccessManager *manager, QObject *parent) : QObject(parent)
{
    qDebug() << "Initializing Wagnis...";
    this->manager = manager;
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
    idHash.addData("Piepmatz");
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
