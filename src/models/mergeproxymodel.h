/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef LIRI_MERGEPROXYMODEL_H
#define LIRI_MERGEPROXYMODEL_H

#include <QAbstractItemModel>
#include <QScopedPointer>

#include <LiriModels/lirimodelsglobal.h>

namespace Liri {

class MergeProxyModelPrivate;

class LIRIMODELS_EXPORT MergeProxyModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MergeProxyModel)
public:
    explicit MergeProxyModel(QObject *parent = nullptr);
    virtual ~MergeProxyModel();

    Q_SCRIPTABLE void addSourceModel(QAbstractItemModel *sourceModel);
    Q_SCRIPTABLE void removeSourceModel(QAbstractItemModel *sourceModel);

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

    QMap<int, QVariant> itemData(const QModelIndex &proxyIndex) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    MergeProxyModelPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_ptr, void slotRowsAboutToBeInserted(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotRowsInserted(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotRowsRemoved(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotColumnsInserted(const QModelIndex &parent, int, int))
    Q_PRIVATE_SLOT(d_ptr, void slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_ptr, void slotColumnsRemoved(const QModelIndex &parent, int, int))
    Q_PRIVATE_SLOT(d_ptr, void slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles))
    Q_PRIVATE_SLOT(d_ptr, void slotSourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint))
    Q_PRIVATE_SLOT(d_ptr, void slotSourceLayoutChanged(const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint))
    Q_PRIVATE_SLOT(d_ptr, void slotModelAboutToBeReset())
    Q_PRIVATE_SLOT(d_ptr, void slotModelReset())
};

} // namespace Liri

#endif // LIRI_MERGEPROXYMODEL_H
