// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
