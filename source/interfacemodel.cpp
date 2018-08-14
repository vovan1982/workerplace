#include <QtSql>
#include "headers/interfacemodel.h"

InterfaceModel::InterfaceModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    lastInsId = QVariant();
    sortColumn = 1;
    sortOrder = Qt::AscendingOrder;
}

Qt::ItemFlags InterfaceModel::flags(
        const QModelIndex &index) const {

    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
//    if (index.column() >= 1 && index.column() <= 7 && index.column() != 2 && index.column() != 5)
//        flags |= Qt::ItemIsEditable;
//    if (index.column() == 4)
//        flags |= Qt::ItemIsUserCheckable;
    return flags;
}

QVariant InterfaceModel::data(
            const QModelIndex &index,
            int role) const {

    QVariant value = QSqlQueryModel::data(index, role);

    switch (role) {
    case Qt::DisplayRole:
        if(index.column() >= 3 &&  index.column() <=5)
            if(QSqlQueryModel::data(QSqlQueryModel::index(index.row(),8)).toInt() == 1) return QVariant(tr("Авто"));
        if(index.column() == 6)
            if(QSqlQueryModel::data(QSqlQueryModel::index(index.row(),9)).toInt() == 1) return QVariant(tr("Авто"));
        if(index.column() == 7)
            if(QSqlQueryModel::data(QSqlQueryModel::index(index.row(),10)).toInt() == 1) return QVariant(tr("Авто"));
        return value;
    }
    return QVariant();
}

bool InterfaceModel::setData(
            const QModelIndex &index,
            const QVariant &value,
            int /* role */) {
    if (index.column() < 1)
        return false;

    QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
    int id = QSqlQueryModel::data(primaryKeyIndex).toInt();

    bool ok;
    QSqlQuery query;
    if (index.column() == 1) {
        query.prepare("update networkinterface set CodNetworkData = ? where CodNetworkInterface = ?");
        query.addBindValue(value.toInt());
        query.addBindValue(id);
    }else if(index.column() == 2) {
        query.prepare("update networkinterface set Name = ? where CodNetworkInterface = ?");
        query.addBindValue(value.toString());
        query.addBindValue(id);
    }else if(index.column() >= 3 && index.column() <= 7) {
        if(value.isNull()){
            if(index.column() == 3){
                query.prepare("update networkinterface set IP = NULL where CodNetworkInterface = ?");
                query.addBindValue(id);
            }
            if(index.column() == 4){
                query.prepare("update networkinterface set Mask = NULL where CodNetworkInterface = ?");
                query.addBindValue(id);
            }
            if(index.column() == 5){
                query.prepare("update networkinterface set Gate = NULL where CodNetworkInterface = ?");
                query.addBindValue(id);
            }
            if(index.column() == 6){
                query.prepare("update networkinterface set DNS = NULL where CodNetworkInterface = ?");
                query.addBindValue(id);
            }
            if(index.column() == 7){
                query.prepare("update networkinterface set WINS = NULL where CodNetworkInterface = ?");
                query.addBindValue(id);
            }
        }else{
            if(index.column() == 3){
                query.prepare("update networkinterface set IP = ? where CodNetworkInterface = ?");
                query.addBindValue(value.toString());
                query.addBindValue(id);
            }
            if(index.column() == 4){
                query.prepare("update networkinterface set Mask = ? where CodNetworkInterface = ?");
                query.addBindValue(value.toString());
                query.addBindValue(id);
            }
            if(index.column() == 5){
                query.prepare("update networkinterface set Gate = ? where CodNetworkInterface = ?");
                query.addBindValue(value.toString());
                query.addBindValue(id);
            }
            if(index.column() == 6){
                query.prepare("update networkinterface set DNS = ? where CodNetworkInterface = ?");
                query.addBindValue(value.toString());
                query.addBindValue(id);
            }
            if(index.column() == 7){
                query.prepare("update networkinterface set WINS = ? where CodNetworkInterface = ?");
                query.addBindValue(value.toString());
                query.addBindValue(id);
            }
        }
    }else if(index.column() == 8) {
        query.prepare("update networkinterface set AutoIP = ? where CodNetworkInterface = ?");
        query.addBindValue(value.toInt());
        query.addBindValue(id);
    }else if(index.column() == 9) {
        query.prepare("update networkinterface set AutoDNS = ? where CodNetworkInterface = ?");
        query.addBindValue(value.toInt());
        query.addBindValue(id);
    }else if(index.column() == 10) {
        query.prepare("update networkinterface set AutoWINS = ? where CodNetworkInterface = ?");
        query.addBindValue(value.toInt());
        query.addBindValue(id);
    }
    ok = query.exec();
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}
bool InterfaceModel::insertRecord(int CodNetworkData, const QString &name, const QString &ip, const QString &mask, const QString &gate,
                                  const QString &dns, const QString &wins, int autoIp, int autoDns, int autoWins)
{
    QString queryText1, queryText2;
    QSqlQuery query;
    bool ok;
    queryText1 = "INSERT INTO networkinterface (CodNetworkData, Name";
    queryText2 = QString(" VALUES (%1,'%2'").arg(CodNetworkData).arg(name);
    if(!ip.isEmpty()){
        queryText1 += ", IP";
        queryText2 += ",'" + ip + "'";
    }
    if(!mask.isEmpty()){
        queryText1 += ", Mask";
        queryText2 += ",'" + mask + "'";
    }
    if(!gate.isEmpty()){
        queryText1 += ", Gate";
        queryText2 += ",'" + gate + "'";
    }
    if(!dns.isEmpty()){
        queryText1 += ", DNS";
        queryText2 += ",'" + dns + "'";
    }
    if(!wins.isEmpty()){
        queryText1 += ", WINS";
        queryText2 += ",'" + wins + "'";
    }
    if(autoIp < 1){
        queryText1 += ", AutoIP";
        queryText2 += QString(",%1").arg(autoIp);
    }
    if(autoDns < 1){
        queryText1 += ", AutoDNS";
        queryText2 += QString(",%1").arg(autoDns);
    }
    if(autoWins < 1){
        queryText1 += ", AutoWINS";
        queryText2 += QString(",%1").arg(autoWins);
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
bool InterfaceModel::removeRecord(int id)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("DELETE FROM networkinterface WHERE CodNetworkInterface = %1").arg(id));
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}
bool InterfaceModel::updateRecord(int CodNetworkInterface, int CodNetworkData, const QString &name, const QString &ip, const QString &mask, const QString &gate,
                                  const QString &dns, const QString &wins, int autoIp, int autoDns, int autoWins)
{
    QString queryText;
    QSqlQuery query;
    bool ok, prev;
    prev = false;
    queryText = "UPDATE networkinterface SET ";
    if(CodNetworkData != 0){
        queryText += QString("CodNetworkData = %1").arg(CodNetworkData);
        prev = true;
    }
    if(!name.isEmpty()){
        if(prev) queryText += QString(", "); else prev = true;
        queryText += QString("Name = '%1'").arg(name);
    }
    if(autoIp == 1){
        if(prev) queryText += QString(", "); else prev = true;
        queryText += QString("IP = NULL, ");
        queryText += QString("Mask = NULL, ");
        queryText += QString("Gate = NULL");
    }else{
        if(!ip.isEmpty()){
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("IP = '%1'").arg(ip);
        }else{
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("IP = NULL");
        }
        if(!mask.isEmpty()){
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("Mask = '%1'").arg(mask);
        }else{
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("Mask = NULL");
        }
        if(!gate.isEmpty()){
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("Gate = '%1'").arg(gate);
        }else{
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("Gate = NULL");
        }
    }
    if(prev) queryText += QString(", "); else prev = true;
    queryText += QString("AutoIP = %1").arg(autoIp);
    if(autoDns == 1){
        if(prev) queryText += QString(", "); else prev = true;
        queryText += QString("DNS = NULL");
    }else{
        if(!dns.isEmpty()){
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("DNS = '%1'").arg(dns);
        }else{
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("DNS = NULL").arg(dns);
        }
    }
    if(prev) queryText += QString(", "); else prev = true;
    queryText += QString("AutoDNS = %1").arg(autoDns);
    if(autoWins == 1){
        if(prev) queryText += QString(", "); else prev = true;
        queryText += QString("WINS = NULL");
    }else{
        if(!wins.isEmpty()){
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("WINS = '%1'").arg(wins);
        }else{
            if(prev) queryText += QString(", "); else prev = true;
            queryText += QString("WINS = NULL").arg(wins);
        }
    }
    if(prev) queryText += QString(", "); else prev = true;
    queryText += QString("AutoWINS = %1").arg(autoWins);

    queryText += QString(" WHERE CodNetworkInterface = %1").arg(CodNetworkInterface);
    ok = query.exec(queryText);
    if(!ok)
        setLastError(query.lastError());
    else
        refresh();
    return ok;
}

