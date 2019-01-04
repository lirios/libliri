/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2010-2011 Razor team
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

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
    return QString::fromLatin1("%1/%2").arg(XdgDirs::autostartHome(), fi.fileName());
}

} // namespace Liri
