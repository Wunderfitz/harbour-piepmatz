/*
    Copyright (C) 2017-20 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef IMAGESSEARCHWORKER_H
#define IMAGESSEARCHWORKER_H

#include <QThread>
#include <QFileInfo>

class ImagesSearchWorker : public QThread
{
    Q_OBJECT
    void run() Q_DECL_OVERRIDE {
        performSearch();
    }
public:
    ImagesSearchWorker();
    QList<QFileInfo> getAvailableImages();

signals:
    void searchFinished();

private:
    void performSearch();

    QList<QFileInfo> availableImages;
    QString getSDCardDirectory();
};

#endif // IMAGESSEARCHWORKER_H
