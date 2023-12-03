// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

#include <LiriNotifications/Notification>

using namespace Liri;

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Application
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Notify"));
    app.setApplicationVersion(QStringLiteral(LIBLIRI_VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Notifications client"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Summary
    QCommandLineOption summary(QStringList() << QStringLiteral("summary") << QStringLiteral("s"),
                               TR("Summary"), TR("summary"));
    parser.addOption(summary);

    // Body
    QCommandLineOption body(QStringList() << QStringLiteral("body") << QStringLiteral("b"),
                            TR("Body text"), TR("body"));
    parser.addOption(body);

    // Icon
    QCommandLineOption icon(QStringList() << QStringLiteral("icon") << QStringLiteral("i"),
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
