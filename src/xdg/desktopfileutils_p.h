/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2010-2011 Razor team
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

#ifndef DESKTOPFILEUTILS_P_H
#define DESKTOPFILEUTILS_P_H

#include <QString>

QString &doEscape(QString &str, const QHash<QChar, QChar> &repl);
QString &escape(QString &str);
QString &escapeExec(QString &str);
QString &doUnEscape(QString &str, const QHash<QChar, QChar> &repl);
QString &unEscape(QString &str);
QString &unEscapeExec(QString &str);
QString findDesktopFile(const QString &dirName, const QString &desktopName);
QString findDesktopFile(const QString &desktopName);

QStringList parseCombinedArgString(const QString &program);

QString expandDynamicUrl(QString url);

void replaceVar(QString &str, const QString &varName, const QString &after);
QString expandEnvVariables(const QString &str);
QStringList expandEnvVariables(const QStringList &strs);

#endif // DESKTOPFILEUTILS_P_H
