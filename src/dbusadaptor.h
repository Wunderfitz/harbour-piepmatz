#ifndef DBUSADAPTOR_H
#define DBUSADAPTOR_H

#include <QDBusAbstractAdaptor>

class DBusAdaptor : public QDBusAbstractAdaptor
{

    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "de.ygriega.piepmatz")

public:
    DBusAdaptor(QObject *parent);

signals:
    void pleaseOpenUrl(const QString &url);

public slots:
    void openUrl(const QStringList &arguments);

};

#endif // DBUSADAPTOR_H
