// clang-format off

// Copyright (C) 2008 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Michael Pyne <mpyne@kde.org>
// Copyright (C) 2010 Michael Leupold <lemma@confuego.org>
// Copyright (C) 2013 Alex Merry <alex.merry@kdemail.net>
// Copyright (C) 2013 John Layt <jlayt@kde.org>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "kformatprivate_p.h"

KFormat::KFormat(const QLocale &locale)
    : d(new KFormatPrivate(locale))
{
}

KFormat::KFormat(const KFormat &other)
    : d(other.d)
{
}

KFormat& KFormat::operator=(const KFormat &other)
{
    d = other.d;
    return *this;
}

KFormat::~KFormat()
{
}

QString KFormat::formatByteSize(double size,
                                int precision,
                                KFormat::BinaryUnitDialect dialect,
                                KFormat::BinarySizeUnits units) const
{
    return d->formatByteSize(size, precision, dialect, units);
}

QString KFormat::formatDuration(quint64 msecs,
                                KFormat::DurationFormatOptions options) const
{
    return d->formatDuration(msecs, options);
}

QString KFormat::formatDecimalDuration(quint64 msecs,
                                       int decimalPlaces) const
{
    return d->formatDecimalDuration(msecs, decimalPlaces);
}

QString KFormat::formatSpelloutDuration(quint64 msecs) const
{
    return d->formatSpelloutDuration(msecs);
}

QString KFormat::formatRelativeDate(const QDate &date,
                                    QLocale::FormatType format) const
{
    return d->formatRelativeDate(date, format);
}

QString KFormat::formatRelativeDateTime(const QDateTime &dateTime,
                                        QLocale::FormatType format) const
{
    return d->formatRelativeDateTime(dateTime, format);
}

#include "moc_kformat.cpp"
