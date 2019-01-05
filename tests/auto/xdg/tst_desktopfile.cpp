/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2016 Luís Pereira <luis.artur.pereira@gmail.com>
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

#include <QObject>
#include <QtTest>

#include <LiriXdg/DesktopFile>

class Language
{
public:
    Language(const QString &lang)
        : mPreviousLang(QString::fromLocal8Bit(qgetenv("LC_MESSAGES")))
    {
        qputenv("LC_MESSAGES", lang.toLocal8Bit());
    }

    ~Language()
    {
        qputenv("LC_MESSAGES", mPreviousLang.toLocal8Bit());
    }

private:
    QString mPreviousLang;
};

class TestDesktopFile : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRead()
    {
        QTemporaryFile file("testReadXXXXXX.desktop");
        QVERIFY(file.open());
        const QString fileName = file.fileName();
        QTextStream ts(&file);
        ts << "[Desktop Entry]\n"
              "Type=Application\n"
              "Name=MyApp\n"
              "Icon=foobar\n"
              "MimeType=text/plain;image/png;;\n"
              "\n";
        file.close();
        QVERIFY(QFile::exists(fileName));

        Liri::DesktopFile df;
        df.load(fileName);

        QVERIFY(df.isValid());
        QCOMPARE(df.type(), Liri::DesktopFile::ApplicationType);
        QCOMPARE(df.name(), QStringLiteral("MyApp"));
        QCOMPARE(df.iconName(), QStringLiteral("foobar"));
        QCOMPARE(df.mimeTypes(), QStringList() << QStringLiteral("text/plain") << QStringLiteral("image/png"));
        QCOMPARE(df.fileName(), QFileInfo(fileName).canonicalFilePath());
    }

    void testReadLocalized_data()
    {
        QTest::addColumn<QString>("locale");
        QTest::addColumn<QString>("translation");

        const QString pt = QString::fromUtf8("A Minha Aplicação");
        const QString pt_BR = QString::fromUtf8("O Meu Aplicativo");

        QTest::newRow("C") << QStringLiteral("C") << QStringLiteral("My Application");
        QTest::newRow("pt") << QStringLiteral("pt") << pt;
        QTest::newRow("pt_PT") << QStringLiteral("pt_PT") << pt;
        QTest::newRow("pt_BR") << QStringLiteral("pt_BR") << pt_BR;
    }

    void testReadLocalized()
    {
        QTemporaryFile file("testReadLocalizedXXXXXX.desktop");
        QVERIFY(file.open());
        const QString fileName = file.fileName();
        QTextStream ts(&file);
        ts << QString::fromUtf8(
                "[Desktop Entry]\n"
                "Type=Application\n"
                "Name=My Application\n"
                "Name[pt]=A Minha Aplicação\n"
                "Name[pt_BR]=O Meu Aplicativo\n"
                "Icon=foo\n"
                "\n");
        file.close();

        QVERIFY(QFile::exists(fileName));

        Liri::DesktopFile df;
        df.load(fileName);
        QVERIFY(df.isValid());

        QFETCH(QString, locale);
        QFETCH(QString, translation);

        Language lang(locale);

        QCOMPARE(df.name(), translation);
    }
};

QTEST_MAIN(TestDesktopFile)

#include "tst_desktopfile.moc"
