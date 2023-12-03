// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_DESKTOPFILE_P_H
#define LIRI_DESKTOPFILE_P_H

#include "desktopfile.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace Liri {

class DesktopFilePrivate : public QSharedData
{
public:
    explicit DesktopFilePrivate() {}

    void clear();

    bool readFile();

    DesktopFile::Type detectType(DesktopFile *q) const;

    bool checkTryExec(const QString &progName) const;

    bool contains(const QString &key) const;

    QString localizedKey(const QString &key) const;

    bool startApplicationDetached(DesktopFile *q, const QString &actionName,
                                  const QStringList &urls);
    bool startLinkDetached(DesktopFile *q);
    void startByDBus(DesktopFile *q, const QString &action, const QStringList &urls);
    bool startProcess(DesktopFile *q, const QString &actionName, const QStringList &urls);

    QString fileName;
    QString prefix;
    QMap<QString, QVariant> items;
    DesktopFile::Type type = DesktopFile::UnknownType;
    QProcessEnvironment env;
};

class DesktopFileCachePrivate
{
public:
    explicit DesktopFileCachePrivate();

    void initialize();
    void initialize(const QString &path);

    DesktopFile *load(const QString &fileName);

    QHash<QString, DesktopFile *> cache;
    QHash<QString, QList<DesktopFile *>> defaultAppsCache;
};

} // namespace Liri

#endif // LIRI_DESKTOPFILE_P_H
