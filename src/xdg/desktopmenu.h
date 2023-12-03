// Copyright (C) 2010-2011 Razor team
// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_DESKTOPMENU_H
#define LIRI_DESKTOPMENU_H

#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include <LiriXdg/lirixdgglobal.h>

namespace Liri {

class DesktopMenuPrivate;

/*! @brief The XdgMenu class implements the "Desktop Menu Specification" from freedesktop.org.

 Freedesktop menu is a user-visible hierarchy of applications, typically displayed as a menu.

 Example usage:
@code
    QString menuFile = XdgMenu::getMenuFileName();
    XdgMenu xdgMenu();

    bool res = xdgMenu.read(menuFile);
    if (!res)
    {
        QMessageBox::warning(this, "Parse error", xdgMenu.errorString());
    }

    QDomElement rootElement = xdgMenu.xml().documentElement()
 @endcode

 @sa http://specifications.freedesktop.org/menu-spec/menu-spec-latest.html
 */

class LIRIXDG_EXPORT DesktopMenu : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DesktopMenu)

    friend class XdgMenuReader;
    friend class XdgMenuApplinkProcessor;

public:
    explicit DesktopMenu(QObject *parent = nullptr);
    virtual ~DesktopMenu();

    bool read(const QString &menuFileName);
    void save(const QString &fileName);

    const QDomDocument xml() const;
    QString menuFileName() const;

    QDomElement findMenu(QDomElement &baseElement, const QString &path, bool createNonExisting);

    /*! Returns a  list of strings identifying the environments that should
     *  display a desktop entry. Internally all comparisions involving the
     *  desktop enviroment names are made case insensitive.
     */
    QStringList environments();

    /*!
     *  Set currently running environments. Example: RAZOR, KDE, or GNOME...
     *  Internally all comparisions involving the desktop enviroment names
     *  are made case insensitive.
     */
    void setEnvironments(const QStringList &envs);
    void setEnvironments(const QString &env);

    /*!
     * Returns a string description of the last error that occurred if read() returns false.
     */
    const QString errorString() const;

    /*!
     * @brief The name of the directory for the debug XML-files.
     */
    const QString logDir() const;

    /*!
     * @brief The name of the directory for the debug XML-files. If a directory is specified,
     * then after you run the DesktopMenu::read, you can see and check the results of the each step.
     */
    void setLogDir(const QString &directory);

    static QString getMenuFileName(const QString &baseName = QStringLiteral("applications.menu"));

    bool isOutDated() const;

Q_SIGNALS:
    void changed();

protected:
    void addWatchPath(const QString &path);

private:
    DesktopMenuPrivate *const d_ptr;
};

} // namespace Liri

#endif // LIRI_DESKTOPMENU_H
