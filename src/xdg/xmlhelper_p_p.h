// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTXDG_XMLHELPER_H
#define QTXDG_XMLHELPER_H

#include <QDebug>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

class DomElementIterator
{
public:
    explicit DomElementIterator(const QDomNode &parentNode, const QString &tagName = QString())
        : mTagName(tagName)
        , mParent(parentNode)
    {
        toFront();
    }

    void toFront()
    {
        mNext = mParent.firstChildElement(mTagName);
    }

    bool hasNext()
    {
        return (!mNext.isNull());
    }

    const QDomElement &next()
    {
        mCur = mNext;
        mNext = mNext.nextSiblingElement(mTagName);
        return mCur;
    }

    void toBack()
    {
        mNext = mParent.lastChildElement(mTagName);
    }

    bool hasPrevious()
    {
        return (!mNext.isNull());
    }

    const QDomElement &previous()
    {
        mCur = mNext;
        mNext = mNext.previousSiblingElement(mTagName);
        return mCur;
    }

    const QDomElement &current() const
    {
        return mCur;
    }

private:
    QString mTagName;
    QDomNode mParent;
    QDomElement mCur;
    QDomElement mNext;
};

class MutableDomElementIterator
{
public:
    explicit MutableDomElementIterator(QDomNode &parentNode, const QString &tagName = QString())
        : mTagName(tagName)
        , mParent(parentNode)
    {
        toFront();
    }

    void toFront()
    {
        mNext = mParent.firstChildElement(mTagName);
    }

    bool hasNext()
    {
        return (!mNext.isNull());
    }

    QDomElement &next()
    {
        mCur = mNext;
        mNext = mNext.nextSiblingElement(mTagName);
        return mCur;
    }

    void toBack()
    {
        mNext = mParent.lastChildElement(mTagName);
    }

    bool hasPrevious()
    {
        return (!mNext.isNull());
    }

    QDomElement &previous()
    {
        mCur = mNext;
        mNext = mNext.previousSiblingElement(mTagName);
        return mCur;
    }

    QDomElement &current()
    {
        return mCur;
    }

private:
    QString mTagName;
    QDomNode mParent;
    QDomElement mCur;
    QDomElement mNext;
};

QDebug operator<<(QDebug dbg, const QDomElement &el);

#endif // QTXDG_XMLHELPER_H
