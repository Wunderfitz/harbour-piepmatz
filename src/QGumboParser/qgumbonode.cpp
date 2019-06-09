#include <cstring>
#include <sstream>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QRegularExpression>
#include "qgumbonode.h"
#include "qgumboattribute.h"

namespace {

const char* const ID_ATTRIBUTE 		= u8"id";
const char* const CLASS_ATTRIBUTE 	= u8"class";
const char* const STYLE_ATTRIBUTE 	= u8"style";

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

// Element tags to score by default.
const QList<HtmlTag> DEFAULT_TAGS_TO_SCORE = QList<HtmlTag>() << HtmlTag::SECTION << HtmlTag::H2 << HtmlTag::H3 << HtmlTag::H4 << HtmlTag::H5 << HtmlTag::H6 << HtmlTag::P << HtmlTag::TD << HtmlTag::PRE;

template<typename TFunctor>
bool iterateTree(GumboNode* node, TFunctor& functor)
{
    if (!node || node->type != GUMBO_NODE_ELEMENT)
        return false;

    if (functor(node))
        return true;

    for (uint i = 0; i < node->v.element.children.length; ++i) {
        if (iterateTree(static_cast<GumboNode*>(node->v.element.children.data[i]), functor))
            return true;
    }

    return false;
}

template<typename TFunctor>
bool iterateChildren(GumboNode* node, TFunctor& functor)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return false;

    GumboVector& vec = node->v.element.children;

    for (uint i = 0, e = vec.length; i < e; ++i) {
        GumboNode* node = static_cast<GumboNode*>(vec.data[i]);
        if (functor(node))
            return true;
    }

    return false;
}

} /* namespace */

QGumboNode::QGumboNode()
{
}

QGumboNode::QGumboNode(GumboNode* node) :
    ptr_(node)
{
    if (!ptr_)
        throw std::runtime_error("can't create Node from nullptr");
}

bool QGumboNode::isProbablyVisible() const
{
    if (!this->getAttribute("hidden").isEmpty()) {
        return false;
    }

    QVariantMap myStyles = this->getStyles();
    if (!myStyles.isEmpty()) {
        if (myStyles.value("display") == "none") {
            return false;
        }
    }

    return true;
}

bool QGumboNode::containsContent() const
{
    uint childrenSize = this->children().size();
    return !(this->isElement() &&
             this->innerText().trimmed().size() == 0 &&
            (childrenSize = 0 ||
             childrenSize == this->getElementsByTagName(HtmlTag::BR).size() + this->getElementsByTagName(HtmlTag::HR).size()));
}

QVariantMap QGumboNode::getStyles() const
{
    QVariantMap myStyles;
    QString rawStyles = getAttribute(STYLE_ATTRIBUTE);
    QStringList rawStyleList = rawStyles.split(';');
    QListIterator<QString> rawStyleListIterator(rawStyleList);
    while (rawStyleListIterator.hasNext()) {
        QString nextStyle = rawStyleListIterator.next();
        QStringList singleStyle = nextStyle.split(':');
        if (singleStyle.size() == 2) {
            myStyles.insert(singleStyle.at(0).trimmed(), singleStyle.at(1).trimmed());
        }
    }
    return myStyles;
}

