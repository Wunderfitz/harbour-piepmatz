/*
    Copyright (C) 2017-19 Sebastian J. Wolf

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

#include "contentextractor.h"

const char FLAG_STRIP_UNLIKELYS = 0x1;
const char FLAG_WEIGHT_CLASSES =  0x2;
const char FLAG_CLEAN_CONDITIONALLY = 0x4;

// https://developer.mozilla.org/en-US/docs/Web/API/Node/nodeType
const int ELEMENT_NODE = 1;
const int TEXT_NODE = 3;

// Max number of nodes supported by this parser. Default: 0 (no limit)
const int DEFAULT_MAX_ELEMS_TO_PARSE = 0;

// The number of top candidates to consider when analysing how
// tight the competition is among candidates.
const int DEFAULT_N_TOP_CANDIDATES = 5;

// Element tags to score by default.
const QStringList DEFAULT_TAGS_TO_SCORE = QStringList() << "SECTION" << "H2" << "H3" << "H4" << "H5" << "H6" << "P" << "TD" << "PRE";

// The default number of chars an article must have in order to return a result
const int DEFAULT_CHAR_THRESHOLD = 500;

ContentExtractor::ContentExtractor(QObject *parent, QGumboNode *rootNode) : QObject(parent)
{
    this->rootNode = rootNode;
}
