#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent) : QObject(parent)
{
    qDebug() << "Initializing D-BUS connectivity";
    this->dbusAdaptor = new DBusAdaptor(this);
    QDBusConnection sessionBusConnection = QDBusConnection::sessionBus();

    if (!sessionBusConnection.isConnected()) {
        qDebug() << "Error connecting to D-BUS";
        return;
    }

    if (!sessionBusConnection.registerObject("/", this)) {
        qDebug() << "Error registering root object to D-BUS" << sessionBusConnection.lastError().message();
        return;
    }

    if (!sessionBusConnection.registerService(INTERFACE_NAME)) {
        qDebug() << "Error registering interface to D-BUS" << sessionBusConnection.lastError().message();
        return;
    }

}

DBusAdaptor *DBusInterface::getDBusAdaptor()
{
    return this->dbusAdaptor;
}
