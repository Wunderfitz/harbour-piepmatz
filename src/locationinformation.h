#ifndef LOCATIONINFORMATION_H
#define LOCATIONINFORMATION_H

#include <QGeoSatelliteInfoSource>
#include <QGeoPositionInfoSource>
#include <QVariantMap>

class LocationInformation : public QObject
{
    Q_OBJECT
public:
    explicit LocationInformation(QObject *parent = 0);

    Q_INVOKABLE bool hasInformation();
    Q_INVOKABLE QVariantMap getCurrentPosition();
    Q_INVOKABLE void updateInformation();

signals:
    void newPosition(const QVariantMap &coordinate);
public slots:

private slots:
    void positionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *source;
    QVariantMap currentPosition;
    int updateCount;
};

#endif // LOCATIONINFORMATION_H
