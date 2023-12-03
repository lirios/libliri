// Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
// Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mergeproxymodel.h"
#include "mergeproxymodel_p.h"

namespace Liri {

/*!
 * \class MergeProxyModel
 * \inmodule LiriModels
 * \brief This proxy takes multiple source models and concatenates their rows.
 *
 * In other words, the proxy will have all rows of the first source model,
 * followed by all rows of the second source model, etc.
 *
 * Only flat models (lists and tables) are supported, no trees.
 *
 * All models are assumed to have the same number of columns.
 * More precisely, the number of columns of the first source model is used,
 * so all source models should have at least as many columns as the first source model,
 * and additional columns in other source models will simply be ignored.
 *
 * Source models can be added and removed at runtime, including the first source
 * model (but it should keep the same number of columns).
 *
 * Dynamic insertion and removal of rows and columns in any source model is supported.
 * dataChanged, layoutChanged and reset coming from the source models are supported.
 *
 * At the moment this model doesn't support editing, drag-n-drop.
 * It could be added though, nothing prevents it.
 *
 * This proxy does not inherit from QAbstractProxyModel because it uses multiple source
 * models, rather than a single one.
 */

/*
 * KConcatenateRowsProxyModelPrivate
 */

MergeProxyModelPrivate::MergeProxyModelPrivate(MergeProxyModel* model)
    : m_rowCount(0)
    , q_ptr(model)
{
}

int MergeProxyModelPrivate::computeRowsPrior(const QAbstractItemModel *sourceModel) const
{
    int rowsPrior = 0;
    for (const QAbstractItemModel *model : m_models) {
        if (model == sourceModel)
            break;
        rowsPrior += model->rowCount();
    }
    return rowsPrior;
}

QAbstractItemModel *MergeProxyModelPrivate::sourceModelForRow(int row, int *sourceRow) const
{
    int rowCount = 0;
    QAbstractItemModel *selection = nullptr;
    for (QAbstractItemModel *model : m_models) {
        const int subRowCount = model->rowCount();
        if (rowCount + subRowCount > row) {
            selection = model;
            break;
        }
        rowCount += subRowCount;
    }
    *sourceRow = row - rowCount;
    return selection;
}

void MergeProxyModelPrivate::slotRowsAboutToBeInserted(const QModelIndex &, int start, int end)
{
    Q_Q(MergeProxyModel);

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    const int rowsPrior = computeRowsPrior(model);
    q->beginInsertRows(QModelIndex(), rowsPrior + start, rowsPrior + end);
}

void MergeProxyModelPrivate::slotRowsInserted(const QModelIndex &, int start, int end)
{
    Q_Q(MergeProxyModel);

    m_rowCount += end - start + 1;
    q->endInsertRows();
}

void MergeProxyModelPrivate::slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end)
{
    Q_Q(MergeProxyModel);

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    const int rowsPrior = computeRowsPrior(model);
    q->beginRemoveRows(QModelIndex(), rowsPrior + start, rowsPrior + end);
}

void MergeProxyModelPrivate::slotRowsRemoved(const QModelIndex &, int start, int end)
{
    Q_Q(MergeProxyModel);

    m_rowCount -= end - start + 1;
    q->endRemoveRows();
}

void MergeProxyModelPrivate::slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    Q_Q(MergeProxyModel);

    if (parent.isValid()) // we are flat
        return;

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model)
        q->beginInsertColumns(QModelIndex(), start, end);
}

void MergeProxyModelPrivate::slotColumnsInserted(const QModelIndex &parent, int, int)
{
    Q_Q(MergeProxyModel);

    if (parent.isValid()) // we are flat
        return;

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model)
        q->endInsertColumns();
}

void MergeProxyModelPrivate::slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_Q(MergeProxyModel);

    if (parent.isValid()) // we are flat
        return;

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model)
        q->beginRemoveColumns(QModelIndex(), start, end);
}

void MergeProxyModelPrivate::slotColumnsRemoved(const QModelIndex &parent, int, int)
{
    Q_Q(MergeProxyModel);

    if (parent.isValid()) // we are flat
        return;

    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model)
        q->endRemoveColumns();
}

