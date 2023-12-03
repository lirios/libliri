// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OSRELEASE_P_H
#define OSRELEASE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "osrelease.h"

namespace Liri {

class OsReleasePrivate
{
    Q_DECLARE_PUBLIC(OsRelease)
public:
    explicit OsReleasePrivate(OsRelease *self);

    QString name;
    QString version;
    QString id;
    QStringList idLike;
    QString versionCodeName;
    QString versionId;
    QString prettyName;
    QString ansiColor;
    QString cpeName;
    QUrl homeUrl;
    QUrl docUrl;
    QUrl supportUrl;
    QUrl bugReportUrl;
    QUrl privacyPolicyUrl;
    QString buildId;
    QString variant;
    QString variantId;
    QString logo;

    void readOsRelease();
    void readFile(const QString &fileName);
    void resetDefaults();

protected:
    OsRelease *q_ptr = nullptr;
};

} // namespace Liri


#endif // OSRELEASE_P_H
