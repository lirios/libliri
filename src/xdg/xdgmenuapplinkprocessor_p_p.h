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

#ifndef QTXDG_XDGMENUAPPLINKPROCESSOR_H
#define QTXDG_XDGMENUAPPLINKPROCESSOR_H

#include "xdgmenurules_p_p.h"
#include <QObject>
#include <QtXml/QDomElement>
#include <QLinkedList>
#include <QString>
#include <QHash>

namespace Liri {

class DesktopFile;
class DesktopMenu;

class XdgMenuAppFileInfo;

typedef QLinkedList<XdgMenuAppFileInfo *> XdgMenuAppFileInfoList;

typedef QHash<QString, XdgMenuAppFileInfo *> XdgMenuAppFileInfoHash;
typedef QHash<QString, XdgMenuAppFileInfo *>::iterator XdgMenuAppFileInfoHashIterator;

class XdgMenuApplinkProcessor : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuApplinkProcessor(QDomElement &element, Liri::DesktopMenu *menu, XdgMenuApplinkProcessor *parent = 0);
    virtual ~XdgMenuApplinkProcessor();
    void run();

protected:
    void step1();
    void step2();
    void fillAppFileInfoList();
    void findDesktopFiles(const QString &dirName, const QString &prefix);

    //bool loadDirectoryFile(const QString& fileName, QDomElement& element);
    void createRules();
    bool checkTryExec(const QString &progName);

private:
    XdgMenuApplinkProcessor *mParent;
    QLinkedList<XdgMenuApplinkProcessor *> mChilds;
    XdgMenuAppFileInfoHash mAppFileInfoHash;
    XdgMenuAppFileInfoList mSelected;
    QDomElement mElement;
    bool mOnlyUnallocated;

    Liri::DesktopMenu *mMenu;
    XdgMenuRules mRules;
};

class XdgMenuAppFileInfo : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuAppFileInfo(Liri::DesktopFile *desktopFile, const QString &id, QObject *parent)
        : QObject(parent)
        , mDesktopFile(desktopFile)
        , mAllocated(false)
        , mId(id)
    {
    }

    Liri::DesktopFile *desktopFile() const { return mDesktopFile; }
    bool allocated() const { return mAllocated; }
    void setAllocated(bool value) { mAllocated = value; }
    QString id() const { return mId; }

private:
    Liri::DesktopFile *mDesktopFile;
    bool mAllocated;
    QString mId;
};

} // namespace Liri

#endif // QTXDG_XDGMENUAPPLINKPROCESSOR_H
