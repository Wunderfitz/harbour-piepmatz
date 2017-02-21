/*
  Copyright (C) 2017 Sebastian J. Wolf
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

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    QQmlContext *context = view.data()->rootContext();
    AccountModel accountModel;
    context->setContextProperty("accountModel", &accountModel);

    TwitterApi* twitterApi = accountModel.getTwitterApi();
    context->setContextProperty("twitterApi", twitterApi);

    view->setSource(SailfishApp::pathTo("qml/harbour-piepmatz.qml"));
    view->show();
    return app->exec();
}
