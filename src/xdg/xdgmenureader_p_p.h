// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTXDG_XDGMENUREADER_H
#define QTXDG_XDGMENUREADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

namespace Liri {

class DesktopMenu;

class XdgMenuReader : public QObject
{
    Q_OBJECT
public:
    explicit XdgMenuReader(Liri::DesktopMenu *menu, XdgMenuReader *parentReader = 0, QObject *parent = 0);
    virtual ~XdgMenuReader();

    bool load(const QString &fileName, const QString &baseDir = QString());
    QString fileName() const { return mFileName; }
    QString errorString() const { return mErrorStr; }
    QDomDocument &xml() { return mXml; }

protected:
    void processMergeTags(QDomElement &element);
    void processMergeFileTag(QDomElement &element, QStringList *mergedFiles);
    void processMergeDirTag(QDomElement &element, QStringList *mergedFiles);
    void processDefaultMergeDirsTag(QDomElement &element, QStringList *mergedFiles);

    void processAppDirTag(QDomElement &element);
    void processDefaultAppDirsTag(QDomElement &element);

    void processDirectoryDirTag(QDomElement &element);
    void processDefaultDirectoryDirsTag(QDomElement &element);
    void addDirTag(QDomElement &previousElement, const QString &tagName, const QString &dir);

    void mergeFile(const QString &fileName, QDomElement &element, QStringList *mergedFiles);
    void mergeDir(const QString &dirName, QDomElement &element, QStringList *mergedFiles);

private:
    QString mFileName;
    QString mDirName;
    QString mErrorStr;
    QDomDocument mXml;
    XdgMenuReader *mParentReader;
    QStringList mBranchFiles;
    Liri::DesktopMenu *mMenu;
};

} // namespace Liri

#endif // QTXDG_XDGMENUREADER_H
