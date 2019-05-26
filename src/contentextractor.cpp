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

#include <QRegularExpression>
#include <QDebug>

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

const QRegularExpression REGEXP_UNLIKELY_CANDIDATES = QRegularExpression("/-ad-|ai2html|banner|breadcrumbs|combx|comment|community|cover-wrap|disqus|extra|foot|gdpr|header|legends|menu|related|remark|replies|rss|shoutbox|sidebar|skyscraper|social|sponsor|supplemental|ad-break|agegate|pagination|pager|popup|yom-remote/i");
const QRegularExpression REGEXP_OK_MAYBE_ITS_A_CANDIDATE = QRegularExpression("/and|article|body|column|main|shadow/i");
const QRegularExpression REGEXP_POSITIVE = QRegularExpression("/article|body|content|entry|hentry|h-entry|main|page|pagination|post|text|blog|story/i");
const QRegularExpression REGEXP_NEGATIVE = QRegularExpression("/hidden|^hid$| hid$| hid |^hid |banner|combx|comment|com-|contact|foot|footer|footnote|gdpr|masthead|media|meta|outbrain|promo|related|scroll|share|shoutbox|sidebar|skyscraper|sponsor|shopping|tags|tool|widget/i");
const QRegularExpression REGEXP_EXTRANEOUS = QRegularExpression("/print|archive|comment|discuss|e[\\-]?mail|share|reply|all|login|sign|single|utility/i");
const QRegularExpression REGEXP_BYLINE = QRegularExpression("/byline|author|dateline|writtenby|p-author/i");
const QRegularExpression REGEXP_REPLACE_FONTS = QRegularExpression("/<(\\/?)font[^>]*>/gi");
const QRegularExpression REGEXP_NORMALIZE = QRegularExpression("/\\s{2,}/g");
const QRegularExpression REGEXP_VIDEOS = QRegularExpression("/\\/\\/(www\\.)?((dailymotion|youtube|youtube-nocookie|player\\.vimeo|v\\.qq)\\.com|(archive|upload\\.wikimedia)\\.org|player\\.twitch\\.tv)/i");
const QRegularExpression REGEXP_NEXT_LINK = QRegularExpression("/(next|weiter|continue|>([^\\|]|$)|»([^\\|]|$))/i");
const QRegularExpression REGEXP_PREV_LINK = QRegularExpression("/(prev|earl|old|new|<|«)/i");
const QRegularExpression REGEXP_WHITESPACE = QRegularExpression("/^\\s*$/");
const QRegularExpression REGEXP_HAS_CONTENT = QRegularExpression("/\\S$/");

const QStringList DIV_TO_P_ELEMS = QStringList() << "A" << "BLOCKQUOTE" << "DL" << "DIV" << "IMG" << "OL" << "P" << "PRE" << "TABLE" << "UL" << "SELECT";
const QStringList ALTER_TO_DIV_EXCEPTIONS = QStringList() << "DIV" << "ARTICLE" << "SECTION" << "P";
const QStringList PRESENTATIONAL_ATTRIBUTES = QStringList() << "align" << "background" << "bgcolor" << "border" << "cellpadding" << "cellspacing" << "frame" << "hspace" << "rules" << "style" << "valign" << "vspace";
const QStringList DEPRECATED_SIZE_ATTRIBUTE_ELEMS = QStringList() << "TABLE" << "TH" << "TD" << "HR" << "PRE";
const QStringList PHRASING_ELEMS = QStringList() << "ABBR" << "AUDIO" << "B" << "BDO" << "BR" << "BUTTON" << "CITE" << "CODE" << "DATA" <<
                                                    "DATALIST" << "DFN" << "EM" << "EMBED" << "I" << "IMG" << "INPUT" << "KBD" << "LABEL" <<
                                                    "MARK" << "MATH" << "METER" << "NOSCRIPT" << "OBJECT" << "OUTPUT" << "PROGRESS" << "Q" <<
                                                    "RUBY" << "SAMP" << "SCRIPT" << "SELECT" << "SMALL" << "SPAN" << "STRONG" << "SUB" <<
                                                    "SUP" << "TEXTAREA" << "TIME" << "VAR" << "WBR";
const QStringList CLASSES_TO_PRESERVE = QStringList() << "page";

ContentExtractor::ContentExtractor(QObject *parent, QGumboNode *rootNode) : QObject(parent)
{
    this->rootNode = rootNode;

    this->_maxElemsToParse = DEFAULT_MAX_ELEMS_TO_PARSE;
    this->_nbTopCandidates = DEFAULT_N_TOP_CANDIDATES;
    this->_charThreshold = DEFAULT_CHAR_THRESHOLD;
    this->_classesToPreserve = CLASSES_TO_PRESERVE;

    this->_flags = FLAG_STRIP_UNLIKELYS |
                   FLAG_WEIGHT_CLASSES |
                   FLAG_CLEAN_CONDITIONALLY;
}

