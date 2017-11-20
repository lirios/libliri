/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QCoreApplication>
#include <QDBusConnectionInterface>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>

#include "defaultlogind_p_p.h"
#include "logind.h"
#include "logind_p.h"

#include <qplatformdefs.h>

#include <sys/sysmacros.h>

Q_LOGGING_CATEGORY(lcLogind, "liri.logind")

const static QString login1Service = QLatin1String("org.freedesktop.login1");
const static QString login1Object = QLatin1String("/org/freedesktop/login1");
const static QString login1ManagerInterface = QLatin1String("org.freedesktop.login1.Manager");
const static QString login1SeatInterface = QLatin1String("org.freedesktop.login1.Seat");
const static QString login1SessionInterface = QLatin1String("org.freedesktop.login1.Session");

const static QString dbusService = QLatin1String("org.freedesktop.DBus");
const static QString dbusPropertiesInterface = QLatin1String("org.freedesktop.DBus.Properties");

namespace Liri {

/*
 * DefaultLogind
 */

DefaultLogind::DefaultLogind(QObject *parent)
    : Logind(QDBusConnection::systemBus(), parent)
{
}

Q_GLOBAL_STATIC(DefaultLogind, s_logind)

/*
 * LogindPrivate
 */

LogindPrivate::LogindPrivate(Logind *qq)
    : bus(QDBusConnection::systemBus())
    , q_ptr(qq)
{
}

void LogindPrivate::_q_serviceRegistered()
{
    Q_Q(Logind);

    // Use session id if available, otherwise get the session by pid
    QString methodName;
    QVariantList args;
    const QByteArray sessionId = qgetenv("XDG_SESSION_ID");
    if (sessionId.isEmpty()) {
        methodName = QLatin1String("GetSessionByPID");
        args << static_cast<quint32>(QCoreApplication::applicationPid());
    } else {
        methodName = QLatin1String("GetSession");
        args << QString::fromLocal8Bit(sessionId);
    }

    // Get the current session as soon as the logind service is registered
    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           login1Object,
                                           login1ManagerInterface,
                                           methodName);
    message.setArguments(args);

    QDBusPendingReply<QDBusObjectPath> result = bus.asyncCall(message);
    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(result, q);
    q->connect(callWatcher, &QDBusPendingCallWatcher::finished, q,
               [q, this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QDBusObjectPath> reply = *w;
        w->deleteLater();

        // Skip if we're already connected
        if (isConnected)
            return;

        // Verify the reply
        if (!reply.isValid()) {
            qCDebug(lcLogind, "Session not registered with logind: %s",
                    qPrintable(reply.error().message()));
            return;
        }

        // Get the session path
        sessionPath = reply.value().path();
        qCDebug(lcLogind) << "Session path:" << sessionPath;

        // We are connected now
        isConnected = true;

        // Listen for lock and unlock signals
        bus.connect(login1Service, sessionPath, login1SessionInterface,
                    QLatin1String("Lock"),
                    q, SIGNAL(lockSessionRequested()));
        bus.connect(login1Service, sessionPath, login1SessionInterface,
                    QLatin1String("Unlock"),
                    q, SIGNAL(unlockSessionRequested()));

        // Listen for properties changed
        bus.connect(login1Service, sessionPath, dbusPropertiesInterface,
                    QLatin1String("PropertiesChanged"),
                    q, SLOT(_q_sessionPropertiesChanged()));

        // Listen for prepare signals
        bus.connect(login1Service, login1Object, login1ManagerInterface,
                    QLatin1String("PrepareForSleep"),
                    q, SIGNAL(prepareForSleep(bool)));
        bus.connect(login1Service, login1Object, login1ManagerInterface,
                    QLatin1String("PrepareForShutdown"),
                    q, SIGNAL(prepareForShutdown(bool)));

        // Activate the session in case we are on another vt, the
        // call blocks on purpose because we need to get properties
        QDBusMessage message =
                QDBusMessage::createMethodCall(login1Service,
                                               sessionPath,
                                               login1ManagerInterface,
                                               QLatin1String("Activate"));
        bus.call(message);

        // Get properties
        _q_sessionPropertiesChanged();

        Q_EMIT q->connectedChanged(isConnected);
    });
}

void LogindPrivate::_q_serviceUnregistered()
{
    Q_Q(Logind);

    // Disconnect prepare signals
    bus.disconnect(login1Service, login1Object, login1ManagerInterface,
                   QLatin1String("PrepareForSleep"),
                   q, SIGNAL(prepareForSleep(bool)));
    bus.disconnect(login1Service, login1Object, login1ManagerInterface,
                   QLatin1String("PrepareForShutdown"),
                   q, SIGNAL(prepareForShutdown(bool)));

    // Connection lost
    isConnected = false;
    Q_EMIT q->connectedChanged(isConnected);

    // Reset properties
    if (sessionActive) {
        sessionActive = false;
        Q_EMIT q->sessionActiveChanged(false);
    }
    if (vt != -1) {
        vt = -1;
        Q_EMIT q->vtNumberChanged(-1);
    }
}

