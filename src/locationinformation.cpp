#include "locationinformation.h"

#include <QDebug>

LocationInformation::LocationInformation(QObject *parent) : QObject(parent)
{
    qDebug() << "Initializing location services...";
    source = QGeoPositionInfoSource::createDefaultSource(this);
    if (source) {
        qDebug() << "Success: We can obtain location information!";
        connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
        source->startUpdates();
    } else {
        qDebug() << "Fail: No location information possible!";
    }
}

bool LocationInformation::hasInformation()
{
    return !this->currentPosition.isEmpty();
}

QVariantMap LocationInformation::getCurrentPosition()
{
    return this->currentPosition;
}

void LocationInformation::updateInformation()
{
    source->startUpdates();
}

void LocationInformation::positionUpdated(const QGeoPositionInfo &info)
{
    qDebug() << "LocationInformation::positionUpdated" << info;
    if (info.hasAttribute(QGeoPositionInfo::HorizontalAccuracy) && info.hasAttribute(QGeoPositionInfo::VerticalAccuracy)) {
        if (info.attribute(QGeoPositionInfo::HorizontalAccuracy) < 1000 && info.attribute(QGeoPositionInfo::VerticalAccuracy) < 1000) {
            qDebug() << "Position exact enough, stopping updates...";
            QGeoCoordinate geoCoordinate = info.coordinate();
            this->currentPosition.clear();
            this->currentPosition.insert("latitude", QString::number(geoCoordinate.latitude()));
            this->currentPosition.insert("longitude", QString::number(geoCoordinate.longitude()));
            emit newPosition(this->currentPosition);
            source->stopUpdates();
        }
    }
}
