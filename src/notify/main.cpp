/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include <LiriNotifications/Notification>

using namespace Liri;

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QLatin1String(x))

int main(int argc, char *argv[])
{
    // Application
    QCoreApplication app(argc, argv);
    app.setApplicationName(QLatin1String("Notify"));
    app.setApplicationVersion(QLatin1String(LIBLIRI_VERSION));
    app.setOrganizationName(QLatin1String("Liri"));
    app.setOrganizationDomain(QLatin1String("liri.io"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Notifications client"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Summary
    QCommandLineOption summary(QStringList() << QLatin1String("summary") << QLatin1String("s"),
                               TR("Summary"), TR("summary"));
    parser.addOption(summary);

    // Body
    QCommandLineOption body(QStringList() << QLatin1String("body") << QLatin1String("b"),
                            TR("Body text"), TR("body"));
    parser.addOption(body);

    // Icon
    QCommandLineOption icon(QStringList() << QLatin1String("icon") << QLatin1String("i"),
                            TR("Application icon"), TR("icon"));
    parser.addOption(icon);

    // Parse command line
    parser.process(app);

    // At least the summary must be set
    if (!parser.isSet(summary)) {
        qCritical("Please specify a summary");
        return 1;
    }

    // Notification
    Notification *n = new Notification();
    QObject::connect(n, &Notification::sendSucceeded,
                     &app, &QCoreApplication::quit);
    QObject::connect(n, &Notification::sendFailed,
                     &app, &QCoreApplication::quit);
    n->setSummary(parser.value(summary));
    if (parser.isSet(body))
        n->setBody(parser.value(body));
    if (parser.isSet(icon))
        n->setApplicationIcon(parser.value(icon));
    n->send();

    return app.exec();
}
