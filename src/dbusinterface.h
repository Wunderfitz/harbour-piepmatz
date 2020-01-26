#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QObject>
#include <QtDBus>

#include "dbusadaptor.h"

const QString INTERFACE_NAME = "de.ygriega.piepmatz";

class DBusInterface : public QObject
{
    Q_OBJECT
public:
    explicit DBusInterface(QObject *parent = nullptr);
    DBusAdaptor *getDBusAdaptor();

signals:

public slots:

private:
    DBusAdaptor *dbusAdaptor;

};

#endif // DBUSINTERFACE_H
