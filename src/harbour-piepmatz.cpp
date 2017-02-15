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

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    qmlRegisterType<O1Twitter>("com.pipacs.o2", 1, 0, "O1Twitter");
    QQmlContext *ctxt = view.data()->rootContext();
    AccountModel accountModel;
    ctxt->setContextProperty("accountModel", &accountModel);

    view->setSource(SailfishApp::pathTo("qml/harbour-piepmatz.qml"));
    view->show();
    return app->exec();
}
