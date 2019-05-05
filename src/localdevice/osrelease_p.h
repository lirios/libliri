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
