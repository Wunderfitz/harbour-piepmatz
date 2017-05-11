#ifndef DIRECTMESSAGESMODEL_H
#define DIRECTMESSAGESMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariantList>
#include "twitterapi.h"

class DirectMessagesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    DirectMessagesModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();
    Q_INVOKABLE void setUserId(const QString &userId);

signals:
    void updateFailed(const QString &errorMessage);

private slots:
    void handleDirectMessagesListSuccessful(const QVariantMap &result);
    void handleDirectMessagesListError(const QString &errorMessage);
    void handleShowUserSuccessful(const QVariantMap &result);
    void handleShowUserError(const QString &errorMessage);

private:
    TwitterApi *twitterApi;
    QVariantList contacts;
    QList<QString> involvedUsers;
    QString userId;
};

#endif // DIRECTMESSAGESMODEL_H
