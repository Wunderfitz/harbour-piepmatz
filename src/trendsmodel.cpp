/*
    Copyright (C) 2017-18 Sebastian J. Wolf

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
#include "trendsmodel.h"

#include <QListIterator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

TrendsModel::TrendsModel(TwitterApi *twitterApi)
{
    this->twitterApi = twitterApi;
    connect(twitterApi, &TwitterApi::getIpInfoSuccessful, this, &TrendsModel::handleGetIpInfoSuccessful);
    connect(twitterApi, &TwitterApi::placesForTrendsSuccessful, this, &TrendsModel::handlePlacesForTrendsSuccessful);
    connect(twitterApi, &TwitterApi::trendsSuccessful, this, &TrendsModel::handleTrendsSuccessful);

    QFile woeidsFile(QStringLiteral("/usr/share/harbour-piepmatz/qml/js/woeids-countries.json"));
    if (woeidsFile.open(QIODevice::ReadOnly)) {
        woeidsCountries = QJsonDocument::fromJson(woeidsFile.readAll()).array().toVariantList();
        woeidsFile.close();
    } else {
        qDebug() << "Unable to open WOEIDs file...";
    }
}

int TrendsModel::rowCount(const QModelIndex &) const
{
    return trends.size();
}

QVariant TrendsModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole) {
        return QVariant(trends.value(index.row()));
    }
    return QVariant();
}

void TrendsModel::handleGetIpInfoSuccessful(const QVariantMap &result)
{
    qDebug() << "TrendsModel::handleGetIpInfoSuccessful";
    this->country = result.value("country").toString();
    QString location = result.value("loc").toString();
    QStringList locationList = location.split(",");
    if (locationList.size() == 2) {
        qDebug() << "Getting trends for coordinates lat: " + locationList.value(0) + ", long: " + locationList.value(1);
        twitterApi->placesForTrends(locationList.value(0), locationList.value(1));
    }
}

void TrendsModel::handlePlacesForTrendsSuccessful(const QVariantList &result)
{
    qDebug() << "TrendsModel::handlePlacesForTrendsSuccessful";
    if (!result.isEmpty()) {
        QVariantMap firstResult = result.value(0).toMap();
        QString woeid = firstResult.value("woeid").toString();
        if (firstResult.value("countryCode") != this->country) {
            qDebug() << "Closest trends location is in another country, try to choose own country from list first...";
            QListIterator<QVariant> woeidIterator(woeidsCountries);
            while (woeidIterator.hasNext()) {
                QVariantMap woeidCountry = woeidIterator.next().toMap();
                if (woeidCountry.value("countryCode").toString() == this->country) {
                    qDebug() << "Choosing WOEID for " + woeidCountry.value("name").toString();
                    woeid = woeidCountry.value("woeid").toString();
                }
            }
        }
        qDebug() << "My place (WOEID): " + woeid;
        twitterApi->trends(woeid);
    }
}

void TrendsModel::handleTrendsSuccessful(const QVariantList &result)
{
    qDebug() << "TrendsModel::handleTrendsSuccessful";
    if (!result.isEmpty()) {
        beginResetModel();
        this->trends.clear();
        QVariantMap trendsMap = result.value(0).toMap();
        this->trends.append(trendsMap.value("trends").toList());
        QString place = trendsMap.value("locations").toList().value(0).toMap().value("name").toString();
        endResetModel();
        emit trendsRetrieved(place);
        qDebug() << "Trends model updated for " + place;
    }
}
