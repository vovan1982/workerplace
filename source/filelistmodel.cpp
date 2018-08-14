#include "headers/filelistmodel.h"
#include <QApplication>
#include <QPalette>
#include <QBrush>
#include <QDir>
#include <QIcon>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    hideFileName = true;
}
QModelIndex FileListModel::index ( int row, int column, const QModelIndex & /*parent*/) const
{
    if (fileList.isEmpty() || row < 0)
        return QModelIndex();
    return createIndex(row, column);
}
int FileListModel::rowCount(const QModelIndex & /* parent */) const
{
    return fileCount;
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= fileList.size() || index.row() < 0)
        return QVariant();
    QVariant item = fileList.at(index.row());
//    if (role == Qt::DisplayRole)
//        return fileList.at(index.row());
//    else if (role == Qt::BackgroundRole) {
//        int batch = (index.row() / 100) % 2;
//        if (batch == 0)
//            return qApp->palette().base();
//        else
//            return qApp->palette().alternateBase();
//    }else
    if ((item.toString().isNull() || item.toString().isEmpty()) && role == Qt::DisplayRole)
        return item;
    if (role == Qt::DisplayRole && !hideFileName)
        return item;
    if (role == Qt::DecorationRole)
        return QIcon(dirPatch+"/"+item.toString());
    else if(role == Qt::ToolTipRole || role == Qt::EditRole)
        return item;
    return QVariant();
}

bool FileListModel::canFetchMore(const QModelIndex & /* index */) const
{
    if (fileCount < fileList.size())
        return true;
    else
        return false;
}

void FileListModel::fetchMore(const QModelIndex & /* index */)
{
    int remainder = fileList.size() - fileCount;
    int itemsToFetch = qMin(100, remainder);

    beginInsertRows(QModelIndex(), fileCount, fileCount+itemsToFetch);
    
    fileCount += itemsToFetch;

    endInsertRows();

    emit numberPopulated(itemsToFetch);
}

void FileListModel::setDirPath(const QString &path)
{
    int i = 0;
    QString defStr;
    QDir dir(path);
    dirPatch = path;
    QStringList filters;
    filters << "*.png" << "*.gif" << "*.ico";
    dir.setNameFilters(filters);
    fileList = dir.entryList();
    foreach (const QString &str, fileList) {
        if (str.contains("Default") || str.contains("default")){
            defStr = fileList.takeAt(i);
            fileList.prepend(defStr);
            break;
        }
        i++;
    }
    fileCount = 0;
    //reset();
}

QModelIndex FileListModel::indexFromItemData(const QString &file)
{
    int itemrow = fileList.indexOf(file);
    return createIndex(itemrow,0);
}

QModelIndex FileListModel::findItem(const QString &name)
{
    return indexFromItemData(name);
}

void FileListModel::setHideFileName(bool hide)
{
    hideFileName = hide;
}
