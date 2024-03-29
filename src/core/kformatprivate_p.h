// clang-format off

// Copyright (C) 2008 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2009 Michael Pyne <mpyne@kde.org>
// Copyright (C) 2010 Michael Leupold <lemma@confuego.org>
// Copyright (C) 2013 Alex Merry <alex.merry@kdemail.net>
// Copyright (C) 2013 John Layt <jlayt@kde.org>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef KFORMATPRIVATE_P_H
#define KFORMATPRIVATE_P_H

#include "kformat.h"

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS

class KFormatPrivate : public QSharedData
{
    Q_DECLARE_TR_FUNCTIONS(KFormat)

public:

    explicit KFormatPrivate(const QLocale &locale);
    virtual ~KFormatPrivate();

    QString formatByteSize(double size,
                           int precision,
                           KFormat::BinaryUnitDialect dialect,
                           KFormat::BinarySizeUnits units) const;

    QString formatDuration(quint64 msecs,
                           KFormat::DurationFormatOptions options) const;

    QString formatDecimalDuration(quint64 msecs,
                                  int decimalPlaces) const;

    QString formatSpelloutDuration(quint64 msecs) const;

    QString formatRelativeDate(const QDate &date,
                               QLocale::FormatType format) const;

    QString formatRelativeDateTime(const QDateTime &dateTime,
                                   QLocale::FormatType format) const;

private:

    QLocale m_locale;
};

#endif // KFORMATPRIVATE_P_H
