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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <LiriDBusService/liridbusserviceglobal.h>

class ApplicationAdaptor;

namespace Liri {

class DBusServicePrivate;

class LIRIDBUSSERVICE_EXPORT DBusService : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DBusService)
public:
    enum StartupOption {
        Single          = 1,
        Multiple        = 2,
        NoExitOnFailure = 4
    };
    Q_ENUM(StartupOption)
    Q_DECLARE_FLAGS(StartupOptions, StartupOption)
    Q_FLAGS(StartupOptions)

    explicit DBusService(StartupOptions options = Multiple, QObject *parent = nullptr);
    ~DBusService();

    bool isRegistered() const;

    QString errorMessage() const;

Q_SIGNALS:
    void activationRequested(const QVariantMap &data);
    void actionActivationRequested(const QString &name,
                                   const QVariantList &args,
                                   const QVariantMap &data);
    void openRequested(const QList<QUrl> &urls, const QVariantMap &data);

public Q_SLOTS:
    void unregister();

private:
    DBusServicePrivate *const d_ptr;

    friend class ::ApplicationAdaptor;

private Q_SLOTS:
    void Activate(const QVariantMap &platform_data);
    void ActivateAction(const QString &action_name, const QVariantList &parameter, const QVariantMap &platform_data);
    void Open(const QStringList &uris, const QVariantMap &platform_data);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DBusService::StartupOptions)

} // namespace Liri