QVariantMap ContentExtractor::parse()
{
    qDebug() << "ContentExtractor::parse";
    QVariantMap contentMap;

    // Avoid parsing too large documents, as per configuration option
    if (this->_maxElemsToParse > 0) {
      int numTags = this->rootNode->childElementCount();
      if (numTags > this->_maxElemsToParse) {
        qDebug() << "Aborting parsing document; " << numTags << " elements found";
        return contentMap;
      }
    }

    QVariantMap metadata = this->getArticleMetadata();

    return contentMap;
}

QVariantMap ContentExtractor::getArticleMetadata()
{
    qDebug() << "ContentExtractor::getArticleMetadata";
    QVariantMap articleMetadata;
    QVariantMap values;
    QGumboNodes metaElements = this->rootNode->getElementsByTagName(HtmlTag::META);

    // property is a space-separated list of values
    QRegularExpression propertyPattern("\\s*(dc|dcterm|og|twitter)\\s*:\\s*(author|creator|description|title|site_name)\\s*", QRegularExpression::CaseInsensitiveOption);

    // name is a single value
    QRegularExpression namePattern("^\\s*(?:(dc|dcterm|og|twitter|weibo:(article|webpage))\\s*[\\.:]\\s*)?(author|creator|description|title|site_name)\\s*$", QRegularExpression::CaseInsensitiveOption);

    for (QGumboNode &element : metaElements) {
        QString elementName = element.getAttribute("name");
        QString elementProperty = element.getAttribute("property");
        QString content = element.getAttribute("content");
        if (content.isEmpty()) {
            continue;
        }
        bool propertyMatched = false;
        if (!elementProperty.isEmpty()) {
            QRegularExpressionMatchIterator propertyMatchIterator = propertyPattern.globalMatch(elementProperty);
            while (propertyMatchIterator.hasNext()) {
                propertyMatched = true;
                QRegularExpressionMatch propertyMatch = propertyMatchIterator.next();
                values.insert(propertyMatch.captured(0).toLower().replace(QRegularExpression("\\s"), ""), content.trimmed());
            }
        }
        if (!propertyMatched && !elementName.isEmpty()) {
            qDebug() << elementName;
            QRegularExpressionMatch nameMatch = namePattern.match(elementName);
            if (nameMatch.hasMatch()) {
                values.insert(elementName.toLower().replace(QRegularExpression("\\s"), "").replace(QRegularExpression("\\."), ":"), content.trimmed());
            }
        }

        if (values.contains("dc:title")) { articleMetadata.insert("title", values.value("dc:title")); }
        if (values.contains("dcterm:title")) { articleMetadata.insert("title", values.value("dcterm:title")); }
        if (values.contains("og:title")) { articleMetadata.insert("title", values.value("og:title")); }
        if (values.contains("weibo:article:title")) { articleMetadata.insert("title", values.value("weibo:article:title")); }
        if (values.contains("weibo:webpage:title")) { articleMetadata.insert("title", values.value("weibo:webpage:title")); }
        if (values.contains("title")) { articleMetadata.insert("title", values.value("title")); }
        if (values.contains("twitter:title")) { articleMetadata.insert("title", values.value("twitter:title")); }

        if (values.contains("dc:creator")) { articleMetadata.insert("byline", values.value("dc:creator")); }
        if (values.contains("dcterm:creator")) { articleMetadata.insert("byline", values.value("dcterm:creator")); }
        if (values.contains("author")) { articleMetadata.insert("byline", values.value("author")); }

        if (values.contains("dc:description")) { articleMetadata.insert("excerpt", values.value("dc:description")); }
        if (values.contains("dcterm:description")) { articleMetadata.insert("excerpt", values.value("dcterm:description")); }
        if (values.contains("og:description")) { articleMetadata.insert("excerpt", values.value("og:description")); }
        if (values.contains("weibo:article:description")) { articleMetadata.insert("excerpt", values.value("weibo:article:description")); }
        if (values.contains("weibo:webpage:description")) { articleMetadata.insert("excerpt", values.value("weibo:webpage:description")); }
        if (values.contains("description")) { articleMetadata.insert("excerpt", values.value("description")); }
        if (values.contains("twitter:description")) { articleMetadata.insert("excerpt", values.value("twitter:description")); }

        if (values.contains("og:site_name")) { articleMetadata.insert("siteName", values.value("og:site_name")); }

    }

    qDebug() << "[ContentExtractor] Article Metadata: " << articleMetadata;

    return articleMetadata;
}
