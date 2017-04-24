#ifndef IMAGESMODEL_H
#define IMAGESMODEL_H

#include <QAbstractListModel>
#include <QVariantList>

class ImagesModel: public QAbstractListModel
{
    Q_OBJECT
public:
    ImagesModel();

    virtual int rowCount(const QModelIndex&) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void update();

private:
    QVariantList images;
    bool searchInProgress;
};

#endif // IMAGESMODEL_H
