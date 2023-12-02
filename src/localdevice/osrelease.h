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

#ifndef OSRELEASE_H
#define OSRELEASE_H

#include <QObject>
#include <QUrl>
#include <QtQmlIntegration>

#include <LiriLocalDevice/lirilocaldeviceglobal.h>

namespace Liri {

class OsReleasePrivate;

class LIRILOCALDEVICE_EXPORT OsRelease : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OsRelease)
    Q_DISABLE_COPY(OsRelease)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QString id READ name NOTIFY idChanged)
    Q_PROPERTY(QStringList idLike READ idLike NOTIFY idLikeChanged)
    Q_PROPERTY(QString versionCodeName READ versionCodeName NOTIFY versionCodeNameChanged)
    Q_PROPERTY(QString versionId READ versionId NOTIFY versionIdChanged)
    Q_PROPERTY(QString prettyName READ prettyName NOTIFY prettyNameChanged)
    Q_PROPERTY(QString ansiColor READ ansiColor NOTIFY ansiColorChanged)
    Q_PROPERTY(QString cpeName READ cpeName NOTIFY cpeNameChanged)
    Q_PROPERTY(QUrl homeUrl READ homeUrl NOTIFY homeUrlChanged)
    Q_PROPERTY(QUrl documentationUrl READ documentationUrl NOTIFY documentationUrlChanged)
    Q_PROPERTY(QUrl supportUrl READ supportUrl NOTIFY supportUrlChanged)
    Q_PROPERTY(QUrl bugReportUrl READ bugReportUrl NOTIFY bugReportUrlChanged)
    Q_PROPERTY(QUrl privacyPolicyUrl READ privacyPolicyUrl NOTIFY privacyPolicyUrlChanged)
    Q_PROPERTY(QString buildId READ buildId NOTIFY buildIdChanged)
    Q_PROPERTY(QString variant READ variant NOTIFY variantChanged)
    Q_PROPERTY(QString variantId READ variantId NOTIFY variantIdChanged)
    Q_PROPERTY(QString logoIconName READ logoIconName NOTIFY logoIconNameChanged)
    QML_ELEMENT
public:
    explicit OsRelease(QObject *parent = nullptr);
    ~OsRelease();

    QString name() const;
    QString version() const;

    QString id() const;
    QStringList idLike() const;

    QString versionCodeName() const;
    QString versionId() const;

    QString prettyName() const;

    QString ansiColor() const;

    QString cpeName() const;

    QUrl homeUrl() const;
    QUrl documentationUrl() const;
    QUrl supportUrl() const;
    QUrl bugReportUrl() const;
    QUrl privacyPolicyUrl() const;

    QString buildId() const;

    QString variant() const;
    QString variantId() const;

    QString logoIconName() const;

Q_SIGNALS:
    void nameChanged();
    void versionChanged();
    void idChanged();
    void idLikeChanged();
    void versionCodeNameChanged();
    void versionIdChanged();
    void prettyNameChanged();
    void ansiColorChanged();
    void cpeNameChanged();
    void homeUrlChanged();
    void documentationUrlChanged();
    void supportUrlChanged();
    void bugReportUrlChanged();
    void privacyPolicyUrlChanged();
    void buildIdChanged();
    void variantChanged();
    void variantIdChanged();
    void logoIconNameChanged();

private:
    OsReleasePrivate *const d_ptr;
};

} // namespace Liri

#endif // OSRELEASE_H
