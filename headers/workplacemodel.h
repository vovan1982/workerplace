#ifndef WORKPLACEMODEL_H
#define WORKPLACEMODEL_H

#include <QSqlDatabase>
#include <QSqlQueryModel>

class WorkPlaceModel : public QSqlQueryModel {
    Q_OBJECT
public:
    WorkPlaceModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,
                 const QVariant &value, int role = Qt::EditRole);
    void setFilter(const QString &filter);
    void clearFilter();
    void select();
    void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
    bool insertRecord(int CodDepartment, const QString &name, int PrimaryUser = 0,
                      const QString &InternalNumber = "", const QString &Location = "", int WareHouse = 0);
    bool removeRecord(int id);
    bool updateRecord(int CodWorkerPlace, int CodDepartment = 0, const QString &name = "", int PrimaryUser = 0,
                      const QString &InternalNumber = "", const QString &Location = "");
    QVariant lastInsertId();
    QMap<int,QVariant> dataRecord(int row) const;
    QString tableName() const;
private:
    int sortColumn;
    QVariant lastInsId;
    Qt::SortOrder sortOrder;
    QString wpFilter;
    void refresh();
};

#endif // WORKPLACEMODEL_H
