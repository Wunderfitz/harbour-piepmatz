#include "locationinformation.h"

#include <QDebug>

const char SETTINGS_POSITIONING[] = "settings/positioning";

LocationInformation::LocationInformation(QObject *parent) : QObject(parent), settings("harbour-piepmatz", "settings")
{
    qDebug() << "Initializing location services...";
    source = QGeoPositionInfoSource::createDefaultSource(this);
    updateCount = 0;
    if (source && isEnabled()) {
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

bool LocationInformation::isEnabled()
{
    return settings.value(SETTINGS_POSITIONING, true).toBool();
}

void LocationInformation::setEnabled(const bool &enabled)
{
    if (isEnabled() != enabled) {
        if (enabled) {
            updateInformation();
        } else {
            source = 0;
            this->currentPosition.clear();
        }
    }
    settings.setValue(SETTINGS_POSITIONING, enabled);
}

QVariantMap LocationInformation::getCurrentPosition()
{
    return this->currentPosition;
}

void LocationInformation::updateInformation()
{
    qDebug() << "LocationInformation::updateInformation";
    source = QGeoPositionInfoSource::createDefaultSource(this);
    updateCount = 0;
    if (source && isEnabled()) {
        qDebug() << "Success: We can (again) obtain location information!";
        connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(positionUpdated(QGeoPositionInfo)));
        source->startUpdates();
    }
}

void LocationInformation::positionUpdated(const QGeoPositionInfo &info)
{
    qDebug() << "LocationInformation::positionUpdated" << info;
    updateCount++;
    if (updateCount >= 10) {
        qDebug() << "GPS fix taking too long, stopping updates...";
        source->stopUpdates();
        return;
    }
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
