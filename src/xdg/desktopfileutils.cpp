// Copyright (C) 2010-2011 Razor team
// Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QUrl>

#include "desktopfileutils_p.h"
#include "xdgdirs_p_p.h"

QString &doEscape(QString &str, const QHash<QChar, QChar> &repl)
{
    // First we replace slash.
    str.replace(QLatin1Char('\\'), QLatin1String("\\\\"));

    QHash<QChar, QChar>::const_iterator i = repl.constBegin();
    while (i != repl.constEnd()) {
        if (i.key() != QLatin1Char('\\'))
            str.replace(i.key(), QStringLiteral("\\\\%1").arg(i.value()));

        ++i;
    }

    return str;
}

/************************************************
 The escape sequences \s, \n, \t, \r, and \\ are supported for values
 of type string and localestring, meaning ASCII space, newline, tab,
 carriage return, and backslash, respectively.
 ************************************************/
QString &escape(QString &str)
{
    QHash<QChar, QChar> repl;
    repl.insert(QLatin1Char('\n'), QLatin1Char('n'));
    repl.insert(QLatin1Char('\t'), QLatin1Char('t'));
    repl.insert(QLatin1Char('\r'), QLatin1Char('r'));

    return doEscape(str, repl);
}

/************************************************
 Quoting must be done by enclosing the argument between double quotes and
 escaping the
    double quote character,
    backtick character ("`"),
    dollar sign ("$") and
    backslash character ("\")
by preceding it with an additional backslash character.
Implementations must undo quoting before expanding field codes and before
passing the argument to the executable program.

Note that the general escape rule for values of type string states that the
backslash character can be escaped as ("\\") as well and that this escape
rule is applied before the quoting rule. As such, to unambiguously represent a
literal backslash character in a quoted argument in a desktop entry file
requires the use of four successive backslash characters ("\\\\").
Likewise, a literal dollar sign in a quoted argument in a desktop entry file
is unambiguously represented with ("\\$").
 ************************************************/
QString &escapeExec(QString &str)
{
    QHash<QChar, QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert(QLatin1Char('"'), QLatin1Char('"')); // double quote,
    repl.insert(QLatin1Char('\''), QLatin1Char('\'')); // single quote ("'"),
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\')); // backslash character ("\"),
    repl.insert(QLatin1Char('$'), QLatin1Char('$')); // dollar sign ("$"),

    return doEscape(str, repl);
}

QString &doUnEscape(QString &str, const QHash<QChar, QChar> &repl)
{
    int n = 0;
    while (1) {
        n = str.indexOf(QLatin1String("\\"), n);
        if (n < 0 || n > str.length() - 2)
            break;

        if (repl.contains(str.at(n + 1))) {
            str.replace(n, 2, repl.value(str.at(n + 1)));
        }

        n++;
    }

    return str;
}

/************************************************
 The escape sequences \s, \n, \t, \r, and \\ are supported for values
 of type string and localestring, meaning ASCII space, newline, tab,
 carriage return, and backslash, respectively.
 ************************************************/
QString &unEscape(QString &str)
{
    QHash<QChar, QChar> repl;
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\'));
    repl.insert(QLatin1Char('s'), QLatin1Char(' '));
    repl.insert(QLatin1Char('n'), QLatin1Char('\n'));
    repl.insert(QLatin1Char('t'), QLatin1Char('\t'));
    repl.insert(QLatin1Char('r'), QLatin1Char('\r'));

    return doUnEscape(str, repl);
}

/************************************************
 Quoting must be done by enclosing the argument between double quotes and
 escaping the
    double quote character,
    backtick character ("`"),
    dollar sign ("$") and
    backslash character ("\")
by preceding it with an additional backslash character.
Implementations must undo quoting before expanding field codes and before
passing the argument to the executable program.

Reserved characters are
    space (" "),
    tab,
    newline,
    double quote,
    single quote ("'"),
    backslash character ("\"),
    greater-than sign (">"),
    less-than sign ("<"),
    tilde ("~"),
    vertical bar ("|"),
    ampersand ("&"),
    semicolon (";"),
    dollar sign ("$"),
    asterisk ("*"),
    question mark ("?"),
    hash mark ("#"),
    parenthesis ("(") and (")")
    backtick character ("`").

Note that the general escape rule for values of type string states that the
backslash character can be escaped as ("\\") as well and that this escape
rule is applied before the quoting rule. As such, to unambiguously represent a
literal backslash character in a quoted argument in a desktop entry file
requires the use of four successive backslash characters ("\\\\").
Likewise, a literal dollar sign in a quoted argument in a desktop entry file
is unambiguously represented with ("\\$").
 ************************************************/
