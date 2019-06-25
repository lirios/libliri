/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QFileInfo>
#include <QtDBus/QDBusConnection>
#include <QtGui/QGuiApplication>

#include "application_adaptor.h"
#include "dbusservice.h"
#include "dbusservice_p.h"

namespace Liri {

DBusServicePrivate::DBusServicePrivate()
{
}

QString DBusServicePrivate::getServiceName() const
{
    QString name;
    const QString desktopFileName = QGuiApplication::desktopFileName();

    if (desktopFileName.isEmpty()) {
        const QString domain = QCoreApplication::organizationDomain();
        const QStringList parts = domain.split(QLatin1Char('.'), QString::SkipEmptyParts);

        QString reverseDomain;
        if (parts.isEmpty()) {
            reverseDomain = QStringLiteral("local");
        } else {
            for (const QString &part : parts) {
                reverseDomain.prepend(QLatin1Char('.'));
                reverseDomain.prepend(part);
            }
        }

        name = reverseDomain + QCoreApplication::applicationName();
    } else {
        QFileInfo info(desktopFileName);
        name = info.fileName().replace(QRegularExpression(QStringLiteral("\\.desktop$")), QString());
    }

    return name;
}

QString DBusServicePrivate::getObjectPath(const QString &name) const
{
    QString path = QLatin1Char('/') + name;
    path.replace(QLatin1Char('.'), QLatin1Char('/'));
    path.replace(QLatin1Char('-'), QLatin1Char('_'));
    return path;
}

/*!
 * \class DBusService
 * \inmodule LiriDBusService
 * \brief Registers the application with D-Bus
 *
 * DBusService allows you to register the application with D-Bus
 * and make it available for D-Bus activation.
 *
 * The application is registered at a predictable location on the
 * session bus with the org.freedesktop.Application documented
 * \l{https://specifications.freedesktop.org/desktop-entry-spec/latest/ar01s07.html}(here).
 *
 * If QGuiApplication::desktopFileName is defined, DBusService will use the
 * base name of that minus the ".desktop" suffix as D-Bus service name;
 * otherwise the reversed domain name (or "local" if it's not defined)
 * plus the application name.
 *
 * An application can either work in Single or Multiple mode.
 *
 * In Single mode, only one instance of the application is allowed.
 * The first instance registers with D-Bus and any attempt to run the
 * application again will cause the activationRequested() signal to be
 * emitted in the already running application; the duplicate instance will
 * then quit.
 * Applications in single mode should parse command line arguments later,
 * after creating the DBusService object, in order to make sure they are
 * the original instance of the application.
 *
 * In Multiple mode, multiple instances of the application are allowed.
 * The instance PID will be appended to the D-Bus service name to
 * distinguish between instances.
 *
 * Applications that are D-Bus activatable need to set the D-Bus activation
 * entries in their desktop files, set QGuiApplication::desktopFileName
 * accordingly, use Single mode and connect to the signals emitted by
 * DBusService.
 *
 * This is an example of desktop file for a D-Bus activatable application,
 * and it's called \c org.domain.MyApplication.desktop:
 *
 * \code
 * [Desktop Entry]
 * Name=MyApplication
 * Comment=A single instance application
 * Exec=myapplication %U
 * Icon=org.domain.MyApplication
 * Terminal=false
 * Type=Application
 * DBusActivatable=true
 * Categories=Qt;Utility;
 * \endcode
 *
 * Below an example usage:
 *
 * \code
 * #include <QtGui/QGuiApplication>
 * #include <LiriDBusService/DBusService>
 *
 * int main(int argc, char *argv[])
 * {
 *     QGuiApplication app(argc, argv);
 *     app.setOrganizationDomain(QLatin1String("domain.org"));
 *     app.setApplicationName(QLatin1String("myapplication"));
 *     app.setDesktopFileName(QLatin1String("org.domain.MyApplication.desktop"));
 *
 *     Liri::DBusService service(Liri::DBusService::Single);
 *
 *     // At this point we are sure there is only one instance
 *
 *     // Command line parsing goes here
 *
 *     // Connect DBusService signals here
 *
 *     return app.exec();
 * }
 * \endcode
 *
 * \sa QGuiApplication::desktopFileName
 * \sa QCoreApplication::domainName
 * \sa QCoreApplication::applicationName
 */

/*!
 * \qmltype Application
 * \inqmlmodule Liri.DBusService
 * \brief Client to launch D-Bus activatable applications.
 *
 * Application allows you to open up an activatable application, using
 * the org.freedesktop.Application interface.
 *
 * \code
 * import Liri.DBusService 1.0 as DBusService
 *
 * DBusService.Application {
 *     id: app
 *     serviceName: "org.vendor.MyApp"
 *     objectPath: "/org/vendor/MyApp"
 * }
 *
 * Button {
 *     onClicked: app.activate()
 * }
 * \endcode
 */

/*!
 * \enum DBusService::StartupOption
 *
 * This enum type is used to specify startup options.
 *
 * \value Single Only a single instance of the application is allowed.
 * \value Multiple Multiple instances of the application are allowed.
 * \value NoExitOnFailure Do not exit if D-Bus registration fails.
 */

/*!
 * Constructs a DBusService object with the specified \a options.
 */
DBusService::DBusService(StartupOptions options, QObject *parent)
    : QObject(parent)
    , d_ptr(new DBusServicePrivate())
{
    Q_D(DBusService);

    new ApplicationAdaptor(this);

    QDBusConnectionInterface *bus = nullptr;

    if (!QDBusConnection::sessionBus().isConnected() || !(bus = QDBusConnection::sessionBus().interface())) {
        d->errorMessage = QStringLiteral("Failed to connect to the D-Bus session bus, "
                                         "make sure the D-Bus daemon is running.");
    }

    if (bus) {
        d->serviceName = d->getServiceName();

        QString objectPath = d->getObjectPath(d->serviceName);

        // Append PID for multiple mode
        if (options.testFlag(Multiple)) {
            const QString pid = QString::number(QCoreApplication::applicationPid());
            d->serviceName += QLatin1Char('-') + pid;
        }

        // Register object
        QDBusConnection::sessionBus().registerObject(objectPath, this,
                                                     QDBusConnection::ExportAdaptors);

        d->registered = bus->registerService(d->serviceName) == QDBusConnectionInterface::ServiceRegistered;

        // Handle registration
        if (d->registered) {
            connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                    this, &DBusService::unregister);
        } else {
            if (options.testFlag(Single)) {
                QDBusMessage msg =
                        QDBusMessage::createMethodCall(d->serviceName,
                                                       objectPath,
                                                       QStringLiteral("org.freedesktop.Application"),
                                                       QStringLiteral("Activate"));
                QVariantMap data;
                data.insert(QStringLiteral("desktop-startup-id"), QString::fromUtf8(qgetenv("DESKTOP_STARTUP_ID")));
                msg.setArguments(QVariantList() << data);
                QDBusMessage reply = QDBusConnection::sessionBus().call(msg, QDBus::Block, 1 * 60 * 1000);
                if (reply.type() == QDBusMessage::ErrorMessage)
                    d->errorMessage = reply.errorMessage();
                else
                    exit(0);
            } else {
                d->errorMessage = QStringLiteral("Failed to register '%1' with D-Bus, "
                                                 "another instance already owns it!")
                        .arg(d->serviceName);
            }
        }
    }

