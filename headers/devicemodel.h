#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QtSql>

class TreeItem;

class DeviceModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DeviceModel(const QString &dbConnectionName = "default", const QSqlDatabase &connectionData = QSqlDatabase(), QObject *parent = 0, bool inThread = true);
    ~DeviceModel();
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& index = QModelIndex()) const;
    int columnCount(const QModelIndex& index = QModelIndex()) const;
//    bool canFetchMore(const QModelIndex & parent) const;
//    void fetchMore(const QModelIndex & parent);
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
    bool sqlInsertRow(int row, int codOrganization, int codWorkerPlace, int codTypeDevice, const QString &name, int codState, int type,
                      QMap<QString,QVariant> colValue = QMap<QString,QVariant>(),const QModelIndex &parent = QModelIndex());
    bool sqlRemoveRow(int row, const QModelIndex &parent = QModelIndex());
    bool sqlUpdateRow(int row, QMap<QString,QVariant> setData = QMap<QString,QVariant>(), const QModelIndex &parent = QModelIndex());
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex());
    void setFilter(const QString& filters);
    void select();
    void clear();
    void initEmpty();
    QModelIndex moveItem(int movedId, int newParentId);
    void copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position);
    QModelIndex findData(int idData);
    QSqlError lastError();
    QString aliasModelTable();
    QString nameModelTable();
    QString nameModelTreeTable();
    struct colomnIndex{
        int typeDevName;
        int id;
        int parent_id;
        int codTypeDevice;
        int codOrganization;
        int codWorkerPlace;
        int orgName;
        int wpName;
        int name;
        int networkName;
        int codDomainWg;
        int domainWgName;
        int inventoryN;
        int serialN;
        int codProducer;
        int producerName;
        int codProvider;
        int providerName;
        int datePurchase;
        int datePosting;
        int endGuarantee;
        int price;
        int codState;
        int stateName;
        int note;
        int type;
        int iconPath;
        int rv;
        int detailDescription;
    } cIndex;
    struct colomnTableName{
        QString id;
        QString parent_id;
        QString codOrganization;
        QString codWorkerPlace;
        QString codTypeDevice;
        QString name;
        QString networkName;
        QString codDomainWg;
        QString domainWgName;
        QString inventoryN;
        QString serialN;
        QString codProvider;
        QString codProducer;
        QString datePurchase;
        QString datePosting;
        QString endGuarantee;
        QString price;
        QString codState;
        QString note;
        QString rv;
        QString detailDescription;
    } colTabName;

private:
    QMap<QString,QVariant> credentials;
    TreeItem* search(TreeItem* , int);
    TreeItem *itemDataFromIndex(const QModelIndex& index) const;
    QModelIndex indexFromItemData(TreeItem*);
    TreeItem *rootItemData;
    bool queryDelItem(TreeItem*);
    QString tabName,treeTable,aliasTable,filter,primaryQuery;
    QSqlError lastErr;
    bool m_inThread;
signals:
    void newTreeIsSet();
    void logData(QString text);
public slots:
    void setNewTree(TreeItem* deviceTree);
private slots:
    void setLastError(const QSqlError &error);
    void setError(QString errorText);
};

#endif // DEVICEMODEL_H
