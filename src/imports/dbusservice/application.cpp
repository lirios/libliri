// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