QString &unEscapeExec(QString &str)
{
    unEscape(str);
    QHash<QChar, QChar> repl;
    // The parseCombinedArgString() splits the string by the space symbols,
    // we temporarily replace them on the special characters.
    // Replacement will reverse after the splitting.
    repl.insert(QLatin1Char(' '), QChar(01)); // space
    repl.insert(QLatin1Char('\t'), QChar(02)); // tab
    repl.insert(QLatin1Char('\n'), QChar(03)); // newline,

    repl.insert(QLatin1Char('"'), QLatin1Char('"')); // double quote,
    repl.insert(QLatin1Char('\''), QLatin1Char('\'')); // single quote ("'"),
    repl.insert(QLatin1Char('\\'), QLatin1Char('\\')); // backslash character ("\"),
    repl.insert(QLatin1Char('>'), QLatin1Char('>')); // greater-than sign (">"),
    repl.insert(QLatin1Char('<'), QLatin1Char('<')); // less-than sign ("<"),
    repl.insert(QLatin1Char('~'), QLatin1Char('~')); // tilde ("~"),
    repl.insert(QLatin1Char('|'), QLatin1Char('|')); // vertical bar ("|"),
    repl.insert(QLatin1Char('&'), QLatin1Char('&')); // ampersand ("&"),
    repl.insert(QLatin1Char(';'), QLatin1Char(';')); // semicolon (";"),
    repl.insert(QLatin1Char('$'), QLatin1Char('$')); // dollar sign ("$"),
    repl.insert(QLatin1Char('*'), QLatin1Char('*')); // asterisk ("*"),
    repl.insert(QLatin1Char('?'), QLatin1Char('?')); // question mark ("?"),
    repl.insert(QLatin1Char('#'), QLatin1Char('#')); // hash mark ("#"),
    repl.insert(QLatin1Char('('), QLatin1Char('(')); // parenthesis ("(")
    repl.insert(QLatin1Char(')'), QLatin1Char(')')); // parenthesis (")")
    repl.insert(QLatin1Char('`'), QLatin1Char('`')); // backtick character ("`").

    return doUnEscape(str, repl);
}

QString findDesktopFile(const QString &dirName, const QString &desktopName)
{
    QDir dir(dirName);
    QFileInfo fi(dir, desktopName);

    if (fi.exists())
        return fi.canonicalFilePath();

    const QFileInfoList dirs = dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &d : dirs) {
        QString cn = d.canonicalFilePath();
        if (dirName != cn) {
            QString f = findDesktopFile(cn, desktopName);
            if (!f.isEmpty())
                return f;
        }
    }

    return QString();
}

QString findDesktopFile(const QString &desktopName)
{
    QStringList dataDirs = XdgDirs::dataDirs();
    dataDirs.prepend(XdgDirs::dataHome(false));

    for (const QString &dirName : const_cast<const QStringList &>(dataDirs)) {
        QString f = findDesktopFile(dirName + QStringLiteral("/applications"), desktopName);
        if (!f.isEmpty())
            return f;
    }

    return QString();
}

QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i) {
        if (program.at(i) == QLatin1Char('"')) {
            ++quoteCount;
            if (quoteCount == 3) {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount) {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace()) {
            if (!tmp.isEmpty()) {
                args += tmp;
                tmp.clear();
            }
        } else {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

QString expandDynamicUrl(QString url)
{
    const QStringList env = QProcess::systemEnvironment();
    for (const QString &line : env) {
        QString name = line.section(QLatin1Char('='), 0, 0);
        QString val = line.section(QLatin1Char('='), 1);
        url.replace(QStringLiteral("$%1").arg(name), val);
        url.replace(QStringLiteral("${%1}").arg(name), val);
    }

    return url;
}

void replaceVar(QString &str, const QString &varName, const QString &after)
{
    str.replace(QRegularExpression(QStringLiteral("\\$%1(?!\\w)").arg(varName)), after);
    str.replace(QRegularExpression(QStringLiteral("\\$\\{%1\\}").arg(varName)), after);
}

QString expandEnvVariables(const QString &str)
{
    QString scheme = QUrl(str).scheme();

    if (   scheme == QLatin1String("http")   || scheme == QLatin1String("https")
        || scheme == QLatin1String("shttp")  || scheme == QLatin1String("ftp")
        || scheme == QLatin1String("ftps")   || scheme == QLatin1String("pop")
        || scheme == QLatin1String("pops")   || scheme == QLatin1String("imap")
        || scheme == QLatin1String("imaps")  || scheme == QLatin1String("mailto")
        || scheme == QLatin1String("nntp")   || scheme == QLatin1String("irc")
        || scheme == QLatin1String("telnet") || scheme == QLatin1String("xmpp")
        || scheme == QLatin1String("nfs"))
        return str;

    const QString homeDir = QFile::decodeName(qgetenv("HOME"));

    QString res = str;
    res.replace(QRegularExpression(QStringLiteral("~(?=$|/)")), homeDir);

    replaceVar(res, QStringLiteral("HOME"), homeDir);
    replaceVar(res, QStringLiteral("USER"), QString::fromLocal8Bit(qgetenv("USER")));

    replaceVar(res, QStringLiteral("XDG_DESKTOP_DIR"), XdgDirs::userDir(XdgDirs::Desktop));
    replaceVar(res, QStringLiteral("XDG_TEMPLATES_DIR"), XdgDirs::userDir(XdgDirs::Templates));
    replaceVar(res, QStringLiteral("XDG_DOCUMENTS_DIR"), XdgDirs::userDir(XdgDirs::Documents));
    replaceVar(res, QStringLiteral("XDG_MUSIC_DIR"), XdgDirs::userDir(XdgDirs::Music));
    replaceVar(res, QStringLiteral("XDG_PICTURES_DIR"), XdgDirs::userDir(XdgDirs::Pictures));
    replaceVar(res, QStringLiteral("XDG_VIDEOS_DIR"), XdgDirs::userDir(XdgDirs::Videos));
    replaceVar(res, QStringLiteral("XDG_PHOTOS_DIR"), XdgDirs::userDir(XdgDirs::Pictures));

    return res;
}

QStringList expandEnvVariables(const QStringList &strs)
{
    QStringList res;
    for (const QString &s : strs)
        res << expandEnvVariables(s);

    return res;
}
