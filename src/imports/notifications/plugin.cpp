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

#include <QtQml/QtQml>

#include <LiriNotifications/Notification>

#include "notifications.h"
#include "notificationsimageprovider.h"

class LiriNotificationsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri)
    {
        // @uri Liri.Notifications
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Notifications"));

        qmlRegisterSingletonType<Notifications>(uri, 1, 0, "NotificationsService", [](QQmlEngine *engine, QJSEngine *) {
            Notifications *notifications = new Notifications();
            engine->addImageProvider(QStringLiteral("notifications"), new NotificationsImageProvider(notifications->daemon()));
            return static_cast<QObject *>(notifications);
        });
        qmlRegisterType<Liri::Notification>(uri, 1, 0, "Notification");
    }
};

#include "plugin.moc"
