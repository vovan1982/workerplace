#ifndef LICENSEMODEL_H
#define LICENSEMODEL_H

#include <QtSql>
#include <QFileIconProvider>

class TreeItem;

class LicenseModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit LicenseModel(QObject *parent = 0);
    ~LicenseModel();
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
    bool sqlInsertRow(int row, int codWorkerPlace, int codPO, int codTypeLicense, int codStatePO,
                      QMap<QString,QVariant> colValue = QMap<QString,QVariant>(), const QModelIndex &parent = QModelIndex());
    bool sqlRemoveRow(int row, const QModelIndex &parent = QModelIndex());
    bool sqlUpdateRow(int row, QMap<QString,QVariant> setData = QMap<QString,QVariant>(), const QModelIndex &parent = QModelIndex());
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    void setFilter(const QString& filters);
    void select();
    QModelIndex moveItem(int movedId, int newParentId);
    void copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position);
    QModelIndex findData(int idData);
    QSqlError lastError();
    void setShowParent(bool,QString);
    QString aliasModelTable();
    QString nameModelTable();
    struct colomnIndex{
        int namePO;
        int key;
        int codDevice;
        int codPO;
        int codWorkerPlace;
        int codTypeLicense;
        int codStatePO;
        int nameProd;
        int nameDevice;
        int nameWP;
        int regName;
        int regKey;
        int regMail;
        int kolLicense;
        int invN;
        int versionN;
        int codProvider;
        int datePurchase;
        int dateEndLicense;
        int price;
        int nameLic;
        int nameState;
        int note;
        int ico;
        int rv;
        int isLicense;
    } cIndex;
    struct colomnTableName{
        QString key;
        QString codDevice;
        QString codPO;
        QString codWorkerPlace;
        QString codTypeLicense;
        QString codStatePO;
        QString regName;
        QString regKey;
        QString regMail;
        QString kolLicense;
        QString invN;
        QString versionN;
        QString codProvider;
        QString datePurchase;
        QString dateEndLicense;
        QString price;
        QString note;
        QString rv;
    } colTabName;
private:
    QVector<QVariant> rootData;
    TreeItem* InitTree();
    TreeItem* search(TreeItem* , int);
    TreeItem *itemDataFromIndex(const QModelIndex& index) const;
    QModelIndex indexFromItemData(TreeItem*);
    TreeItem *rootItemData;
    QFileIconProvider ip;
    bool queryDelItem(TreeItem*);
    QString tabName,treeTable,aliasTable,filter,primaryQuery;
    QSqlError lastErr;
    bool showParent;
    QString parentDevId;
};

#endif // LICENSEMODEL_H
