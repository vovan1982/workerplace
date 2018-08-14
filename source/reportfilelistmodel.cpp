#include <QAbstractListModel>
#include "headers/reportfilelistmodel.h"
#include "headers/reportfileitem.h"

ReportFileListModel::ReportFileListModel(const QVector<ReportFileItem*>& fileItems, QObject *parent): QAbstractListModel(parent)
{
    items = fileItems;
}

int ReportFileListModel::rowCount(const QModelIndex& ) const
{
    return items.size();
}

QVariant ReportFileListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();
    if (index.row() >= items.size()) return QVariant();
    if (role == Qt::DisplayRole) {
        return QVariant(items.at(index.row())->getName());
    } else {
        return QVariant();
    }
}

QVariant ReportFileListModel::getDescription(const QModelIndex &index) const {
    if (!index.isValid()) return QVariant();
    if (index.row() >= items.size()) return QVariant();
    return QVariant(items.at(index.row())->getDescription());
}

QVariant ReportFileListModel::getPath(const QModelIndex &index) const {
    if (!index.isValid()) return QVariant();
    if (index.row() >= items.size()) return QVariant();
    return QVariant(items.at(index.row())->getPath());
}
