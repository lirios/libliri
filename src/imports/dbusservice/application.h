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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QUrl>
#include <QVariantMap>

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
public:
    explicit Application(QObject *parent = nullptr);

    QString serviceName() const;
    void setServiceName(const QString &name);

    QString objectPath() const;
    void setObjectPath(const QString &path);

    Q_INVOKABLE void activate(const QVariantMap &platformData = QVariantMap());
    Q_INVOKABLE void activateAction(const QString &name, const QVariantList &parameters = QVariantList(), const QVariantMap &platformData = QVariantMap());
    Q_INVOKABLE void open(const QStringList &uris = QStringList(), const QVariantMap &platformData = QVariantMap());

Q_SIGNALS:
    void serviceNameChanged();
    void objectPathChanged();

private:
    QString m_serviceName;
    QString m_objectPath;
};

#endif // APPLICATION_H
