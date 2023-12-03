// SPDX-FileCopyrightText: 2010-2011 Razor team
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "xmlhelper_p_p.h"

#include <QDebug>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

QDebug operator<<(QDebug dbg, const QDomElement &el)
{
    QDomNamedNodeMap map = el.attributes();

    QString args;
    for (int i = 0; i < map.count(); ++i)
        args += QLatin1Char(' ') + map.item(i).nodeName() + QLatin1Char('=') + map.item(i).nodeValue() + QLatin1Char('\'');

    dbg.nospace() << QStringLiteral("<%1%2>%3</%1>").arg(el.tagName(), args, el.text());
    return dbg.space();
}
