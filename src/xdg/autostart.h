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

#ifndef LIRI_AUTOSTART_H
#define LIRI_AUTOSTART_H

#include <LiriXdg/DesktopFile>

namespace Liri {

/*! @brief The AutoStart class implements the "Desktop Application Autostart Specification"
 * from freedesktop.org.
 * This specification defines a method for automatically starting applications during the startup
 * of a desktop environment and after mounting a removable medium.
 * Now we impliment only startup.
 *
 * @sa http://standards.freedesktop.org/autostart-spec/autostart-spec-latest.html
 */
class LIRIXDG_EXPORT AutoStart
{
public:
    /*! Returns a list of DesktopFile objects for all the .desktop files in the Autostart directories
        When the .desktop file has the Hidden key set to true, the .desktop file must be ignored. But you
        can change this behavior by setting excludeHidden to false. */
    static DesktopFileList desktopFileList(bool excludeHidden = true);

    /*! Returns a list of DesktopFile objects for .desktop files in the specified Autostart directories
        When the .desktop file has the Hidden key set to true, the .desktop file must be ignored. But you
        can change this behavior by setting excludeHidden to false. */
    static DesktopFileList desktopFileList(QStringList dirs, bool excludeHidden = true);

    /// For DesktopFile returns the file path of the same name in users personal autostart directory.
    static QString localPath(const DesktopFile &file);
};

} // namespace Liri

#endif // LIRI_AUTOSTART_H
