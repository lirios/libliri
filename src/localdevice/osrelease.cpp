// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

#include "osrelease_p.h"

namespace Liri {

OsReleasePrivate::OsReleasePrivate(OsRelease *self)
    : q_ptr(self)
{
}

void OsReleasePrivate::readOsRelease()
{
    const auto etcOsRelease = QStringLiteral("/etc/os-release");
    const auto usrLibOsRelease = QStringLiteral("/usr/lib/os-release");

    if (QFile::exists(etcOsRelease))
        readFile(etcOsRelease);
    else if (QFile::exists(usrLibOsRelease))
        readFile(usrLibOsRelease);
}

void OsReleasePrivate::readFile(const QString &fileName)
{
    Q_Q(OsRelease);

    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
        return;

    name.clear();
    version.clear();
    id.clear();
    idLike.clear();
    versionCodeName.clear();
    versionId.clear();
    prettyName.clear();
    ansiColor.clear();
    cpeName.clear();
    homeUrl.clear();
    docUrl.clear();
    supportUrl.clear();
    bugReportUrl.clear();
    privacyPolicyUrl.clear();
    buildId.clear();
    variant.clear();
    variantId.clear();
    logo.clear();

    const QRegularExpression re(QStringLiteral("^(.+)=(.+)$"));
    const QRegularExpression firstQuoteRe(QStringLiteral("^['\"]"));
    const QRegularExpression lastQuoteRe(QStringLiteral("['\"]$"));

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip comments
        if (line.startsWith(QLatin1Char('#')))
            continue;

        QRegularExpressionMatch match = re.match(line);
        if (!match.hasMatch())
            continue;

        const QString varName = match.captured(1);
        const QString value = match.captured(2).replace(firstQuoteRe, QString()).replace(lastQuoteRe, QString());

        if (varName == QStringLiteral("NAME"))
            name = value;
        else if (varName == QStringLiteral("VERSION"))
            version = value;
        else if (varName == QStringLiteral("ID"))
            id = value;
        else if (varName == QStringLiteral("ID_LIKE"))
            idLike = value.split(QLatin1Char(' '));
        else if (varName == QStringLiteral("VERSION_CODENAME"))
            versionCodeName = value;
        else if (varName == QStringLiteral("VERSION_ID"))
            versionId = value;
        else if (varName == QStringLiteral("PRETTY_NAME"))
            prettyName = value;
        else if (varName == QStringLiteral("ANSI_COLOR"))
            ansiColor = value;
        else if (varName == QStringLiteral("CPE_NAME"))
            cpeName = value;
        else if (varName == QStringLiteral("HOME_URL"))
            homeUrl = QUrl::fromUserInput(value);
        else if (varName == QStringLiteral("DOCUMENTATION_URL"))
            docUrl = QUrl::fromUserInput(value);
        else if (varName == QStringLiteral("SUPPORT_URL"))
            supportUrl = QUrl::fromUserInput(value);
        else if (varName == QStringLiteral("BUG_REPORT_URL"))
            bugReportUrl = QUrl::fromUserInput(value);
        else if (varName == QStringLiteral("PRIVACY_POLICY_URL"))
            privacyPolicyUrl = QUrl::fromUserInput(value);
        else if (varName == QStringLiteral("BUILD_ID"))
            buildId = value;
        else if (varName == QStringLiteral("VARIANT"))
            variant = value;
        else if (varName == QStringLiteral("VARIANT_ID"))
            variantId = value;
        else if (varName == QStringLiteral("LOGO"))
            logo = value;
    }

    file.close();

    resetDefaults();

    Q_EMIT q->nameChanged();
    Q_EMIT q->versionChanged();
    Q_EMIT q->idChanged();
    Q_EMIT q->idLikeChanged();
    Q_EMIT q->versionCodeNameChanged();
    Q_EMIT q->versionIdChanged();
    Q_EMIT q->prettyNameChanged();
    Q_EMIT q->ansiColorChanged();
    Q_EMIT q->cpeNameChanged();
    Q_EMIT q->homeUrlChanged();
    Q_EMIT q->documentationUrlChanged();
    Q_EMIT q->supportUrlChanged();
    Q_EMIT q->bugReportUrlChanged();
    Q_EMIT q->privacyPolicyUrlChanged();
    Q_EMIT q->buildIdChanged();
    Q_EMIT q->variantChanged();
    Q_EMIT q->variantIdChanged();
    Q_EMIT q->logoIconNameChanged();
}

