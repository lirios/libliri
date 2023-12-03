// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtTest/QtTest>

#include <LiriModels/QObjectListModel>

class CustomObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString string MEMBER m_string CONSTANT)
    Q_PROPERTY(int integer MEMBER m_integer CONSTANT)
    Q_PROPERTY(bool boolean MEMBER m_boolean CONSTANT)
public:
    CustomObject(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    QString m_string;
    int m_integer;
    bool m_boolean;
};


class TestQObjectListModel : public QObject
{
    Q_OBJECT
public:
    TestQObjectListModel(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:
    void initTestCase()
    {
        int i = 0;
        while (i < 5) {
            CustomObject *o = new CustomObject(this);
            o->m_string = QStringLiteral("item%1").arg(i + 1);
            o->m_integer = 42 + i;
            o->m_boolean = true;
            m_list.append(o);
            i++;
        }
    }

    void creation()
    {
        QObjectListModel *model = QObjectListModel::create(m_list, this);
        QVERIFY(model);

        QCOMPARE(model->rowCount(QModelIndex()), m_list.count());

        for (int i = 0; i < 5; i++) {
            QModelIndex index = model->index(i, 0, QModelIndex());
            auto object = model->data(index, Qt::DisplayRole).value<CustomObject *>();
            QCOMPARE(object->m_string, QStringLiteral("item%1").arg(i + 1));
            QCOMPARE(object->m_integer, 42 + i);
            QCOMPARE(object->m_boolean, true);
        }

        delete model;
    }

private:
    QList<CustomObject *> m_list;
};

QTEST_MAIN(TestQObjectListModel)

#include "tst_qobjectlistmodel.moc"
