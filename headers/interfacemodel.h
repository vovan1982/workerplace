#ifndef INTERFACEMODEL_H
#define INTERFACEMODEL_H

#include <QSqlQueryModel>

class InterfaceModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    InterfaceModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index,
                 const QVariant &value, int role = Qt::EditRole);
    void setFilter(const QString &filter);
    void clearFilter();
    void select();
    void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
    bool insertRecord(int CodNetworkData, const QString &name, const QString &ip = "", const QString &mask = "", const QString &gate = "",
                      const QString &dns = "", const QString &wins = "", int autoIp = 1, int autoDns = 1, int autoWins = 1);
    bool removeRecord(int id);
    bool updateRecord(int CodNetworkInterface, int CodNetworkData, const QString &name, const QString &ip = "", const QString &mask = "", const QString &gate = "",
                      const QString &dns = "", const QString &wins = "", int autoIp = 1, int autoDns = 1, int autoWins = 1);
    QVariant lastInsertId();
private:
    int sortColumn;
    QVariant lastInsId;
    Qt::SortOrder sortOrder;
    QString iFilter;
    void refresh();
};

#endif // INTERFACEMODEL_H