void LogindPrivate::_q_sessionPropertiesChanged()
{
    if (!isConnected || sessionPath.isEmpty())
        return;

    getSessionActive();
    getVirtualTerminal();
}

void LogindPrivate::checkServiceRegistration()
{
    Q_Q(Logind);

    // Get the current session if the logind service is register
    QDBusMessage message =
            QDBusMessage::createMethodCall(dbusService,
                                           QLatin1String("/"),
                                           dbusService,
                                           QLatin1String("ListNames"));

    QDBusPendingReply<QStringList> result = bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, q);
    q->connect(watcher, &QDBusPendingCallWatcher::finished, q,
               [this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QStringList> reply = *w;
        w->deleteLater();

        if (!reply.isValid())
            return;

        if (reply.value().contains(login1Service))
            _q_serviceRegistered();
    });
}

void LogindPrivate::getSessionActive()
{
    Q_Q(Logind);

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           sessionPath,
                                           dbusPropertiesInterface,
                                           QLatin1String("Get"));
    message.setArguments(QVariantList() << login1SessionInterface << QStringLiteral("Active"));

    QDBusPendingReply<QVariant> result = bus.asyncCall(message);
    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(result, q);
    q->connect(callWatcher, &QDBusPendingCallWatcher::finished, q,
               [q, this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QVariant> reply = *w;
        w->deleteLater();

        if (!reply.isValid()) {
            qCWarning(lcLogind, "Failed to get \"Active\" property from session: %s",
                      qPrintable(reply.error().message()));
            return;
        }

        const bool active = reply.value().toBool();
        if (sessionActive != active) {
            sessionActive = active;
            Q_EMIT q->sessionActiveChanged(active);
        }
    });
}

void LogindPrivate::getVirtualTerminal()
{
    Q_Q(Logind);

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           sessionPath,
                                           dbusPropertiesInterface,
                                           QLatin1String("Get"));
    message.setArguments(QVariantList() << login1SessionInterface << QStringLiteral("VTNr"));

    QDBusPendingReply<QVariant> result = bus.asyncCall(message);
    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(result, q);
    q->connect(callWatcher, &QDBusPendingCallWatcher::finished, q,
               [q, this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QVariant> reply = *w;
        w->deleteLater();

        if (!reply.isValid()) {
            qCWarning(lcLogind, "Failed to get \"VTNr\" property from session: %s",
                      qPrintable(reply.error().message()));
            return;
        }

        const uint vtnr = reply.value().toUInt();
        if (vt != static_cast<int>(vtnr)) {
            vt = static_cast<int>(vtnr);
            Q_EMIT q->vtNumberChanged(vt);
        }
    });
}

/*
 * Logind
 */

/*!
 * \class Logind
 * \inmodule LiriLogind
 * \brief Qt-style API for logind.
 *
 * LiriLogind is a Qt-style API for logind.
 *
 * More information on logind can be obtained
 * \l{https://www.freedesktop.org/wiki/Software/systemd/logind/}(here).
 *
 * Logging category is "liri.logind".
 *
 * This class is a singleton, you cannot instantiate it. Use the instance()
 * method to get access.
 */

/*!
 * Constructs a Logind instance with the given \a parent and D-Bus \a connection.
 */
Logind::Logind(const QDBusConnection &connection, QObject *parent)
    : QObject(parent)
    , d_ptr(new LogindPrivate(this))
{
    // Initialize and respond to logind service (un)registration
    Q_D(Logind);
    d->bus = connection;
    d->watcher =
            new QDBusServiceWatcher(login1Service, d->bus,
                                    QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration,
                                    this);
    connect(d->watcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(_q_serviceRegistered()));
    connect(d->watcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(_q_serviceUnregistered()));

    // Is logind already registered?
    d->checkServiceRegistration();
}

/*!
 * Destroys the Logind object.
 */
Logind::~Logind()
{
    delete d_ptr;
}

/*!
 * Return the instance of Logind.
 */
Logind *Logind::instance()
{
    return s_logind();
}

bool Logind::checkService()
{
    QDBusConnectionInterface *interface = QDBusConnection::systemBus().interface();
    return interface->isServiceRegistered(login1Service);
}

/*!
 * \property Logind::isConnected
 *
 * This property holds whether we are connected to logind.
 */