void MergeProxyModelPrivate::slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles)
{
    Q_Q(MergeProxyModel);

    if (!from.isValid()) // QSFPM bug, it emits dataChanged(invalid, invalid) if a cell in a hidden column changes
        return;

    const QModelIndex myFrom = q->mapFromSource(from);
    const QModelIndex myTo = q->mapFromSource(to);
    Q_EMIT q->dataChanged(myFrom, myTo, roles);
}

void MergeProxyModelPrivate::slotSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(MergeProxyModel);

    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }

    Q_EMIT q->layoutAboutToBeChanged(parents, hint);

    const QModelIndexList persistentIndexList = q->persistentIndexList();
    layoutChangePersistentIndexes.reserve(persistentIndexList.size());

    for (const QPersistentModelIndex &proxyPersistentIndex : persistentIndexList) {
        proxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        layoutChangePersistentIndexes << srcPersistentIndex;
    }
}

void MergeProxyModelPrivate::slotSourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    Q_Q(MergeProxyModel);

    for (int i = 0; i < proxyIndexes.size(); ++i) {
        const QModelIndex proxyIdx = proxyIndexes.at(i);
        QModelIndex newProxyIdx = q->mapFromSource(layoutChangePersistentIndexes.at(i));
        q->changePersistentIndex(proxyIdx, newProxyIdx);
    }

    layoutChangePersistentIndexes.clear();
    proxyIndexes.clear();

    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    Q_EMIT q->layoutChanged(parents, hint);
}

void MergeProxyModelPrivate::slotModelAboutToBeReset()
{
    Q_Q(MergeProxyModel);

    const QAbstractItemModel *sourceModel = qobject_cast<const QAbstractItemModel *>(q->sender());
    Q_ASSERT(m_models.contains(const_cast<QAbstractItemModel *>(sourceModel)));
    const int oldRows = sourceModel->rowCount();
    if (oldRows > 0) {
        slotRowsAboutToBeRemoved(QModelIndex(), 0, oldRows - 1);
        slotRowsRemoved(QModelIndex(), 0, oldRows - 1);
    }
    if (m_models.at(0) == sourceModel)
        q->beginResetModel();
}

void MergeProxyModelPrivate::slotModelReset()
{
    Q_Q(MergeProxyModel);

    const QAbstractItemModel *sourceModel = qobject_cast<const QAbstractItemModel *>(q->sender());
    Q_ASSERT(m_models.contains(const_cast<QAbstractItemModel *>(sourceModel)));
    if (m_models.at(0) == sourceModel)
        q->endResetModel();
    const int newRows = sourceModel->rowCount();
    if (newRows > 0) {
        slotRowsAboutToBeInserted(QModelIndex(), 0, newRows - 1);
        slotRowsInserted(QModelIndex(), 0, newRows - 1);
    }
}

/*
 * KConcatenateRowsProxyModel
 */

/*!
 * Creates a KConcatenateRowsProxyModel.
 * \param parent optional parent
 */
MergeProxyModel::MergeProxyModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new MergeProxyModelPrivate(this))
{
}

/*!
 * Destructor.
 */
MergeProxyModel::~MergeProxyModel()
{
    delete d_ptr;
}

/*!
 * Adds a source model \a sourceModel, after all existing source models.
 * \param sourceModel the source model
 *
 * The ownership of \a sourceModel is not affected by this.
 * The same source model cannot be added more than once.
 */