    if (!d->registered && !options.testFlag(NoExitOnFailure)) {
        qCritical("%s", qPrintable(d->errorMessage));
        exit(1);
    }
}

/*!
 * Destructs a DBusService object.
 */
DBusService::~DBusService()
{
    delete d_ptr;
}

/*!
 * Returns whether the application is registered.
 */
bool DBusService::isRegistered() const
{
    Q_D(const DBusService);
    return d->registered;
}

/*!
 * Returns the error message from the D-Bus registration, if failed.
 */
QString DBusService::errorMessage() const
{
    Q_D(const DBusService);
    return d->errorMessage;
}

/*!
 * Unregisters the service from D-Bus.
 * Automatically called when the application quits.
 */
void DBusService::unregister()
{
    Q_D(DBusService);

    if (!QDBusConnection::sessionBus().isConnected() || !d->registered)
        return;

    QDBusConnectionInterface *bus = QDBusConnection::sessionBus().interface();
    if (bus)
        bus->unregisterService(d->serviceName);
}

/*!
 * \internal
 */
void DBusService::Activate(const QVariantMap &platform_data)
{
    Q_EMIT activationRequested(platform_data);
}

/*!
 * \internal
 */
void DBusService::ActivateAction(const QString &action_name, const QVariantList &parameter, const QVariantMap &platform_data)
{
    Q_EMIT actionActivationRequested(action_name, parameter, platform_data);
}

/*!
 * \internal
 */
void DBusService::Open(const QStringList &uris, const QVariantMap &platform_data)
{
    Q_EMIT openRequested(QUrl::fromStringList(uris), platform_data);
}

} // namespace Liri
