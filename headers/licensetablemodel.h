#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QMap>
#include <QtSql>
#include <QVariant>
#include <QAbstractTableModel>

class LicenseTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    LicenseTableModel(QObject *parent = nullptr, const QSqlDatabase db = QSqlDatabase());
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool setRowData(const QModelIndex &index, const QList<QVariant> &rowData);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRowFromModel(int row, const QModelIndex &parent=QModelIndex());
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role=Qt::EditRole);
    bool updateRowData(int row, const QModelIndex &parent=QModelIndex());
    bool select();
    void setFilter(const QString& filters);
    void clearModel();
    QString nameModelTable();
    QString aliasModelTable();
    QSqlError getLastError();
    struct colomnIndex{
        int namePO;
        int key;
        int codPO;
        int codOrganization;
        int codTypeLicense;
        int codStatePO;
        int nameProd;
        int nameOrg;
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
        int statusRow;
    } cIndex;
    struct colomnTableName{
        QString key;
        QString codPO;
        QString codOrganization;
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
    QSqlDatabase m_db;
    QString tabName,aliasTable,filter,primaryQuery;
    QSqlError lastErr;
    QList< QMap<QString,QVector<QVariant> > > changes;
    QList< QVector<QVariant> > modelData;
    QList< QVector<QVariant> > cacheModelData;
    QVector<QVariant> headerDatas;

public slots:
    bool submitAll();
    void revertAll();
};

#endif // TABLEMODEL_H
