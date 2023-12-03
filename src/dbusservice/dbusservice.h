// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
