#ifndef SAVEDSEARCHESMODEL_H
#define SAVEDSEARCHESMODEL_H


#include <QAbstractListModel>
#include <QVariantList>
#include "twitterapi.h"

class SavedSearchesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    SavedSearchesModel(TwitterApi *twitterApi);

    virtual int rowCount(const QModelIndex &) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();
    Q_INVOKABLE void saveSearch(const QString &query);
    Q_INVOKABLE void removeSavedSearch(const QString &id);

signals:
    void updateFinished();
    void updateError(const QString &errorMessage);
    void saveSuccessful(const QString &query);
    void saveError(const QString &errorMessage);
    void removeError(const QString &errorMessage);

public slots:
    void handleSavedSearchesSuccessful(const QVariantList &result);
    void handleSavedSearchesError(const QString &errorMessage);
    void handleSaveSearchSuccessful(const QVariantMap &result);
    void handleSaveSearchError(const QString &errorMessage);
    void handleDestroySavedSearchSuccessful(const QVariantMap &result);
    void handleDestroySavedSearchError(const QString &errorMessage);

private:
    QVariantList savedSearches;
    TwitterApi *twitterApi;
    bool updateInProgress;
};

#endif // SAVEDSEARCHESMODEL_H
