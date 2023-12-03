// Copyright (C) 2010-2011 Razor team
// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
