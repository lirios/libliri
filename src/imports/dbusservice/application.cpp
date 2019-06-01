/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDBusConnection>
#include <QDBusMessage>
#include <QProcess>

#include "application.h"

Application::Application(QObject *parent)
    : QObject(parent)
{
}

QString Application::serviceName() const
{
    return m_serviceName;
}

void Application::setServiceName(const QString &name)
{
    if (m_serviceName == name)
        return;

    m_serviceName = name;
    Q_EMIT serviceNameChanged();
}

QString Application::objectPath() const
{
    return m_objectPath;
}

void Application::setObjectPath(const QString &path)
{
    if (m_objectPath == path)
        return;

    m_objectPath = path;
    Q_EMIT objectPathChanged();
}

void Application::activate(const QVariantMap &platformData)
{
    QProcess::execute(QStringLiteral("dbus-update-activation-environment --all"));

    auto msg = QDBusMessage::createMethodCall(
                m_serviceName, m_objectPath,
                QStringLiteral("org.freedesktop.Application"),
                QStringLiteral("Activate"));
    QVariantList args;
    args.append(platformData);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void Application::activateAction(const QString &name, const QVariantList &parameters, const QVariantMap &platformData)
{
    QProcess::execute(QStringLiteral("dbus-update-activation-environment --all"));

    auto msg = QDBusMessage::createMethodCall(
                m_serviceName, m_objectPath,
                QStringLiteral("org.freedesktop.Application"),
                QStringLiteral("ActivateAction"));
    QVariantList args;
    args.append(name);
    args.append(parameters);
    args.append(platformData);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}

void Application::open(const QStringList &uris, const QVariantMap &platformData)
{
    QProcess::execute(QStringLiteral("dbus-update-activation-environment --all"));

    auto msg = QDBusMessage::createMethodCall(
                m_serviceName, m_objectPath,
                QStringLiteral("org.freedesktop.Application"),
                QStringLiteral("Open"));
    QVariantList args;
    args.append(uris);
    args.append(platformData);
    msg.setArguments(args);
    QDBusConnection::sessionBus().send(msg);
}
