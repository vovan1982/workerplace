#include <QtSql>
#include "headers/workplacemodel.h"

WorkPlaceModel::WorkPlaceModel(QObject *parent)
                : QSqlQueryModel(parent)
{
    lastInsId = QVariant();
    sortColumn = 3;
    sortOrder = Qt::AscendingOrder;
}

Qt::ItemFlags WorkPlaceModel::flags(
        const QModelIndex &index) const {

    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() >= 1 && index.column() <= 7 && index.column() != 2 && index.column() != 5)
        flags |= Qt::ItemIsEditable;
    return flags;
}

QVariant WorkPlaceModel::data(
        const QModelIndex &index,
        int role) const {

    QVariant value = QSqlQueryModel::data(index, role);
    QVariant wh = QSqlQueryModel::data(QSqlQueryModel::index(index.row(),10));

    switch (role) {
    case Qt::DisplayRole:
        return value;
    case Qt::DecorationRole: {
        if (index.column() == 3 && wh.toInt() == 1)
            return QIcon(":/128x128/wp_and_wh/ico/warehouse.png");
        else if (index.column() == 3 && wh.toInt() == 0)
            return QIcon(":/128x128/wp_and_wh/ico/workplase.png");
        else
            return QVariant();
    }
    }
    return QVariant();
}