void MergeProxyModel::addSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(MergeProxyModel);
    Q_ASSERT(sourceModel);
    Q_ASSERT(!d->m_models.contains(sourceModel));

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(slotRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(slotColumnsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(slotColumnsRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(slotColumnsAboutToBeInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(slotColumnsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(slotSourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)));
    connect(sourceModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(slotSourceLayoutChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint)));
    connect(sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(slotModelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()), this, SLOT(slotModelReset()));

    const int newRows = sourceModel->rowCount();
    if (newRows > 0)
        beginInsertRows(QModelIndex(), d->m_rowCount, d->m_rowCount + newRows - 1);
    d->m_rowCount += newRows;
    d->m_models.append(sourceModel);
    if (newRows > 0) {
        endInsertRows();
    }
}

/*!
 * Removes the source model \a sourceModel.
 * \param sourceModel a source model previously added to this proxy
 *
 * The ownership of \a sourceModel is not affected by this.
 */
void MergeProxyModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    Q_D(MergeProxyModel);
    Q_ASSERT(d->m_models.contains(sourceModel));

    disconnect(sourceModel, nullptr, this, nullptr);

    const int rowsRemoved = sourceModel->rowCount();
    const int rowsPrior = d->computeRowsPrior(sourceModel);   // location of removed section

    if (rowsRemoved > 0)
        beginRemoveRows(QModelIndex(), rowsPrior, rowsPrior + rowsRemoved - 1);
    d->m_models.removeOne(sourceModel);
    d->m_rowCount -= rowsRemoved;
    if (rowsRemoved > 0)
        endRemoveRows();
}

/*!
 * Returns the proxy index for a given source index
 * \param sourceIndex an index coming from any of the source models
 * \return a proxy index
 * Calling this method with an index not from a source model is undefined behavior.
 */
QModelIndex MergeProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    Q_D(const MergeProxyModel);

    const QAbstractItemModel *sourceModel = sourceIndex.model();
    int rowsPrior = d->computeRowsPrior(sourceModel);
    return createIndex(rowsPrior + sourceIndex.row(), sourceIndex.column());
}

/*!
 * Returns the source index for a given proxy index.
 * \param proxyIndex an index for this proxy model
 * \return a source index
 */
QModelIndex MergeProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    Q_D(const MergeProxyModel);

    if (!proxyIndex.isValid())
        return QModelIndex();

    const int row = proxyIndex.row();
    int sourceRow;
    QAbstractItemModel *sourceModel = d->sourceModelForRow(row, &sourceRow);
    if (!sourceModel)
        return QModelIndex();
    return sourceModel->index(sourceRow, proxyIndex.column());
}

QVariant MergeProxyModel::data(const QModelIndex &index, int role) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid())
        return QVariant();
    return sourceIndex.model()->data(sourceIndex, role);
}

bool MergeProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid())
        return false;
    QAbstractItemModel *sourceModel = const_cast<QAbstractItemModel *>(sourceIndex.model());
    return sourceModel->setData(sourceIndex, value, role);
}

QMap<int, QVariant> MergeProxyModel::itemData(const QModelIndex &proxyIndex) const
{
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    return sourceIndex.model()->itemData(sourceIndex);
}

Qt::ItemFlags MergeProxyModel::flags(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.model()->flags(sourceIndex);
}

QVariant MergeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_D(const MergeProxyModel);

    if (d->m_models.isEmpty())
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return d->m_models.at(0)->headerData(section, orientation, role);
    } else {
        int sourceRow;
        QAbstractItemModel *sourceModel = d->sourceModelForRow(section, &sourceRow);
        if (!sourceModel)
            return QVariant();
        return sourceModel->headerData(sourceRow, orientation, role);
    }
}

int MergeProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_D(const MergeProxyModel);

    if (d->m_models.isEmpty())
        return 0;
    if (parent.isValid())
        return 0; // flat model;
    return d->m_models.at(0)->columnCount(QModelIndex());
}

QModelIndex MergeProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(column >= 0);
    Q_D(const MergeProxyModel);

    int sourceRow;
    QAbstractItemModel *sourceModel = d->sourceModelForRow(row, &sourceRow);
    if (!sourceModel)
        return QModelIndex();
    return mapFromSource(sourceModel->index(sourceRow, column, parent));
}

QModelIndex MergeProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex(); // we are flat, no hierarchy
}

int MergeProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const MergeProxyModel);

    if (parent.isValid())
        return 0;    // flat model

    return d->m_rowCount;
}

} // namespace Liri

#include "moc_mergeproxymodel.cpp"
