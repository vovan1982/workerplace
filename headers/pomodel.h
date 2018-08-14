#ifndef POMODEL_H
#define POMODEL_H

#include <QtSql>
#include <QFileIconProvider>

class TreeItem;

class PoModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit PoModel(QObject *parent = 0);
    ~PoModel();
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
    bool updateRow(int row, const QModelIndex &parent = QModelIndex());
    bool sqlInsertRow(int row, const QString &name, int codGroup = 0, const QModelIndex &parent = QModelIndex(), int isGroup = 1, int codProducer = 0,
                      const QString &note = "", const QString &icon = "");
    bool sqlRemoveRow(int row, const QModelIndex &parent = QModelIndex());
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    void setFilter(const QString& filters);
    void select();
    QModelIndex moveItem(int movedId, int newParentId);
    void copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position);
    QModelIndex findData(int idData);
    QSqlError lastError();
private:
    QVector<QVariant> rootData;
    TreeItem* InitTree();
    TreeItem* search(TreeItem* , int);
    TreeItem *itemDataFromIndex(const QModelIndex& index) const;
    QModelIndex indexFromItemData(TreeItem*);
    TreeItem *rootItemData;
    QFileIconProvider ip;
    bool queryDelItem(TreeItem*);
    QString tabName,filter;
    QSqlError lastErr;
};

#endif // POMODEL_H
