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

#include <QObject>
#include <QtQml>

#include <LiriLocalDevice/LocalDevice>

QML_DECLARE_TYPE(Liri::OsRelease)

static QObject *localDeviceProvider(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new Liri::LocalDevice();
}

class LiriDevicePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    LiriDevicePlugin()
    {
    }

    void registerTypes(const char *uri) override
    {
        // @uri Liri.Device
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Device"));

        qmlRegisterSingletonType<Liri::LocalDevice>(uri, 1, 0, "LocalDevice", localDeviceProvider);
        qmlRegisterUncreatableType<Liri::OsRelease>(uri, 1, 0, "OsRelease",
                                                    QStringLiteral("Cannot create OsRelease"));
    }
};

#include "plugin.moc"
