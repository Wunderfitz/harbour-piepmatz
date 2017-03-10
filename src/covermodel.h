#ifndef COVERMODEL_H
#define COVERMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class CoverModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CoverModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void setCoverTweets(const QVariantList &coverTweets);

private:
    QVariantList coverTweets;

};

#endif // COVERMODEL_H