void OsReleasePrivate::resetDefaults()
{
    if (name.isEmpty())
        name = QStringLiteral("Linux");
    if (id.isEmpty())
        id = QStringLiteral("linux");
    if (prettyName.isEmpty())
        prettyName = QStringLiteral("Linux");
}

/*!
 * \qmltype OsRelease
 * \inqmlmodule Liri.Device
 * \brief Provides operating system identification.
 *
 * OsRelease provides operating system information, such as name, version, edition
 * and so on.
 *
 * \code
 * import Liri.Device 1.0 as LiriDevice
 *
 * Text {
 *     text: LiriDevice.LocalDevice.osRelease.prettyName || LiriDevice.LocalDevice.osRelease.name
 * }
 * \endcode
 */

/*!
 * \class OsRelease
 * \inmodule LiriLocalDevice
 * \brief Provides operating system identification.
 *
 * OsRelease provides operating system information, such as name, version, edition
 * and so on.
 */

/*!
 * Constructs an OsRelease with the given \a parent.
 */
OsRelease::OsRelease(QObject *parent)
    : QObject(parent)
    , d_ptr(new OsReleasePrivate(this))
{
    d_func()->readOsRelease();
}

/*!
 * Destroys an OsRelease object.
 */
OsRelease::~OsRelease()
{
    delete d_ptr;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::name
 *
 * This property holds the operating system name.
 *
 * * This is suitable for presentation to the user.
 */

/*!
 * \property OsRelease::name
 *
 * This property holds the operating system name.
 *
 * * This is suitable for presentation to the user.
 */
QString OsRelease::name() const
{
    Q_D(const OsRelease);
    return d->name;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::version
 *
 * This property holds the operating system version.
 *
 * This is suitable for presentation to the user.
 */

/*!
 * \property OsRelease::version
 *
 * This property holds the operating system version.
 *
 * * This is suitable for presentation to the user.
 */
QString OsRelease::version() const
{
    Q_D(const OsRelease);
    return d->version;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::id
 *
 * This property holds a lower-case string with no spaces or other characters
 * outside of 0-9, a-z, ".", "_" and "-" identifying the operating system.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */

/*!
 * \property OsRelease::id
 *
 * This property holds a lower-case string with no spaces or other characters
 * outside of 0-9, a-z, ".", "_" and "-" identifying the operating system.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */
QString OsRelease::id() const
{
    Q_D(const OsRelease);
    return d->id;
}

/*!
 * \qmlproperty list<string> Liri::LocalDevice::LocalDevice::osRelease::idLike
 *
 * This property holds a list of identifiers of operating systems that are
 * closely related to the local operating system in regards to packaging
 * and programming interfaces.
 *
 * \sa OsRelease::id
 */

/*!
 * \property OsRelease::idLike
 *
 * This property holds a list of identifiers of operating systems that are
 * closely related to the local operating system in regards to packaging
 * and programming interfaces.
 *
 * \sa OsRelease::id
 */
QStringList OsRelease::idLike() const
{
    Q_D(const OsRelease);
    return d->idLike;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::versionCodeName
 *
 * This property holds a lower-case string with no spaces or other characters
 * outside of 0-9, a-z, ".", "_" and "-" identifying the operating system
 * release code name, excluding any OS name information or release version.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */

/*!
 * \property OsRelease::versionCodeName
 *
 * This property holds a lower-case string with no spaces or other characters
 * outside of 0-9, a-z, ".", "_" and "-" identifying the operating system
 * release code name, excluding any OS name information or release version.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */
QString OsRelease::versionCodeName() const
{
    Q_D(const OsRelease);
    return d->versionCodeName;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::versionId
 *
 * This property holds a lower-case string, mostly numeric, no spaces or
 * other characters outside of 0-9, a-z, ".", "_" and "-", identifying the
 * operating system version, excluding any OS name information or release
 * code name.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */

/*!
 * \property OsRelease::versionId
 *
 * This property holds a lower-case string, mostly numeric, no spaces or
 * other characters outside of 0-9, a-z, ".", "_" and "-", identifying the
 * operating system version, excluding any OS name information or release
 * code name.
 *
 * This is suitable for processing by scripts or usage in generate filenames.
 */
QString OsRelease::versionId() const
{
    Q_D(const OsRelease);
    return d->versionId;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::prettyName
 *
 * This property holds a pretty operating system name in a format suitable
 * for presentation to the user.  May or may not contain a release coee name
 * or OS version of some kind, as suitable.
 *
 * Defaults to "Linux".
 */

/*!
 * \property OsRelease::prettyName
 *
 * This property holds a pretty operating system name in a format suitable
 * for presentation to the user.  May or may not contain a release coee name
 * or OS version of some kind, as suitable.
 *
 * Defaults to "Linux".
 */
QString OsRelease::prettyName() const
{
    Q_D(const OsRelease);
    return d->prettyName;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::ansiColor
 *
 * This property holds a suggested presentation color when showing the OS name
 * on the console.
 */

/*!
 * \property OsRelease::ansiColor
 *
 * This property holds a suggested presentation color when showing the OS name
 * on the console.
 */
QString OsRelease::ansiColor() const
{
    Q_D(const OsRelease);
    return d->ansiColor;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::cpeName
 *
 * This property holds a CPE name for the operating system.
 */

/*!
 * \property OsRelease::cpeName
 *
 * This property holds a CPE name for the operating system.
 */
QString OsRelease::cpeName() const
{
    Q_D(const OsRelease);
    return d->cpeName;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::homeUrl
 *
 * This property holds a link to the homepage of the operating system.
 */

/*!
 * \property OsRelease::homeUrl
 *
 * This property holds a link to the homepage of the operating system.
 */
QUrl OsRelease::homeUrl() const
{
    Q_D(const OsRelease);
    return d->homeUrl;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::documentationUrl
 *
 * This property holds a link to the documentation of the operating system.
 */

/*!
 * \property OsRelease::documentationUrl
 *
 * This property holds a link to the documentation of the operating system.
 */
QUrl OsRelease::documentationUrl() const
{
    Q_D(const OsRelease);
    return d->docUrl;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::supportUrl
 *
 * This property holds a link to the main support page of the operating system.
 */

/*!
 * \property OsRelease::supportUrl
 *
 * This property holds a link to the main support page of the operating system.
 */
QUrl OsRelease::supportUrl() const
{
    Q_D(const OsRelease);
    return d->supportUrl;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::bugReportUrl
 *
 * This property holds a link to the main bug reporting page of the operating system.
 */

/*!
 * \property OsRelease::bugReportUrl
 *
 * This property holds a link to the main bug reporting page of the operating system.
 */
QUrl OsRelease::bugReportUrl() const
{
    Q_D(const OsRelease);
    return d->bugReportUrl;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::privacyPolicyUrl
 *
 * This property holds a link to the main privacy policy page of the operating system.
 */

/*!
 * \property OsRelease::privacyPolicyUrl
 *
 * This property holds a link to the main privacy policy page of the operating system.
 */
QUrl OsRelease::privacyPolicyUrl() const
{
    Q_D(const OsRelease);
    return d->privacyPolicyUrl;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::buildId
 *
 * This property holds a string uniquely identifying to the system image used as the
 * origin for a distribution (it is not updated with system updates).
 */

/*!
 * \property OsRelease::buildId
 *
 * This property holds a string uniquely identifying to the system image used as the
 * origin for a distribution (it is not updated with system updates).
 */
QString OsRelease::buildId() const
{
    Q_D(const OsRelease);
    return d->buildId;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::variant
 *
 * This property holds a string identifying a specific variant or edition of
 * the operating system, suitable for presentation to the user.
 */

/*!
 * \property OsRelease::variant
 *
 * This property holds a string identifying a specific variant or edition of
 * the operating system, suitable for presentation to the user.
 */
QString OsRelease::variant() const
{
    Q_D(const OsRelease);
    return d->variant;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::variantId
 *
 * This property holds a lower-case string (no spaces or other characters
 * outside of 0–9, a–z, ".", "_" and "-"), identifying a specific variant
 * or edition of the operating system.
 */

/*!
 * \property OsRelease::variantId
 *
 * This property holds a lower-case string (no spaces or other characters
 * outside of 0–9, a–z, ".", "_" and "-"), identifying a specific variant
 * or edition of the operating system.
 */
QString OsRelease::variantId() const
{
    Q_D(const OsRelease);
    return d->variantId;
}

/*!
 * \qmlproperty string Liri::LocalDevice::LocalDevice::osRelease::logoIconName
 *
 * This property holds a string specifying the name of an icon as defined by the
 * \l {http://standards.freedesktop.org/icon-theme-spec/latest/}
 * {freedesktop.org Icon Theme} specification.
 */

/*!
 * \property OsRelease::logoIconName
 *
 * This property holds a string specifying the name of an icon as defined by the
 * \l {http://standards.freedesktop.org/icon-theme-spec/latest/}
 * {freedesktop.org Icon Theme} specification.
 */
QString OsRelease::logoIconName() const
{
    Q_D(const OsRelease);
    return d->logo;
}

} // namespace Liri