bool Logind::isConnected() const
{
    Q_D(const Logind);
    return d->isConnected;
}

/*!
 * \property Logind::isConnected
 *
 * This property holds whether session control was acquired.
 *
 * \sa Logind::takeControl()
 * \sa Logind::releaseControl()
 */
bool Logind::hasSessionControl() const
{
    Q_D(const Logind);
    return d->hasSessionControl;
}

/*!
 * \property Logind::isSessionActive
 *
 * This property holds whether the session this process lives in
 * is active or not, that is whether the current vt is the one
 * where the session was executed.
 */
bool Logind::isSessionActive() const
{
    Q_D(const Logind);
    return d->sessionActive;
}

/*!
 * \property Logind::isInhibited
 *
 * This property holds if there are inhibitions installed.
 */
bool Logind::isInhibited() const
{
    Q_D(const Logind);
    return d->inhibitFds.size() > 0;
}

/*!
 * \property Logind::vtNumber
 *
 * This property holds the current vt number.
 */
int Logind::vtNumber() const
{
    Q_D(const Logind);
    return d->vt;
}

/*!
 * Set the idle hint state to \a idle.
 */
void Logind::setIdleHint(bool idle)
{
    Q_D(Logind);

    if (!d->isConnected || d->sessionPath.isEmpty())
        return;

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QStringLiteral("SetIdleHint"));
    d->bus.asyncCall(message, idle);
}

/*!
 * Install an inhibitor.
 * \param who Name of the application that is installing the inhibitor
 * \param why Reason why the inhibitor is being installed
 * \param flags What is being inhibited
 * \param mode Inhibition mode
 *
 * \sa Logind::inhibited()
 * \sa Logind::uninhibited()
 */
void Logind::inhibit(const QString &who, const QString &why,
                     InhibitFlags flags, InhibitMode mode)
{
    Q_D(Logind);

    if (!d->isConnected)
        return;

    QStringList what;
    if (flags.testFlag(InhibitShutdown))
        what.append(QStringLiteral("shutdown"));
    if (flags.testFlag(InhibitSleep))
        what.append(QStringLiteral("sleep"));
    if (flags.testFlag(InhibitIdle))
        what.append(QStringLiteral("idle"));
    if (flags.testFlag(InhibitPowerKey))
        what.append(QStringLiteral("handle-power-key"));
    if (flags.testFlag(InhibitSuspendKey))
        what.append(QStringLiteral("handle-suspend-key"));
    if (flags.testFlag(InhibitHibernateKey))
        what.append(QStringLiteral("handle-hibernate-key"));
    if (flags.testFlag(InhibitLidSwitch))
        what.append(QStringLiteral("handle-lid-switch"));

    QString modeStr = mode == Block ? QStringLiteral("block") : QStringLiteral("delay");

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           login1Object,
                                           login1ManagerInterface,
                                           QLatin1String("Inhibit"));
    message.setArguments(QVariantList() << what.join(':') << who << why << modeStr);

    QDBusPendingReply<QDBusUnixFileDescriptor> result = d->bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [d, this, who, why](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QDBusUnixFileDescriptor> reply = *w;
        w->deleteLater();

        if (!reply.isValid()) {
            qCWarning(lcLogind, "Unable to acquire inhibition lock: %s",
                      qPrintable(reply.error().message()));
            return;
        }

        qCDebug(lcLogind) << "Inhibition lock acquired successfully";

        const int fd = ::dup(reply.value().fileDescriptor());
        d->inhibitFds.append(fd);
        if (d->inhibitFds.size() == 1)
            Q_EMIT inhibitedChanged(true);
        Q_EMIT inhibited(who, why, fd);
    });
}

/*!
 * Uninstall the inhibitor with \a fd file descriptor.
 */
void Logind::uninhibit(int fd)
{
    Q_D(Logind);

    if (!d->isConnected || !d->inhibitFds.contains(fd))
        return;

    ::close(fd);
    d->inhibitFds.removeOne(fd);

    if (d->inhibitFds.size() == 0)
        Q_EMIT inhibitedChanged(false);
    Q_EMIT uninhibited(fd);
}

/*!
 * Lock the session.
 *
 * \sa Logind::unlockSession()
 */
void Logind::lockSession()
{
    Q_D(Logind);

    if (!d->isConnected || d->sessionPath.isEmpty())
        return;

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("Lock"));
    d->bus.asyncCall(message);
}

/*!
 * Unlock the session.
 *
 * \sa Logind::lockSession()
 */
void Logind::unlockSession()
{
    Q_D(Logind);

    if (!d->isConnected || d->sessionPath.isEmpty())
        return;

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("Unlock"));
    d->bus.asyncCall(message);
}

