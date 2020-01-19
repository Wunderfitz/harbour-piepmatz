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

#ifndef CONTENTEXTRACTOR_H
#define CONTENTEXTRACTOR_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QVariantMap>

#include "QGumboParser/HtmlTag.h"
#include "QGumboParser/qgumbonode.h"

class ContentExtractor : public QObject
{
    Q_OBJECT
public:
    explicit ContentExtractor(QObject *parent = nullptr, QGumboNode *rootNode = nullptr);

    QVariantMap parse();

signals:

public slots:

private:
    QGumboNode *rootNode;
    char _flags;
    int _maxElemsToParse;
    int _nbTopCandidates;
    int _charThreshold;
    QStringList _classesToPreserve;

    QVariantMap getArticleMetadata();
    QString getArticleTitle();
    QString getArticleContent();
    int getInitialContentScore(const QGumboNode &node);
    int getClassWeight(const QGumboNode &node);
    float getLinkDensity(const QGumboNode &node);

};

#endif // CONTENTEXTRACTOR_H