QGumboNodes QGumboNode::getElementById(const QString& nodeId) const
{
    Q_ASSERT(ptr_);

    if (nodeId.isEmpty())
        throw std::invalid_argument("id can't be empty string");

    QGumboNodes nodes;

    auto functor = [&nodes, &nodeId] (GumboNode* node) {
        GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, ID_ATTRIBUTE);
        if (attr) {
            const QString value = QString::fromUtf8(attr->value);
            if (value.compare(nodeId, Qt::CaseInsensitive) == 0) {
                nodes.emplace_back(QGumboNode(node));
                return true;
            }
        }
        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::getElementsByTagName(HtmlTag tag) const
{
    Q_ASSERT(ptr_);

    GumboTag tag_ = static_cast<GumboTag>(tag);
    QGumboNodes nodes;

    auto functor = [&nodes, tag_](GumboNode* node) {
        if (node->v.element.tag == tag_) {
            nodes.emplace_back(QGumboNode(node));
        }
        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::getElementsByClassName(const QString& name) const
{
    Q_ASSERT(ptr_);

    if (name.isEmpty())
        throw std::invalid_argument("class name can't be empty string");

    QGumboNodes nodes;

    auto functor = [&nodes, &name] (GumboNode* node) {
        GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, CLASS_ATTRIBUTE);
        if (attr) {
            const QString value = QString::fromUtf8(attr->value);
            const QVector<QStringRef> parts =
                    value.splitRef(QChar(' '), QString::SkipEmptyParts, Qt::CaseInsensitive);

            for (const QStringRef& part: parts) {
                if (part.compare(name, Qt::CaseInsensitive) == 0) {
                    nodes.emplace_back(QGumboNode(node));
                    break;
                }
            }
        }
        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::getAllElementsForExtractor() const
{
    Q_ASSERT(ptr_);

    QGumboNodes nodes;

    auto functor = [&nodes](GumboNode* node) {
        QGumboNode myNode(node);

        qDebug() << "Current Tag: " << node->v.element.tag;
        QString nodeIdentifier = myNode.getAttribute(CLASS_ATTRIBUTE) + " " + myNode.id();
        qDebug() << "Current Node Identifier: " << nodeIdentifier;


        // Remove all invisible nodes...
        if (!myNode.isProbablyVisible()) {
            return false;
        }

        if (!myNode.getByLine(nodeIdentifier).isEmpty()) {
            return false;
        }

        if (REGEXP_UNLIKELY_CANDIDATES.match(nodeIdentifier).hasMatch() &&
                !REGEXP_OK_MAYBE_ITS_A_CANDIDATE.match(nodeIdentifier).hasMatch() &&
                !myNode.hasAncestorTag(HtmlTag::TABLE) &&
                myNode.tag() != HtmlTag::BODY &&
                myNode.tag() != HtmlTag::A) {
            return false;
        }

        // Remove DIV, SECTION and HEADER nodes without content
        if ((myNode.tag() == HtmlTag::DIV || myNode.tag() == HtmlTag::SECTION || myNode.tag() == HtmlTag::HEADER ||
                myNode.tag() == HtmlTag::H1 || myNode.tag() == HtmlTag::H2 || myNode.tag() == HtmlTag::H3 ||
                myNode.tag() == HtmlTag::H4 || myNode.tag() == HtmlTag::H5 || myNode.tag() == HtmlTag::H6)
                && !myNode.containsContent()) {
            return false;
        }

        if (DEFAULT_TAGS_TO_SCORE.contains(myNode.tag())) {
            return true;
        }

        return false;
    };

    iterateTree(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::childNodes() const
{
    Q_ASSERT(ptr_);

    QGumboNodes nodes;

    auto functor = [&nodes] (GumboNode* node) {
        nodes.emplace_back(QGumboNode(node));
        return false;
    };

    iterateChildren(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::children() const
{
    Q_ASSERT(ptr_);

    QGumboNodes nodes;

    auto functor = [&nodes] (GumboNode* node) {
        if (node->type == GUMBO_NODE_ELEMENT) {
            nodes.emplace_back(QGumboNode(node));
        }
        return false;
    };

    iterateChildren(ptr_, functor);

    return nodes;
}

QGumboNodes QGumboNode::ancestors(const int &maxDepth) const
{
    Q_ASSERT(ptr_);

    QGumboNodes nodes;

    if (!this->hasParent()) {
        return nodes;
    }
    GumboNode *currentNode = ptr_;
    int currentDepth = 0;
    while (currentNode->parent != nullptr) {
        currentDepth++;
        currentNode = currentNode->parent;
        if (currentNode->type == GUMBO_NODE_ELEMENT) {
            nodes.emplace_back(QGumboNode(currentNode));
        }
        if (currentDepth == maxDepth) {
            break;
        }
    }

    return nodes;
}

QGumboNode QGumboNode::parent() const
{
    Q_ASSERT(ptr_);

    QGumboNode myParent(ptr_->parent);

    return myParent;
}

int QGumboNode::childElementCount() const
{
    Q_ASSERT(ptr_);

    int count = 0;

    auto functor = [&count] (GumboNode* node) {
        if (node->type == GUMBO_NODE_ELEMENT)
            ++count;
        return false;
    };

    iterateChildren(ptr_, functor);

    return count;
}

QString QGumboNode::innerText(const bool &normalize) const
{
    Q_ASSERT(ptr_);

    QString text;

    auto functor = [&text] (GumboNode* node) {
        if (node->type == GUMBO_NODE_TEXT) {
            text += QString::fromUtf8(node->v.text.text);
        }
        return false;
    };

    iterateChildren(ptr_, functor);

    if (normalize) {
        text = text.trimmed().replace(REGEXP_NORMALIZE, " ");
    }

    return text;
}

QString QGumboNode::outerHtml() const
{
    Q_ASSERT(ptr_);

    QString text;
    switch (ptr_->type) {
    case GUMBO_NODE_DOCUMENT: {
        throw std::runtime_error("invalid node type");
    }
    case GUMBO_NODE_ELEMENT: {
        const auto& elem = ptr_->v.element;
        const auto& tag = elem.original_tag;
        if (tag.data && tag.length) {
            int lenght = elem.end_pos.offset - elem.start_pos.offset + elem.original_end_tag.length;
            Q_ASSERT(lenght > 0);
            text = QString::fromUtf8(tag.data, lenght);
        }
        break;
    }
    default: {
        const auto& str = ptr_->v.text.original_text;
        text = QString::fromUtf8(str.data, str.length);
    }}
    return text;
}

HtmlTag QGumboNode::tag() const
{
    if (isElement())
        return HtmlTag(ptr_->v.element.tag);

    return HtmlTag::UNKNOWN;
}

QString QGumboNode::tagName() const
{
    Q_ASSERT(ptr_);
    GumboTag tag = ptr_->v.element.tag;
    return QString::fromUtf8(gumbo_normalized_tagname(tag));
}

QString QGumboNode::nodeName() const
{
    return tagName();
}

QString QGumboNode::id() const
{
    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes, ID_ATTRIBUTE);
    if (attr)
        return QString::fromUtf8(attr->value);

    return QString();
}

QStringList QGumboNode::classList() const
{
    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes, CLASS_ATTRIBUTE);
    if (attr) {
        QString values = QString::fromUtf8(attr->value);
        return values.split(u8" ", QString::SkipEmptyParts, Qt::CaseInsensitive);
    }

    return QStringList();
}

bool QGumboNode::isElement() const
{
    return ptr_->type == GUMBO_NODE_ELEMENT;
}

bool QGumboNode::hasAttribute(const QString& name) const
{
    if (name.isEmpty())
        throw std::invalid_argument("attribute can't be empty string");

    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes,
                                               name.toUtf8().constData());
    return attr != nullptr;
}

bool QGumboNode::hasParent() const
{
    return ptr_->parent != nullptr;
}

bool QGumboNode::hasAncestorTag(const HtmlTag &htmlTag, const int &maxDepth) const
{
    int depth = 0;
    QGumboNode currentNode = *this;
    while (currentNode.hasParent()) {
      if (maxDepth > 0 && depth > maxDepth) {
          return false;
      }
      if (currentNode.parent().tag() == htmlTag) {
          return true;
      }
      currentNode = currentNode.parent();
      depth++;
    }
    return false;
}

QString QGumboNode::getAttribute(const QString& attrName) const
{
    if (attrName.isEmpty())
        throw std::invalid_argument("attribute name can't be empty string");

    GumboAttribute* attr = gumbo_get_attribute(&ptr_->v.element.attributes,
                                               attrName.toUtf8().constData());
    if (attr)
        return QString::fromUtf8(attr->value);

    return QString();
}

QString QGumboNode::getByLine(QString matchString) const
{
    QString byLine;

    QString attributeRel = this->getAttribute("rel");
    QString attributeItemprop = this->getAttribute("itemprop");

    QString textContent = this->innerText();
    if (textContent.isEmpty() || textContent.trimmed().size() > 100) {
        return byLine;
    }

    if (attributeRel == "author" || (!attributeItemprop.isEmpty() && attributeItemprop.contains("author")) || REGEXP_BYLINE.match(matchString).hasMatch()) {
      byLine = textContent.trimmed();
    }

    return byLine;
}

QGumboAttributes QGumboNode::allAttributes() const
{
    Q_ASSERT(ptr_);

    QGumboAttributes attrs;

    for (uint i = 0, len = ptr_->v.element.attributes.length; i < len; ++i) {
        GumboAttribute* attr =
                static_cast<GumboAttribute*>(ptr_->v.element.attributes.data[i]);
        attrs.emplace_back(QGumboAttribute(attr->name, attr->value));
    }
    return attrs;
}


void QGumboNode::forEach(std::function<void(const QGumboNode&)> func) const
{
    Q_ASSERT(ptr_);

    auto functor = [&func](GumboNode* node) {
        func(QGumboNode(node));
        return false;
    };

    iterateTree(ptr_, functor);
}

QGumboNode::operator bool() const
{
    return ptr_;
}
