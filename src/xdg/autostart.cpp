// Copyright (C) 2010-2011 Razor team
// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QSet>

#include "autostart.h"
#include "xdgdirs_p_p.h"

/**
 * The Autostart Directories are $XDG_CONFIG_DIRS/autostart. If the same filename is
 * located under multiple Autostart Directories only the file under the most
 * important directory should be used.
 *
 * When multiple .desktop files with the same name exists in multiple directories
 * then only the Hidden key in the most important .desktop file must be considered:
 * If it is set to true all .desktop files with the same name in the other
 * directories MUST be ignored as well.
 */

namespace Liri {

DesktopFileList AutoStart::desktopFileList(bool excludeHidden)
{
    QStringList dirs;
    dirs << XdgDirs::autostartHome(false) << XdgDirs::autostartDirs();

    return desktopFileList(dirs, excludeHidden);
}

DesktopFileList AutoStart::desktopFileList(QStringList dirs, bool excludeHidden)
{
    dirs.removeDuplicates();

    QSet<QString> processed;
    DesktopFileList ret;
    for (const QString &dirName : const_cast<const QStringList &>(dirs)) {
        QDir dir(dirName);
        if (!dir.exists())
            continue;

        const QFileInfoList files = dir.entryInfoList(QStringList(QStringLiteral("*.desktop")), QDir::Files | QDir::Readable);
        for (const QFileInfo &fi : files) {
            if (processed.contains(fi.fileName()))
                continue;

            processed << fi.fileName();

            DesktopFile desktop;
            if (!desktop.load(fi.absoluteFilePath()))
                continue;
            if (excludeHidden && !desktop.isVisible())
                continue;
            if (!desktop.isSuitable())
                continue;

            ret << desktop;
        }
    }
    return ret;
}

QString AutoStart::localPath(const DesktopFile &file)
{
    QFileInfo fi(file.fileName());
    return QStringLiteral("%1/%2").arg(XdgDirs::autostartHome(), fi.fileName());
}

} // namespace Liri