bool WorkPlaceModel::setData(
            const QModelIndex &index,
            const QVariant &value,
            int /* role */) {
    if (index.column() < 1 || index.column() > 7 ||
        index.column() == 2 || index.column() == 5)
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(
                index.row(), 0);
    int id = QSqlQueryModel::data(primaryKeyIndex).toInt();
    bool ok;
    QSqlQuery query;
    int rowVersion;
    if(QSqlQueryModel::data(QSqlQueryModel::index(index.row(),11)).toInt() == 254)
        rowVersion = 0;
    else
        rowVersion = QSqlQueryModel::data(QSqlQueryModel::index(index.row(),11)).toInt()+1;
    if (index.column() == 1) {
        query.prepare("update workerplace set CodDepartment = ?, RV = ? where CodWorkerPlace = ?");
        query.addBindValue(value.toInt());
        query.addBindValue(rowVersion);
        query.addBindValue(id);
    }else if(index.column() == 3) {
        query.prepare("update workerplace set Name = ?, RV = ? where CodWorkerPlace = ?");
        query.addBindValue(value.toString());
        query.addBindValue(rowVersion);
        query.addBindValue(id);
    }else if(index.column() == 4) {
        if(value.isNull()){
            query.prepare("update workerplace set PrimaryUser = NULL, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }else{
            query.prepare("update workerplace set PrimaryUser = ?, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(value.toInt());
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }
    }else if(index.column() == 6) {
        if(value.isNull()){
            query.prepare("update workerplace set InternalNumber = NULL, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }else{
            query.prepare("update workerplace set InternalNumber = ?, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(value.toString());
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }
    }else if(index.column() == 7) {
        if(value.isNull()){
            query.prepare("update workerplace set Location = NULL, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }else{
            query.prepare("update workerplace set Location = ?, RV = ? where CodWorkerPlace = ?");
            query.addBindValue(value.toString());
            query.addBindValue(rowVersion);
            query.addBindValue(id);
        }
    }
    ok = query.exec();
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}
bool WorkPlaceModel::insertRecord(int CodDepartment, const QString &name, int PrimaryUser,
                                  const QString &InternalNumber, const QString &Location, int WareHouse)
{
    QString queryText1, queryText2;
    QSqlQuery query;
    bool ok;
    queryText1 = "INSERT INTO workerplace (CodDepartment, Name";
    queryText2 = QString(" VALUES (%1,'%2'").arg(CodDepartment).arg(name);
    if(PrimaryUser != 0){
        queryText1 += ", PrimaryUser";
        queryText2 += QString(",%1").arg(PrimaryUser);
    }
    if(!InternalNumber.isEmpty()){
        queryText1 += ", InternalNumber";
        queryText2 += ",'" + InternalNumber + "'";
    }
    if(!Location.isEmpty()){
        queryText1 += ", Location";
        queryText2 += ",'" + Location + "'";
    }
    if(WareHouse != 0){
        queryText1 += ", warehouse";
        queryText2 += QString(",%1").arg(WareHouse);
    }
    queryText1 += ")";
    queryText2 += ")";
    queryText1 += queryText2;
    ok = query.exec(queryText1);
    if(!ok)
        setLastError(query.lastError());
    else{
        lastInsId = query.lastInsertId();
        refresh();
    }
    return ok;
}
bool WorkPlaceModel::removeRecord(int id)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("DELETE FROM workerplace WHERE CodWorkerPlace = %1").arg(id));
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}
bool WorkPlaceModel::updateRecord(int CodWorkerPlace, int CodDepartment, const QString &name, int PrimaryUser,
                  const QString &InternalNumber, const QString &Location)
{
    if(CodWorkerPlace == 0 || (CodDepartment == 0 && name.isEmpty() && PrimaryUser == 0 && InternalNumber.isEmpty() && Location.isEmpty() ))
        return false;
    QString queryText;
    QSqlQuery query;
    bool ok;
    int rowVersion;
    QQueue<QVariant> bindval;
    if(!query.exec(QString("SELECT RV FROM workerplace WHERE CodWorkerPlace = %1").arg(CodWorkerPlace))){
        setLastError(query.lastError());
        return false;
    }
    query.next();
    if(query.value(0).toInt() == 254)
        rowVersion = 0;
    else
        rowVersion = query.value(0).toInt()+1;
    queryText = "UPDATE workerplace SET ";
    if(CodDepartment != 0){
        queryText += "CodDepartment = ?,";
        bindval.enqueue(CodDepartment);
    }
    if(!name.isEmpty()){
        queryText += "Name = ?,";
        bindval.enqueue(name);
    }
    if(PrimaryUser != 0){
        queryText += "PrimaryUser = ?,";
        bindval.enqueue(PrimaryUser);
    }
    if(!InternalNumber.isEmpty()){
        queryText += "InternalNumber = ?,";
        bindval.enqueue(InternalNumber);
    }
    if(!Location.isEmpty()){
        queryText += "Location = ?,";
        bindval.enqueue(Location);
    }
    queryText += "RV = ?";
    queryText += " WHERE CodWorkerPlace = ?";
    bindval.enqueue(rowVersion);
    bindval.enqueue(CodWorkerPlace);

    query.prepare(queryText);
    while(!bindval.empty()){
        query.addBindValue(bindval.dequeue());
    }
    ok = query.exec();
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}

void WorkPlaceModel::setFilter(const QString &filter)
{
    wpFilter = filter;
}
void WorkPlaceModel::clearFilter()
{
    wpFilter = "";
}
void WorkPlaceModel::refresh() {
    QString sortOrd = "";
    if(sortOrder == Qt::DescendingOrder)
        sortOrd = " DESC";
    if(!wpFilter.isNull() && !wpFilter.isEmpty())
        setQuery("SELECT "
                 "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                 "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                 "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                 "workerplace.warehouse, workerplace.RV "
                 "FROM workerplace "
                 "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                 "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                 "WHERE " + wpFilter + " ORDER BY " + QString("%1").arg(sortColumn+1) + sortOrd);
    else
        setQuery("SELECT "
                 "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                 "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                 "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                 "workerplace.warehouse, workerplace.RV "
                 "FROM workerplace "
                 "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                 "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                 "ORDER BY "  + QString("%1").arg(sortColumn+1) + sortOrd);
    setHeaderData(2, Qt::Horizontal,
                  tr("Подразделение"));
    setHeaderData(3, Qt::Horizontal,
                  tr("Название"));
    setHeaderData(5, Qt::Horizontal,
                  tr("Основной\nпользователь"));
    setHeaderData(6, Qt::Horizontal,
                  tr("Внутренний\nномер"));
    setHeaderData(7, Qt::Horizontal,
                  tr("Место\nнахождения"));
}
void WorkPlaceModel::sort ( int column, Qt::SortOrder order)
{
    sortColumn = column;
    sortOrder = order;
    refresh();
}
QVariant WorkPlaceModel::lastInsertId()
{
    return lastInsId;
}
void WorkPlaceModel::select()
{
    refresh();
}
QMap<int, QVariant> WorkPlaceModel::dataRecord(int row) const
{
    QMap<int, QVariant> datasRecord;
    for(int i = 0;i<QSqlQueryModel::columnCount();i++)
        datasRecord[i] = QSqlQueryModel::data(QSqlQueryModel::index(row,i));
    return datasRecord;
}
QString WorkPlaceModel::tableName() const
{
    return "workerplace";
}
