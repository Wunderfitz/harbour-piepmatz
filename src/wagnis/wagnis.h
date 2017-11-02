#ifndef WAGNIS_H
#define WAGNIS_H

#include <QObject>

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QObject *parent = 0);
    ~Wagnis();
    Q_INVOKABLE QString getId();

signals:

public slots:

private:
    void generateId();
    QString wagnisId;
};

#endif // WAGNIS_H
