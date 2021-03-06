/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef QTXDG_XDGMENURULES_H
#define QTXDG_XDGMENURULES_H

#include <QObject>
#include <QtXml/QDomElement>
#include <QLinkedList>

#include "desktopfile.h"

namespace Liri {

/**
 * See: http://standards.freedesktop.org/desktop-entry-spec
 */

class XdgMenuRule : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuRule(const QDomElement &element, QObject *parent = 0);
    virtual ~XdgMenuRule();

    virtual bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile) = 0;
};

class XdgMenuRuleOr : public XdgMenuRule
{
    Q_OBJECT
public:
    explicit XdgMenuRuleOr(const QDomElement &element, QObject *parent = 0);

    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);

protected:
    QLinkedList<XdgMenuRule *> mChilds;
};

class XdgMenuRuleAnd : public XdgMenuRuleOr
{
    Q_OBJECT
public:
    explicit XdgMenuRuleAnd(const QDomElement &element, QObject *parent = 0);
    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);
};

class XdgMenuRuleNot : public XdgMenuRuleOr
{
    Q_OBJECT
public:
    explicit XdgMenuRuleNot(const QDomElement &element, QObject *parent = 0);
    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);
};

class XdgMenuRuleFileName : public XdgMenuRule
{
    Q_OBJECT
public:
    explicit XdgMenuRuleFileName(const QDomElement &element, QObject *parent = 0);
    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);

private:
    QString mId;
};

class XdgMenuRuleCategory : public XdgMenuRule
{
    Q_OBJECT
public:
    explicit XdgMenuRuleCategory(const QDomElement &element, QObject *parent = 0);
    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);

private:
    QString mCategory;
};

class XdgMenuRuleAll : public XdgMenuRule
{
    Q_OBJECT
public:
    explicit XdgMenuRuleAll(const QDomElement &element, QObject *parent = 0);
    bool check(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);
};

class XdgMenuRules : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuRules(QObject *parent = 0);
    virtual ~XdgMenuRules();

    void addInclude(const QDomElement &element);
    void addExclude(const QDomElement &element);

    bool checkInclude(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);
    bool checkExclude(const QString &desktopFileId, const Liri::DesktopFile &desktopFile);

protected:
    QLinkedList<XdgMenuRule *> mIncludeRules;
    QLinkedList<XdgMenuRule *> mExcludeRules;
};

} // namespace Liri

#endif // QTXDG_XDGMENURULES_H
