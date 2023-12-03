// Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "formatter.h"

Formatter::Formatter(QObject *parent)
    : QObject(parent)
{
}

QString Formatter::formatByteSize(double size, int precision, BinaryUnitDialect dialect,
                                  BinarySizeUnits units) const
{
    return formatter.formatByteSize(size, precision,
                                    static_cast<KFormat::BinaryUnitDialect>(dialect),
                                    static_cast<KFormat::BinarySizeUnits>(units));
}

QString Formatter::formatDuration(quint64 msecs, DurationFormatOptions options) const
{
    int tempOptions = int(options);
    return formatter.formatDuration(msecs, QFlags<KFormat::DurationFormatOption>(tempOptions));
}

QString Formatter::formatDecimalDuration(quint64 msecs, int decimalPlaces) const
{
    return formatter.formatDecimalDuration(msecs, decimalPlaces);
}

QString Formatter::formatSpelloutDuration(quint64 msecs) const
{
    return formatter.formatSpelloutDuration(msecs);
}

QString Formatter::formatRelativeDate(const QDate &date, QLocale::FormatType format) const
{
    return formatter.formatRelativeDate(date, format);
}

QString Formatter::formatRelativeDateTime(const QDateTime &dateTime,
                                          QLocale::FormatType format) const
{
    return formatter.formatRelativeDateTime(dateTime, format);
}

Formatter *Formatter::create(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(jsEngine)

    return new Formatter();
}
