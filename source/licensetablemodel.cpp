#include "headers/licensetablemodel.h"

LicenseTableModel::LicenseTableModel(QObject *parent, const QSqlDatabase db) : QAbstractTableModel(parent)
{
    filter = "";
    tabName = "licenses";
    aliasTable = "l";
    primaryQuery = QString("SELECT po.Name AS NamePO, "
                           "%2.`key`, "
                           "%2.CodPO, "
                           "%2.CodOrganization, "
                           "%2.CodTypeLicense, "
                           "%2.CodStatePO, "
                           "departments.Name AS NameOrg, "
                           "producer.Name AS NameProd, "
                           "%2.RegName, "
                           "%2.RegKey, "
                           "%2.RegMail, "
                           "%2.KolLicense, "
                           "%2.InvN, "
                           "%2.VersionN, "
                           "%2.CodProvider, "
                           "%2.DatePurchase, "
                           "%2.DateEndLicense, "
                           "%2.Price, "
                           "typelicense.Name AS NameLic, "
                           "statepo.Name AS NameState, "
                           "%2.Note, "
                           "po.Ico, "
                           "%2.RV "
                           "FROM %1 %2 "
                           "INNER JOIN typelicense "
                           "ON %2.CodTypeLicense = typelicense.CodTypeLicense "
                           "INNER JOIN statepo "
                           "ON %2.CodStatePO = statepo.CodStatePO "
                           "INNER JOIN po "
                           "ON %2.CodPO = po.CodPO "
                           "LEFT OUTER JOIN departments "
                           "ON %2.CodOrganization = departments.id "
                           "LEFT OUTER JOIN producer "
                           "ON po.CodProducer = producer.CodProducer ").arg(tabName).arg(aliasTable);
    headerDatas << "NamePO";
    cIndex.namePO = 0;
    headerDatas << "key";
    cIndex.key = 1;
    colTabName.key = "key";
    headerDatas << "CodPO";
    cIndex.codPO = 2;
    colTabName.codPO = "CodPO";
    headerDatas << "CodOrganization";
    cIndex.codOrganization = 3;
    colTabName.codOrganization = "CodOrganization";
    headerDatas << "CodTypeLicense";
    cIndex.codTypeLicense = 4;
    colTabName.codTypeLicense = "CodTypeLicense";
    headerDatas << "CodStatePO";
    cIndex.codStatePO = 5;
    colTabName.codStatePO = "CodStatePO";
    headerDatas << "NameOrg";
    cIndex.nameOrg = 6;
    headerDatas << "NameProd";
    cIndex.nameProd = 7;
    headerDatas << "RegName";
    cIndex.regName = 8;
    colTabName.regName = "RegName";
    headerDatas << "RegKey";
    cIndex.regKey = 9;
    colTabName.regKey = "RegKey";
    headerDatas << "RegMail";
    cIndex.regMail = 10;
    colTabName.regMail = "RegMail";
    headerDatas << "KolLicense";
    cIndex.kolLicense = 11;
    colTabName.kolLicense = "KolLicense";
    headerDatas << "InvN";
    cIndex.invN = 12;
    colTabName.invN = "InvN";
    headerDatas << "VersionN";
    cIndex.versionN = 13;
    colTabName.versionN = "VersionN";
    headerDatas << "CodProvider";
    cIndex.codProvider = 14;
    colTabName.codProvider = "CodProvider";
    headerDatas << "DatePurchase";
    cIndex.datePurchase = 15;
    colTabName.datePurchase = "DatePurchase";
    headerDatas << "DateEndLicense";
    cIndex.dateEndLicense = 16;
    colTabName.dateEndLicense = "DateEndLicense";
    headerDatas << "Price";
    cIndex.price = 17;
    colTabName.price = "Price";
    headerDatas << "NameLic";
    cIndex.nameLic = 18;
    headerDatas << "NameState";
    cIndex.nameState = 19;
    headerDatas << "Note";
    cIndex.note = 20;
    colTabName.note = "Note";
    headerDatas << "Ico";
    cIndex.ico = 21;
    headerDatas << "RV";
    cIndex.rv = 22;
    colTabName.rv = "RV";
    headerDatas << "statusRow";
    cIndex.statusRow = 23;

    if(QSqlDatabase::connectionNames().contains("LicenseTableModel_connection")){
        m_db = QSqlDatabase::database("LicenseTableModel_connection");
    }else{
        m_db = QSqlDatabase::addDatabase(db.driverName(),"LicenseTableModel_connection");
    }
    m_db.setHostName(db.hostName());
    m_db.setDatabaseName(db.databaseName());
    m_db.setPort(db.port());
    m_db.setUserName(db.userName());
    m_db.setPassword(db.password());
    m_db.open();
    lastErr = m_db.lastError();
}

int LicenseTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return modelData.size();
}

int LicenseTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return headerDatas.size();
}

QVariant LicenseTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= modelData.size() || index.row() < 0)
        return QVariant();

    QVector<QVariant> rowData = modelData.at(index.row());

    if (role == Qt::DisplayRole) {
        return rowData.at(index.column());
    }

    if (role == Qt::DecorationRole) {
        if (index.column() == 0){
            if(rowData.at(cIndex.ico).toString().isEmpty() || rowData.at(cIndex.ico).toString().isNull()){
                return QIcon(QDir::currentPath()+"/ico/typepoico/Default.png");
            }else{
                return QIcon(QDir::currentPath()+"/ico/typepoico/"+rowData.at(cIndex.ico).toString());
            }
        }
    }
    return QVariant();
}

QVariant LicenseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal || section >= headerDatas.size() || section < 0)
        return QVariant();

    return headerDatas[section];
}

bool LicenseTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal || section < 0)
    {
        return false;
    }

    auto headerSize = headerDatas.size();

    if (section == headerSize)
    {
        headerDatas.push_back(value);
        emit headerDataChanged(orientation, section, section);
        return true;
    }

    if (section > headerSize)
    {
        std::size_t voidValuesCounter = section - headerSize;
        for(std::size_t i = 0;i<voidValuesCounter;i++){
            headerDatas.push_back(QVariant());
        }
        headerDatas.push_back(value);
        emit headerDataChanged(orientation, section, section);
        return true;
    }

    headerDatas[section] = value;
    emit headerDataChanged(orientation, section, section);
    return true;
}

bool LicenseTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; row++) {
        QVector<QVariant> rowData(headerDatas.size());
        rowData[rowData.size()-1] = "ins";
        modelData.insert(position, rowData);
    }
    endInsertRows();
    return true;
}

bool LicenseTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    for (int row=0; row < rows; ++row) {
        QVector<QVariant> p = modelData.value(row);
        if(p.at(p.size()-1).toString().compare("ins") == 0){
            modelData.removeAt(position);
        }else{
            p[p.size()-1] = "rem";
            modelData.replace(position, p);
        }
    }
    endRemoveRows();
    return true;
}

bool LicenseTableModel::removeRowFromModel(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), row, row);
    modelData.removeAt(row);
    endRemoveRows();
    return true;
}

bool LicenseTableModel::updateRowData(int row, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    QSqlQuery query(m_db);
    QVector<QVariant> p = modelData.at(row);

    query.exec(QString("%1WHERE %2").arg(primaryQuery).arg("`key` = " + p.at(cIndex.key).toString()));

    if (query.lastError().type() != QSqlError::NoError){
        lastErr = query.lastError();
        return false;
    }
    if(query.size()>0){
        query.next();
        for(int i = 0; i<columnCount();i++)
            p[i] = query.value(i);
    }else{
        lastErr.setDatabaseText(tr("Не удалось найти строку с указанным ключём!!!"));
        lastErr.setType(QSqlError::UnknownError);
        return false;
    }

    modelData.replace(row, p);
    emit(dataChanged(index(row,0), index(row,columnCount()-1)));
    return true;
}

bool LicenseTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        QVector<QVariant> p = modelData.value(row);
        p[index.column()] = value;
        if(index.column() != cIndex.statusRow)
            if(p.value(p.size()-1).toString().compare("ins") != 0 && p.value(p.size()-1).toString().compare("rem") != 0)
                p[p.size()-1] = "upd";
        modelData.replace(row, p);
        emit(dataChanged(index, index));
        return true;
    }
    return false;
}

