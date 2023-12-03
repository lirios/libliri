// Copyright (C) 2010-2011 Razor team
// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
