/*
    Copyright (C) 2017 Sebastian J. Wolf

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

#include "o1.h"
#include "o1twitter.h"
#include "o1requestor.h"
#include "accountmodel.h"
#include "twitterapi.h"
#include "timelinemodel.h"
#include "searchmodel.h"
#include "searchusersmodel.h"
#include "mentionsmodel.h"
#include "imagesmodel.h"
#include "directmessagesmodel.h"
#include "trendsmodel.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    AccountModel accountModel;
    context->setContextProperty("accountModel", &accountModel);

    TwitterApi *twitterApi = accountModel.getTwitterApi();
    context->setContextProperty("twitterApi", twitterApi);

    TimelineModel timelineModel(twitterApi);
    context->setContextProperty("timelineModel", &timelineModel);
    context->setContextProperty("coverModel", timelineModel.coverModel);

    SearchModel searchModel(twitterApi);
    context->setContextProperty("searchModel", &searchModel);

    SearchUsersModel searchUsersModel(twitterApi);
    context->setContextProperty("searchUsersModel", &searchUsersModel);

    MentionsModel mentionsModel(twitterApi);
    context->setContextProperty("mentionsModel", &mentionsModel);

    ImagesModel imagesModel(twitterApi);
    context->setContextProperty("imagesModel", &imagesModel);

    DirectMessagesModel directMessagesModel(twitterApi);
    context->setContextProperty("directMessagesModel", &directMessagesModel);

    TrendsModel trendsModel(twitterApi);
    context->setContextProperty("trendsModel", &trendsModel);

    view->setSource(SailfishApp::pathTo("qml/harbour-piepmatz.qml"));
    view->show();
    return app->exec();
}
