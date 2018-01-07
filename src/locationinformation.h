#ifndef LOCATIONINFORMATION_H
#define LOCATIONINFORMATION_H

#include <QGeoSatelliteInfoSource>
#include <QGeoPositionInfoSource>
#include <QVariantMap>
#include <QSettings>

class LocationInformation : public QObject
{
    Q_OBJECT
public:
    explicit LocationInformation(QObject *parent = 0);

    Q_INVOKABLE bool hasInformation();
    Q_INVOKABLE bool isEnabled();
    Q_INVOKABLE void setEnabled(const bool &enabled);
    Q_INVOKABLE QVariantMap getCurrentPosition();
    Q_INVOKABLE void updateInformation();

signals:
    void newPosition(const QVariantMap &coordinate);
public slots:

private slots:
    void positionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *source;
    QSettings settings;
    QVariantMap currentPosition;
    int updateCount;
    bool enabled;
};

#endif // LOCATIONINFORMATION_H
