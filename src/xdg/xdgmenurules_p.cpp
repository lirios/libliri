// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "xdgmenurules_p_p.h"
#include "xmlhelper_p_p.h"

#include <QDebug>
#include <QStringList>

namespace Liri {

/**
 * See: http://standards.freedesktop.org/desktop-entry-spec
 */

XdgMenuRule::XdgMenuRule(const QDomElement &element, QObject *parent)
    : QObject(parent) {
        Q_UNUSED(element)
    }

    XdgMenuRule::~XdgMenuRule()
{
}

/************************************************
 The <Or> element contains a list of matching rules. If any of the matching rules
 inside the <Or> element match a desktop entry, then the entire <Or> rule matches
 the desktop entry.
 ************************************************/
XdgMenuRuleOr::XdgMenuRuleOr(const QDomElement &element, QObject *parent)
    : XdgMenuRule(element, parent)
{
    //qDebug() << "Create OR rule";
    DomElementIterator iter(element, QString());

    while (iter.hasNext()) {
        QDomElement e = iter.next();

        if (e.tagName() == QLatin1String("Or"))
            mChilds.append(new XdgMenuRuleOr(e, this));

        else if (e.tagName() == QLatin1String("And"))
            mChilds.append(new XdgMenuRuleAnd(e, this));

        else if (e.tagName() == QLatin1String("Not"))
            mChilds.append(new XdgMenuRuleNot(e, this));

        else if (e.tagName() == QLatin1String("Filename"))
            mChilds.append(new XdgMenuRuleFileName(e, this));

        else if (e.tagName() == QLatin1String("Category"))
            mChilds.append(new XdgMenuRuleCategory(e, this));

        else if (e.tagName() == QLatin1String("All"))
            mChilds.append(new XdgMenuRuleAll(e, this));

        else
            qWarning() << QStringLiteral("Unknown rule") << e.tagName();
    }
}

bool XdgMenuRuleOr::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    for (QLinkedList<XdgMenuRule *>::Iterator i = mChilds.begin(); i != mChilds.end(); ++i)
        if ((*i)->check(desktopFileId, desktopFile))
            return true;

    return false;
}

/************************************************
 The <And> element contains a list of matching rules. If each of the matching rules
 inside the <And> element match a desktop entry, then the entire <And> rule matches
 the desktop entry.
 ************************************************/
XdgMenuRuleAnd::XdgMenuRuleAnd(const QDomElement &element, QObject *parent)
    : XdgMenuRuleOr(element, parent)
{
    //    qDebug() << "Create AND rule";
}

bool XdgMenuRuleAnd::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    for (QLinkedList<XdgMenuRule *>::Iterator i = mChilds.begin(); i != mChilds.end(); ++i)
        if (!(*i)->check(desktopFileId, desktopFile))
            return false;

    return mChilds.count();
}

/************************************************
 The <Not> element contains a list of matching rules. If any of the matching rules
 inside the <Not> element matches a desktop entry, then the entire <Not> rule does
 not match the desktop entry. That is, matching rules below <Not> have a logical OR
 relationship.
 ************************************************/
XdgMenuRuleNot::XdgMenuRuleNot(const QDomElement &element, QObject *parent)
    : XdgMenuRuleOr(element, parent)
{
    //    qDebug() << "Create NOT rule";
}

bool XdgMenuRuleNot::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    return !XdgMenuRuleOr::check(desktopFileId, desktopFile);
}

/************************************************
 The <Filename> element is the most basic matching rule. It matches a desktop entry
 if the desktop entry has the given desktop-file id. See Desktop-File Id.
 ************************************************/
XdgMenuRuleFileName::XdgMenuRuleFileName(const QDomElement &element, QObject *parent)
    : XdgMenuRule(element, parent)
{
    //qDebug() << "Create FILENAME rule";
    mId = element.text();
}

bool XdgMenuRuleFileName::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    Q_UNUSED(desktopFile)
    //qDebug() << "XdgMenuRuleFileName:" << desktopFileId << mId;
    return desktopFileId == mId;
}

/************************************************
 The <Category> element is another basic matching predicate. It matches a desktop entry
 if the desktop entry has the given category in its Categories field.
 ************************************************/
XdgMenuRuleCategory::XdgMenuRuleCategory(const QDomElement &element, QObject *parent)
    : XdgMenuRule(element, parent)
{
    mCategory = element.text();
}

bool XdgMenuRuleCategory::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    Q_UNUSED(desktopFileId)
    QStringList cats = desktopFile.categories();
    return cats.contains(mCategory);
}

/************************************************
 The <All> element is a matching rule that matches all desktop entries.
 ************************************************/
XdgMenuRuleAll::XdgMenuRuleAll(const QDomElement &element, QObject *parent)
    : XdgMenuRule(element, parent)
{
}

bool XdgMenuRuleAll::check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    Q_UNUSED(desktopFileId)
    Q_UNUSED(desktopFile)
    return true;
}

XdgMenuRules::XdgMenuRules(QObject *parent)
    : QObject(parent)
{
}

XdgMenuRules::~XdgMenuRules()
{
}

void XdgMenuRules::addInclude(const QDomElement &element)
{
    mIncludeRules.append(new XdgMenuRuleOr(element, this));
}

void XdgMenuRules::addExclude(const QDomElement &element)
{
    mExcludeRules.append(new XdgMenuRuleOr(element, this));
}

bool XdgMenuRules::checkInclude(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    for (QLinkedList<XdgMenuRule *>::Iterator i = mIncludeRules.begin(); i != mIncludeRules.end(); ++i)
        if ((*i)->check(desktopFileId, desktopFile))
            return true;

    return false;
}

bool XdgMenuRules::checkExclude(const QString &desktopFileId, const Liri::DesktopFile &desktopFile)
{
    for (QLinkedList<XdgMenuRule *>::Iterator i = mExcludeRules.begin(); i != mExcludeRules.end(); ++i)
        if ((*i)->check(desktopFileId, desktopFile))
            return true;

    return false;
}

} // namespace Liri