void InterfaceModel::setFilter(const QString &filter)
{
    iFilter = filter;
}
void InterfaceModel::clearFilter()
{
    iFilter = "";
}
void InterfaceModel::refresh() {
    QString sortOrd = "";
    if(sortOrder == Qt::DescendingOrder)
        sortOrd = " DESC";
    if(!iFilter.isNull() && !iFilter.isEmpty())
        setQuery("SELECT * FROM networkinterface WHERE " + iFilter + " ORDER BY " + QString("%1").arg(sortColumn+1) + sortOrd);
    else
        setQuery("SELECT * FROM networkinterface ORDER BY " + QString("%1").arg(sortColumn+1) + sortOrd);
    setHeaderData(2, Qt::Horizontal,
                  tr("Название"));
    setHeaderData(3, Qt::Horizontal,
                  tr("IP адрес"));
    setHeaderData(4, Qt::Horizontal,
                  tr("Маска подсети"));
    setHeaderData(5, Qt::Horizontal,
                  tr("Шлюз"));
    setHeaderData(6, Qt::Horizontal,
                  tr("DNS сервера"));
    setHeaderData(7, Qt::Horizontal,
                  tr("WINS сервера"));
    dataChanged(QModelIndex(),QSqlQueryModel::index(QSqlQueryModel::rowCount()-1, QSqlQueryModel::columnCount()));
}
void InterfaceModel::sort ( int column, Qt::SortOrder order)
{
    sortColumn = column;
    sortOrder = order;
    refresh();
}
QVariant InterfaceModel::lastInsertId()
{
    return lastInsId;
}
void InterfaceModel::select()
{
    refresh();
}
