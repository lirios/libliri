// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
