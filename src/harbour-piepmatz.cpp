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
#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QScopedPointer>
#include <QQuickView>
#include <QtQml>
#include <QQmlContext>
#include <QGuiApplication>
#include <QDebug>

#include "o1.h"
#include "o1twitter.h"
#include "o1requestor.h"
#include "accountmodel.h"
#include "twitterapi.h"
#include "locationinformation.h"
#include "timelinemodel.h"
#include "searchmodel.h"
#include "searchusersmodel.h"
#include "mentionsmodel.h"
#include "imagesmodel.h"
#include "directmessagesmodel.h"
#include "trendsmodel.h"
#include "ownlistsmodel.h"
#include "membershiplistsmodel.h"
#include "savedsearchesmodel.h"
#include "dbusadaptor.h"
//#include "wagnis/wagnis.h"

void migrateSettings() {
    const QStringList sailfishOSVersion = QSysInfo::productVersion().split(".");
    int sailfishOSMajorVersion = sailfishOSVersion.value(0).toInt();
    int sailfishOSMinorVersion = sailfishOSVersion.value(1).toInt();
    if ((sailfishOSMajorVersion == 4 && sailfishOSMinorVersion >= 4) || sailfishOSMajorVersion > 4) {
        qDebug() << "Checking if we need to migrate settings...";
        QSettings settings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/de.ygriega/piepmatz/settings.conf", QSettings::NativeFormat);
        if (settings.contains("migrated")) {
            return;
        }
        QSettings oldSettings(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz/settings.conf", QSettings::NativeFormat);
        const QStringList oldKeys = oldSettings.allKeys();
        if (oldKeys.isEmpty()) {
            return;
        }
        qDebug() << "SailfishOS >= 4.4 and old configuration file detected, migrating settings to new location...";
        for (const QString &key : oldKeys) {
            settings.setValue(key, oldSettings.value(key));
        }

        QDir oldConfigLocation(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-piepmatz");
        qDebug() << "Old config directory: " + oldConfigLocation.path();
        if (oldConfigLocation.exists()) {
            qDebug() << "Old configuration files detected, migrating files to new location...";
            const int oldConfigPathLength = oldConfigLocation.absolutePath().length();
            QString configLocationPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/de.ygriega/piepmatz";
            QDir configLocation(configLocationPath);
            if (!configLocation.exists()) {
                configLocation.mkpath(configLocationPath);
            }
            QDirIterator oldConfigIterator(oldConfigLocation, QDirIterator::Subdirectories);
            while (oldConfigIterator.hasNext()) {
                oldConfigIterator.next();
                QFileInfo currentFileInfo = oldConfigIterator.fileInfo();
                if (!currentFileInfo.isHidden() && currentFileInfo.fileName() != "settings.conf") {
                    const QString subPath = currentFileInfo.absoluteFilePath().mid(oldConfigPathLength);
                    const QString targetPath = configLocationPath + subPath;
                    if (currentFileInfo.isDir()) {
                        qDebug() << "Creating new directory " + targetPath;
                        configLocation.mkpath(targetPath);
                    } else if(currentFileInfo.isFile()) {
                        qDebug() << "Copying file to " + targetPath;
                        QFile::copy(currentFileInfo.absoluteFilePath(), targetPath);
                    }
                }
            }
        }

        QDir oldDataLocation(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/harbour-piepmatz");
        qDebug() << "Old data directory: " + oldDataLocation.path();
        if (oldDataLocation.exists()) {
            qDebug() << "Old data files detected, migrating files to new location...";
            const int oldDataPathLength = oldDataLocation.absolutePath().length();
            QString dataLocationPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            QDir dataLocation(dataLocationPath);
            if (!dataLocation.exists()) {
                dataLocation.mkpath(dataLocationPath);
            }
            QDirIterator oldDataIterator(oldDataLocation, QDirIterator::Subdirectories);
            while (oldDataIterator.hasNext()) {
                oldDataIterator.next();
                QFileInfo currentFileInfo = oldDataIterator.fileInfo();
                if (!currentFileInfo.isHidden()) {
                    const QString subPath = currentFileInfo.absoluteFilePath().mid(oldDataPathLength);
                    const QString targetPath = dataLocationPath + subPath;
                    if (currentFileInfo.isDir()) {
                        qDebug() << "Creating new directory " + targetPath;
                        dataLocation.mkpath(targetPath);
                    } else if(currentFileInfo.isFile()) {
                        qDebug() << "Copying file to " + targetPath;
                        QFile::copy(currentFileInfo.absoluteFilePath(), targetPath);
                    }
                }
            }
        }

        settings.setValue("migrated", true);
    }
}

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();

    migrateSettings();

    AccountModel accountModel;
    context->setContextProperty("accountModel", &accountModel);

    TwitterApi *twitterApi = accountModel.getTwitterApi();
    context->setContextProperty("twitterApi", twitterApi);

    LocationInformation *locationInformation = accountModel.getLocationInformation();
    context->setContextProperty("locationInformation", locationInformation);

    DBusAdaptor *dBusAdaptor = accountModel.getDBusAdaptor();
    context->setContextProperty("dBusAdaptor", dBusAdaptor);

//    Wagnis *wagnis = accountModel.getWagnis();
//    context->setContextProperty("wagnis", wagnis);

    TimelineModel timelineModel(twitterApi);
    context->setContextProperty("timelineModel", &timelineModel);
    context->setContextProperty("coverModel", timelineModel.coverModel);

    SearchModel searchModel(twitterApi);
    context->setContextProperty("searchModel", &searchModel);

    SearchUsersModel searchUsersModel(twitterApi);
    context->setContextProperty("searchUsersModel", &searchUsersModel);

    MentionsModel mentionsModel(twitterApi, accountModel);
    context->setContextProperty("mentionsModel", &mentionsModel);

    ImagesModel imagesModel(twitterApi);
    context->setContextProperty("imagesModel", &imagesModel);

    DirectMessagesModel directMessagesModel(twitterApi);
    context->setContextProperty("directMessagesModel", &directMessagesModel);

    TrendsModel trendsModel(twitterApi);
    context->setContextProperty("trendsModel", &trendsModel);

    OwnListsModel ownListsModel(twitterApi);
    context->setContextProperty("ownListsModel", &ownListsModel);

    MembershipListsModel membershipListsModel(twitterApi);
    context->setContextProperty("membershipListsModel", &membershipListsModel);

    SavedSearchesModel savedSearchesModel(twitterApi);
    context->setContextProperty("savedSearchesModel", &savedSearchesModel);

    view->setSource(SailfishApp::pathTo("qml/harbour-piepmatz.qml"));
    view->show();
    return app->exec();
}