bool LicenseTableModel::setRowData(const QModelIndex &index, const QList<QVariant> &rowData)
{
    QVector<QVariant> p = modelData.at(index.row());
    for(int i = 0; i<columnCount();i++)
        p[i] = rowData.value(i);
    modelData.replace(index.row(), p);
    emit(dataChanged(this->index(index.row(),0), this->index(index.row(),columnCount()-1)));
    return true;
}

Qt::ItemFlags LicenseTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool LicenseTableModel::submitAll()
{
    if(modelData.size() <= 0)
        return true;

    bool ok = true;
    QString field, val, queryStr;
    QQueue<QVariant> bindval;
    QSqlQuery query(m_db);
    int lastId;
    beginResetModel();
    for(int i = 0; i<modelData.size();i++){
        QVector<QVariant> p = modelData.value(i);
        if(p.value(p.size()-1).toString().compare("ins") == 0){
            field = "("; val = "(";
            field += "CodOrganization,CodPO,CodTypeLicense,CodStatePO"; val += "?,?,?,?";
            bindval.enqueue(p.value(cIndex.codOrganization));
            bindval.enqueue(p.value(cIndex.codPO));
            bindval.enqueue(p.value(cIndex.codTypeLicense));
            bindval.enqueue(p.value(cIndex.codStatePO));
            if(!p.value(cIndex.regName).toString().isEmpty() && !p.value(cIndex.regName).toString().isNull()){
                field += ","+colTabName.regName; val += ",?";
                bindval.enqueue(p.value(cIndex.regName));
            }
            if(!p.value(cIndex.regKey).toString().isEmpty() && !p.value(cIndex.regKey).toString().isNull()){
                field += ","+colTabName.regKey; val += ",?";
                bindval.enqueue(p.value(cIndex.regKey));
            }
            if(!p.value(cIndex.regMail).toString().isEmpty() && !p.value(cIndex.regMail).toString().isNull()){
                field += ","+colTabName.regMail; val += ",?";
                bindval.enqueue(p.value(cIndex.regMail));
            }
            if(p.value(cIndex.kolLicense).toInt() != 0){
                field += ","+colTabName.kolLicense; val += ",?";
                bindval.enqueue(p.value(cIndex.kolLicense));
            }
            if(!p.value(cIndex.invN).toString().isEmpty() && !p.value(cIndex.invN).toString().isNull()){
                field += ","+colTabName.invN; val += ",?";
                bindval.enqueue(p.value(cIndex.invN));
            }
            if(!p.value(cIndex.versionN).toString().isEmpty() && !p.value(cIndex.versionN).toString().isNull()){
                field += ","+colTabName.versionN; val += ",?";
                bindval.enqueue(p.value(cIndex.versionN));
            }
            if(p.value(cIndex.codProvider).toInt() != 0){
                field += ","+colTabName.codProvider; val += ",?";
                bindval.enqueue(p.value(cIndex.codProvider));
            }
            if(!p.value(cIndex.datePurchase).isNull()){
                field += ","+colTabName.datePurchase; val += ",?";
                bindval.enqueue(p.value(cIndex.datePurchase));
            }
            if(!p.value(cIndex.dateEndLicense).isNull()){
                field += ","+colTabName.dateEndLicense; val += ",?";
                bindval.enqueue(p.value(cIndex.dateEndLicense));
            }
            field += ","+colTabName.price; val += ",?";
            bindval.enqueue(p.value(cIndex.price));
            if(!p.value(cIndex.note).toString().isEmpty() && !p.value(cIndex.note).toString().isNull()){
                field += ","+colTabName.note; val += ",?";
                bindval.enqueue(p.value(cIndex.versionN));
            }
            field += ")"; val += ")";
            queryStr = QString("INSERT INTO %1 %2 VALUES %3").arg(tabName).arg(field).arg(val);
            query.prepare(queryStr);
            while(!bindval.empty()){
                query.addBindValue(bindval.dequeue());
            }
            ok = query.exec();
            if(ok) lastId = query.lastInsertId().toInt();
            else{
                lastErr = query.lastError();
                endResetModel();
                return ok;
            }
            p[cIndex.key] = lastId;
            p[cIndex.rv] = 0;
            p[cIndex.statusRow] = "";
            modelData.replace(i, p);
        }else if(p.value(p.size()-1).toString().compare("rem") == 0){
            ok = query.exec(QString("DELETE FROM %1 WHERE `key` = %2").arg(tabName).arg(p.value(cIndex.key).toInt()));
            if(!ok){
                lastErr = query.lastError();
                endResetModel();
                return ok;
            }
            modelData.removeAt(i);
            i--;
        }else if(p.value(p.size()-1).toString().compare("upd") == 0){
            query.prepare(QString("UPDATE %1 SET CodPO = ?, RegName = ?, RegKey = ?, RegMail = ?, "
                                  "KolLicense = ?, InvN = ?, VersionN = ?, CodProvider = ?, "
                                  "DatePurchase = ?, DateEndLicense = ?, Price = ?, CodTypeLicense = ?, "
                                  "CodStatePO = ?, Note = ?, RV = ? WHERE `key` = %2").arg(tabName).arg(p.value(cIndex.key).toInt()));
            query.addBindValue(p.value(cIndex.codPO));
            query.addBindValue(p.value(cIndex.regName));
            query.addBindValue(p.value(cIndex.regKey));
            query.addBindValue(p.value(cIndex.regMail));
            query.addBindValue(p.value(cIndex.kolLicense));
            query.addBindValue(p.value(cIndex.invN));
            query.addBindValue(p.value(cIndex.versionN));
            query.addBindValue(p.value(cIndex.codProvider));
            query.addBindValue(p.value(cIndex.datePurchase));
            query.addBindValue(p.value(cIndex.dateEndLicense));
            query.addBindValue(p.value(cIndex.price));
            query.addBindValue(p.value(cIndex.codTypeLicense));
            query.addBindValue(p.value(cIndex.codStatePO));
            query.addBindValue(p.value(cIndex.note));
            if(p.value(cIndex.rv).toInt() == 254){
                query.addBindValue(0);
            }else{
                query.addBindValue(p.value(cIndex.rv).toInt()+1);
            }

            ok = query.exec();
            if(!ok){
                lastErr = query.lastError();
                endResetModel();
                return ok;
            }
            if(p.value(cIndex.rv).toInt() == 254){
                p[cIndex.rv] = 0;
            }else{
                p[cIndex.rv] = p.value(cIndex.rv).toInt()+1;
            }
            p[cIndex.statusRow] = "";
            modelData.replace(i, p);
        }
    }
    cacheModelData = modelData;
    endResetModel();
    return ok;
}

void LicenseTableModel::revertAll()
{
    beginResetModel();
    modelData = cacheModelData;
    endResetModel();
}

bool LicenseTableModel::select()
{
    QSqlQuery query(m_db);
    beginResetModel();
    if(filter.isEmpty())
        query.exec(primaryQuery);
    else
        query.exec(QString("%1WHERE %2").arg(primaryQuery).arg(filter));

    if (query.lastError().type() != QSqlError::NoError){
        lastErr = query.lastError();
        return false;
    }
    if(query.size()>0){
        while(query.next())
        {
            QVector<QVariant> rowData;
            for(int i = 0;i<query.record().count();i++){
                rowData.append(query.value(i));
            }
            rowData.append(QVariant());
            modelData.append(rowData);
        }
    }
    cacheModelData = modelData;
    endResetModel();
    return true;
}

void LicenseTableModel::setFilter(const QString& filters)
{
    filter = filters;
}

QSqlError LicenseTableModel::getLastError()
{
    return lastErr;
}

void  LicenseTableModel::clearModel()
{
    beginResetModel();
    modelData.clear();
    endResetModel();
}

QString LicenseTableModel::nameModelTable()
{
    return tabName;
}

QString LicenseTableModel::aliasModelTable()
{
    return aliasTable;
}
