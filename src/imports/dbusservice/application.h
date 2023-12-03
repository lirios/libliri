// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QQmlEngine>

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    QML_ELEMENT
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
