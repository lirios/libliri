// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTXDG_XDGMENULAYOUTPROCESSOR_H
#define QTXDG_XDGMENULAYOUTPROCESSOR_H

#include <QtXml/QDomElement>
#include <QList>

namespace Liri {

struct LayoutItem {
    enum Type {
        Filename,
        Menuname,
        Separator,
        MergeMenus,
        MergeFiles,
        MergeAll,
    };

    Type type;
    bool showEmpty;
    bool isInline;
    bool inlineLimit;
    bool inlineHeader;
    bool inlineAlias;
    QString fileId;
};

//class Layout: public QList<LayoutItem>
//{
//public:
/*    Layout() {}


    bool showEmpty() { return mShowEmpty; }
    void setShowEmpty(bool value) { mShowEmpty = value; }

    bool isInline() { return mInline; }
    void setInline(bool value) { mInline = value; }

    int inlineLimit() { return mInlineLimit; }
    void setInlineLimit(int value) { mInlineLimit = value; }

    bool inlineHeader() {return mInlineHeader; }
    void setInlineHeader(bool value) { mInlineHeader = value; }

    bool inlineAlias() { return mInlineAlias; }
    void setInlineAlias(bool value) { mInlineAlias = value; }



private:
*/

struct LayoutParams {
    bool mShowEmpty;
    bool mInline;
    int mInlineLimit;
    bool mInlineHeader;
    bool mInlineAlias;
};

class XdgMenuLayoutProcessor
{
public:
    XdgMenuLayoutProcessor(QDomElement &element);
    void run();

protected:
    XdgMenuLayoutProcessor(QDomElement &element, XdgMenuLayoutProcessor *parent);

private:
    void setParams(QDomElement defaultLayout, LayoutParams *result);
    QDomElement searchElement(const QString &tagName, const QString &attributeName, const QString &attributeValue) const;
    void processFilenameTag(const QDomElement &element);
    void processMenunameTag(const QDomElement &element);
    void processSeparatorTag(const QDomElement &element);
    void processMergeTag(const QDomElement &element);

    LayoutParams mDefaultParams;
    QDomElement &mElement;
    QDomElement mDefaultLayout;
    QDomElement mLayout;
    QDomElement mResult;
};

} // namespace Liri

#endif // QTXDG_XDGMENULAYOUTPROCESSOR_H
