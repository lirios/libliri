// Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_MERGEPROXYMODEL_P_H
#define LIRI_MERGEPROXYMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Liri API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QAbstractItemModel>

namespace Liri {

class MergeProxyModel;

class MergeProxyModelPrivate
{
    Q_DECLARE_PUBLIC(MergeProxyModel)
public:
    explicit MergeProxyModelPrivate(MergeProxyModel* model);

    int computeRowsPrior(const QAbstractItemModel *sourceModel) const;
    QAbstractItemModel *sourceModelForRow(int row, int *sourceRow) const;

    void slotRowsAboutToBeInserted(const QModelIndex &, int start, int end);
    void slotRowsInserted(const QModelIndex &, int start, int end);
    void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end);
    void slotRowsRemoved(const QModelIndex &, int start, int end);
    void slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void slotColumnsInserted(const QModelIndex &parent, int, int);
    void slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void slotColumnsRemoved(const QModelIndex &parent, int, int);
    void slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles);
    void slotSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void slotSourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void slotModelAboutToBeReset();
    void slotModelReset();

    QList<QAbstractItemModel *> m_models;
    int m_rowCount; // have to maintain it here since we can't compute during model destruction

    // for layoutAboutToBeChanged/layoutChanged
    QVector<QPersistentModelIndex> layoutChangePersistentIndexes;
    QModelIndexList proxyIndexes;

protected:
    MergeProxyModel *q_ptr;
};

} // namespace Liri

#endif // LIRI_MERGEPROXYMODEL_P_H
