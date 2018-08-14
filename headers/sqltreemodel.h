#ifndef SQLTREEMODEL_H
#define SQLTREEMODEL_H

#include <QtWidgets>
#include <QFileIconProvider>
#include <QtSql>

class TreeItem;

class SqlTreeModel : public QAbstractItemModel {
    Q_OBJECT
public:
    SqlTreeModel(QObject *parent = 0);
    ~SqlTreeModel();
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& index = QModelIndex()) const;
    int columnCount(const QModelIndex& index = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    void setRootItemData(TreeItem *item);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool setDataWithOutSQL(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool sqlInsertRow(int row, const QModelIndex &parent = QModelIndex());
    bool sqlRemoveRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool updateRow(int row, const QModelIndex &parent = QModelIndex());
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    Qt::DropActions supportedDropActions() const;
    void setTable(const QString& tableName);
    void setRootName(const QString& rootName);
    void setFilter(const QString& filters);
    void setHideRoot(bool ok = false);
    QString tableName() const;
    void select();
    QModelIndex moveItem(int movedId, int newParentId);
    void copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position);
    QModelIndex findData(int idData);
private:
    TreeItem* InitTree();
    TreeItem* search(TreeItem* , int);
    TreeItem *itemDataFromIndex(const QModelIndex& index) const;
    QModelIndex indexFromItemData(TreeItem*);
    TreeItem *rootItemData;
    bool queryDelItem(TreeItem*);
    QFileIconProvider ip;
    QString tabName, rtName, filter;
    bool hideRoot;
};

#endif // SQLTREEMODEL_H
