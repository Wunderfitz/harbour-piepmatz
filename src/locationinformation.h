/*
    Copyright (C) 2017-20 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
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
