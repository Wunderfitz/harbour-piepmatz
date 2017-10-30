#ifndef WAGNIS_H
#define WAGNIS_H

#include <QObject>

class Wagnis : public QObject
{
    Q_OBJECT
public:
    explicit Wagnis(QObject *parent = 0);
    ~Wagnis();

signals:

public slots:
};

#endif // WAGNIS_H
