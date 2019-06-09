#ifndef QGUMBONODE_H
#define QGUMBONODE_H

#include <vector>
#include <functional>
#include "gumbo-parser/src/gumbo.h"
#include "HtmlTag.h"
#include <QVariantList>

class QString;
class QGumboNode;
class QGumboAttribute;
class QGumboDocument;
class QStringList;

typedef std::vector<QGumboNode> 		QGumboNodes;
typedef std::vector<QGumboAttribute> 	QGumboAttributes;

class QGumboNode
{
public:
    QGumboNode(const QGumboNode&) = default;
    QGumboNode(QGumboNode&&) noexcept = default;
    QGumboNode& operator=(const QGumboNode&) = default;

    HtmlTag tag() const;
    QString tagName() const;
    QString nodeName() const;

    QString id() const;
    QStringList classList() const;

    QGumboNodes getElementById(const QString&) const;
    QGumboNodes getElementsByTagName(HtmlTag) const;
    QGumboNodes getElementsByClassName(const QString&) const;
    QGumboNodes getAllElementsForExtractor() const;
    QGumboNodes childNodes() const;
    QGumboNodes children() const;
    QGumboNodes ancestors(const int &maxDepth = 0) const;
    QGumboNode  parent() const;

    int childElementCount() const;

    bool isElement() const;
    bool hasAttribute(const QString&) const;
    bool hasParent() const;
    bool hasAncestorTag(const HtmlTag &htmlTag, const int &maxDepth = 3) const;
    bool isProbablyVisible() const;
    bool containsContent() const;
    QVariantMap getStyles() const;

    QString innerText(const bool &normalize = false) const;
    QString outerHtml() const;
    QString getAttribute(const QString&) const;
    QString getByLine(QString matchString) const;

    QGumboAttributes allAttributes() const;

    void forEach(std::function<void(const QGumboNode&)>) const;

    explicit operator bool() const;

private:
    QGumboNode();
    QGumboNode(GumboNode* node);

    friend class QGumboDocument;
private:
    GumboNode* ptr_;
};

#endif // QGUMBONODE_H