/*!
 * Take control of the session. No other process will be
 * able to take control, until the releaaseControl() method is called.
 *
 * Access to video and input devices will be granted to the caller.
 *
 * \sa Logind::releaseControl()
 */
void Logind::takeControl()
{
    Q_D(Logind);

    if (!d->isConnected || d->sessionPath.isEmpty() || d->hasSessionControl)
        return;

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("TakeControl"));
    message.setArguments(QVariantList() << false);

    QDBusPendingReply<void> result = d->bus.asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(result, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [d, this](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<void> reply = *w;
        w->deleteLater();

        if (!reply.isValid()) {
            qCWarning(lcLogind, "Unable to take control of the session: %s",
                      qPrintable(reply.error().message()));
            d->hasSessionControl = false;
            Q_EMIT hasSessionControlChanged(d->hasSessionControl);
            return;
        }

        qCDebug(lcLogind) << "Acquired control of the session";
        d->hasSessionControl = true;
        Q_EMIT hasSessionControlChanged(d->hasSessionControl);

        d->bus.connect(login1Service, d->sessionPath, login1SessionInterface,
                       QLatin1String("PauseDevice"),
                       this, SIGNAL(devicePaused(quint32,quint32,QString)));
        d->bus.connect(login1Service, d->sessionPath, login1SessionInterface,
                       QLatin1String("ResumeDevice"),
                       this, SIGNAL(deviceResumed(quint32,quint32,int)));
    });
}

/*!
 * Release control of the session.
 *
 * Access to video and input devices will be revoked from the caller.
 *
 * \sa Logind::takeControl()
 */
void Logind::releaseControl()
{
    Q_D(Logind);

    if (!d->isConnected || d->sessionPath.isEmpty() || !d->hasSessionControl)
        return;

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("ReleaseControl"));
    d->bus.asyncCall(message);

    qCDebug(lcLogind) << "Released control of the session";
    d->hasSessionControl = false;
    Q_EMIT hasSessionControlChanged(d->hasSessionControl);
}

/*!
 * Request access to the device \a fileName.
 *
 * \return File descriptor of the device
 *
 * \sa Logind::releaseDevice()
 */
int Logind::takeDevice(const QString &fileName)
{
    Q_D(Logind);

    struct stat st;
    if (::stat(qPrintable(fileName), &st) < 0) {
        qCWarning(lcLogind, "Failed to stat: %s", qPrintable(fileName));
        return -1;
    }

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("TakeDevice"));
    message.setArguments(QVariantList()
                         << QVariant(major(st.st_rdev))
                         << QVariant(minor(st.st_rdev)));

    // Block until the device is taken
    QDBusMessage reply = d->bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qCWarning(lcLogind, "Failed to take device \"%s\": %s",
                  qPrintable(fileName), qPrintable(reply.errorMessage()));
        return -1;
    }

    const int fd = reply.arguments().first().value<QDBusUnixFileDescriptor>().fileDescriptor();
    return ::fcntl(fd, F_DUPFD_CLOEXEC, 0);
}

/*
 * Revoke access to the device \a fd.
 *
 * \sa Logind::takeDevice()
 */
void Logind::releaseDevice(int fd)
{
    Q_D(Logind);

    struct stat st;
    if (::fstat(fd, &st) < 0) {
        qCWarning(lcLogind, "Failed to stat the file descriptor");
        return;
    }

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("ReleaseDevice"));
    message.setArguments(QVariantList()
                         << QVariant(major(st.st_rdev))
                         << QVariant(minor(st.st_rdev)));

    d->bus.asyncCall(message);
}

/*!
 * Allow a session-controller to synchronously pause a device
 * with \a devMajor major and \a devMinor minor after receiving
 * the devicePaused signal.
 *
 * \sa Logind::devicePaused()
 */
void Logind::pauseDeviceComplete(quint32 devMajor, quint32 devMinor)
{
    Q_D(Logind);

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           d->sessionPath,
                                           login1SessionInterface,
                                           QLatin1String("PauseDeviceComplete"));
    message.setArguments(QVariantList() << devMajor << devMinor);

    d->bus.asyncCall(message);
}

/*!
 * Switch to the vt number \a vt.
 */
void Logind::switchTo(quint32 vt)
{
    Q_D(Logind);

    QDBusMessage message =
            QDBusMessage::createMethodCall(login1Service,
                                           QLatin1String("/org/freedesktop/login1/seat/self"),
                                           login1SeatInterface,
                                           QLatin1String("SwitchTo"));
    message.setArguments(QVariantList() << QVariant(vt));

    d->bus.asyncCall(message);
}

} // namespace Liri

#include "moc_logind.cpp"
