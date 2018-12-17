#include <QDir>
#include <QSqlError>
#include "headers/treeitem.h"
#include "headers/licensemodel.h"

LicenseModel::LicenseModel(QObject *parent) : QAbstractItemModel(parent) {
    filter = "";
    showParent = false;
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
    rootData << "NamePO";
    cIndex.namePO = 0;
    rootData << "key";
    cIndex.key = 1;
    colTabName.key = "key";
    rootData << "CodPO";
    cIndex.codPO = 2;
    colTabName.codPO = "CodPO";
    rootData << "CodOrganization";
    cIndex.codOrganization = 3;
    colTabName.codOrganization = "CodOrganization";
    rootData << "CodTypeLicense";
    cIndex.codTypeLicense = 4;
    colTabName.codTypeLicense = "CodTypeLicense";
    rootData << "CodStatePO";
    cIndex.codStatePO = 5;
    colTabName.codStatePO = "CodStatePO";
    rootData << "NameOrg";
    cIndex.nameOrg = 6;
    rootData << "NameProd";
    cIndex.nameProd = 7;
    rootData << "RegName";
    cIndex.regName = 8;
    colTabName.regName = "RegName";
    rootData << "RegKey";
    cIndex.regKey = 9;
    colTabName.regKey = "RegKey";
    rootData << "RegMail";
    cIndex.regMail = 10;
    colTabName.regMail = "RegMail";
    rootData << "KolLicense";
    cIndex.kolLicense = 11;
    colTabName.kolLicense = "KolLicense";
    rootData << "InvN";
    cIndex.invN = 12;
    colTabName.invN = "InvN";
    rootData << "VersionN";
    cIndex.versionN = 13;
    colTabName.versionN = "VersionN";
    rootData << "CodProvider";
    cIndex.codProvider = 14;
    colTabName.codProvider = "CodProvider";
    rootData << "DatePurchase";
    cIndex.datePurchase = 15;
    colTabName.datePurchase = "DatePurchase";
    rootData << "DateEndLicense";
    cIndex.dateEndLicense = 16;
    colTabName.dateEndLicense = "DateEndLicense";
    rootData << "Price";
    cIndex.price = 17;
    colTabName.price = "Price";
    rootData << "NameLic";
    cIndex.nameLic = 18;
    rootData << "NameState";
    cIndex.nameState = 19;
    rootData << "Note";
    cIndex.note = 20;
    colTabName.note = "Note";
    rootData << "Ico";
    cIndex.ico = 21;
    rootData << "RV";
    cIndex.rv = 22;
    colTabName.rv = "RV";
    rootData << "isLicense";
    cIndex.isLicense = 23;
    rootItemData = new TreeItem(rootData);
}
LicenseModel::~LicenseModel(){
    delete rootItemData;
}
QModelIndex LicenseModel::index(int row, int column,
                             const QModelIndex& parent) const {
    if (!rootItemData || row < 0 || column < 0)
        return QModelIndex();
    TreeItem *parentItem = itemDataFromIndex(parent);
    TreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();
    return createIndex(row, column, childItem);
}
bool LicenseModel::hasChildren(const QModelIndex& parent) const {
    TreeItem *parentItem = itemDataFromIndex(parent);
    if (!parentItem) return false;
    else return (parentItem->childCount() > 0);
}
QModelIndex LicenseModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
        return QModelIndex();
    TreeItem *childItem = itemDataFromIndex(child);
    TreeItem *parentItem = childItem->parent();
    if (parentItem == rootItemData)
        return QModelIndex();
    return createIndex(parentItem->childNumber(), 0, parentItem);
}
int LicenseModel::rowCount(const QModelIndex& index) const {
    if (index.column() > 0)
        return 0;
    TreeItem *item = itemDataFromIndex(index);
    if ( item )
        return item->childCount();
    else
        return 0;
}
int LicenseModel::columnCount(const QModelIndex& /* index */ ) const {
    return rootItemData->columnCount();
}
QVariant LicenseModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    TreeItem *item = itemDataFromIndex(index);
    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            {
                if (!item) return QVariant();
                else return item->data(index.column());
            }
        case Qt::TextAlignmentRole:
            return int(Qt::AlignLeft);
        case Qt::DecorationRole: {
            if (index.column() == 0 && item->data(cIndex.isLicense).toInt() == 1){
                if(item->data(cIndex.ico).toString().isEmpty() || item->data(cIndex.ico).toString().isNull()){
                    return QIcon(QDir::currentPath()+"/ico/typepoico/Default.png");
                }else{
                    return QIcon(QDir::currentPath()+"/ico/typepoico/"+item->data(cIndex.ico).toString());
                }
            }
            if (index.column() == 0 && item->data(cIndex.isLicense).toInt() == 0){
                return QIcon(QDir::currentPath()+"/ico/typedeviceico/"+item->data(cIndex.ico).toString());
            }
        }
    }
    return QVariant();
}
void LicenseModel::setRootItemData(TreeItem *item){
    beginResetModel();
    delete rootItemData;
    rootItemData = item;
    endResetModel();
}
TreeItem* LicenseModel::itemDataFromIndex(const QModelIndex& index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItemData;
}
Qt::ItemFlags LicenseModel::flags(const QModelIndex& /*index*/) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant LicenseModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItemData->data(section);
    return QVariant();
}
bool LicenseModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;
    bool result = rootItemData->setData(section, value);
    if (result)
        emit headerDataChanged(orientation, section, section);
    return result;
}
bool LicenseModel::setData(const QModelIndex& index,
                        const QVariant& value, int role){
    if ( role != Qt::EditRole )
        return false;
    bool ok;
    QSqlQuery query;
    int col = index.column();
    TreeItem *item = itemDataFromIndex(index);
    if(col != 1){
        ok = item->setData(col,value);
        if(ok){
            if(value.isNull()){
                if(col >= cIndex.namePO && col <= cIndex.codPO)
                    ok = false;
                else{
                    query.prepare(QString("UPDATE %1 SET %3 = NULL, RV = ? WHERE CodPO = %2")
                                    .arg(tabName)
                                    .arg(item->data(cIndex.key).toInt())
                                    .arg(rootItemData->data(col).toString()));
                    if(item->data(cIndex.rv).toInt() == 254)
                        query.addBindValue(0);
                    else
                        query.addBindValue(item->data(cIndex.rv).toInt()+1);
                    ok = query.exec();
                    if(!ok) lastErr = query.lastError();
                }
            }else{
                if(col == cIndex.key)
                    ok = false;
                else{
                    query.prepare(QString("UPDATE %1 SET %3 = ?, RV = ? WHERE CodPO = %2")
                                    .arg(tabName)
                                    .arg(item->data(cIndex.key).toInt())
                                    .arg(rootData.value(col).toString()));
                    query.addBindValue(value.toString());
                    if(item->data(cIndex.rv).toInt() == 254)
                        query.addBindValue(0);
                    else
                        query.addBindValue(item->data(cIndex.rv).toInt()+1);
                    ok = query.exec();
                    if(!ok) lastErr = query.lastError();
                }
            }
        }
    }else
        ok = false;
    if (ok)
        emit dataChanged(index, index);
    return ok;
}
bool LicenseModel::setDataWithOutSQL(const QModelIndex& index,
                        const QVariant& value, int role){
    if ( role != Qt::EditRole )
        return false;
    bool ok = true;
    int col = index.column();
    TreeItem *item = itemDataFromIndex(index);
    ok = item->setData(col,value);
    if (ok)
        emit dataChanged(index, index);
    return ok;
}
void LicenseModel::setFilter(const QString& filters)
{
    filter = filters;
}
void LicenseModel::select()
{
    setRootItemData(InitTree());
}
bool LicenseModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;
    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItemData->insertColumns(position, columns);
    endInsertColumns();
    return success;
}
bool LicenseModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;
    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItemData->removeColumns(position, columns);
    endRemoveColumns();
    if (rootItemData->columnCount() == 0)
        removeRows(0, rowCount());
    return success;
}
bool LicenseModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool ok;
    TreeItem *parentItem = itemDataFromIndex(parent);
    beginInsertRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        ok = parentItem->insertChildren(row, 1, rootItemData->columnCount());
        if(!ok){ endInsertRows(); return ok;}
    }
    endInsertRows();
    return ok;
}

bool LicenseModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(itemDataFromIndex(parent)->child(row)->data(cIndex.key).toInt() == 0)
         return false;
    TreeItem *parentItem = itemDataFromIndex(parent);
    if (row < 0 || row > parentItem->childCount())
        return false;
    bool ok = true;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i){
        ok = parentItem->removeChildren(row, 1);
        if(!ok){endRemoveRows(); return ok;}
    }
    endRemoveRows();
    return ok;
}
bool LicenseModel::updateRow(int row, const QModelIndex &parent)
{
    bool ok;
    QSqlQuery query;
    int id;
    TreeItem *parentItem = itemDataFromIndex(parent);
    id = parentItem->child(row)->data(cIndex.key).toInt();
    ok = query.exec(QString("%1WHERE `key` = %2").arg(primaryQuery).arg(id));
    if(!ok) lastErr = query.lastError();
    else{
        query.next();
        for(int i = 0; i<query.record().count(); i++)
            parentItem->child(row)->setData(i,query.value(i));
    }
    if (ok)
        emit dataChanged(parent.child(row,0), parent.child(row,0));
    return ok;
}
bool LicenseModel::sqlInsertRow(int row, int codOrganization, int codPO, int codTypeLicense, int codStatePO,
                           QMap<QString, QVariant> colValue, const QModelIndex &parent)
{
    bool ok;
    QString field, val, queryStr;
    QQueue<QVariant> bindval;
    QSqlQuery query;
    int lastId;
    TreeItem *parentItem = itemDataFromIndex(parent);

    field = "("; val = "(";
    field += "CodOrganization,CodPO,CodTypeLicense,CodStatePO"; val += "?,?,?,?";
    bindval.enqueue(codOrganization);
    bindval.enqueue(codPO);
    bindval.enqueue(codTypeLicense);
    bindval.enqueue(codStatePO);

    if(!colValue.isEmpty()){
        QMapIterator<QString, QVariant> i(colValue);
        while (i.hasNext()) {
            i.next();
            field += ","+i.key(); val += ",?";
            bindval.enqueue(i.value());
        }
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
        return ok;
    }
    ok = query.exec(QString("%1WHERE %3.`key` = %2").arg(primaryQuery).arg(lastId).arg(aliasTable));
    if(ok) ok = insertRow(row,parent);
    else lastErr = query.lastError();
    if(ok){
        query.next();
        for(int i = 0; i<query.record().count(); i++)
            parentItem->child(row)->setData(i,query.value(i));
    }
    return ok;
}

bool LicenseModel::sqlRemoveRow(int row, const QModelIndex &parent)
{
    bool ok;
    TreeItem *parentItem = itemDataFromIndex(parent);
    ok = queryDelItem(parentItem->child(row));
    if(ok) ok = removeRow(row,parent);
    return ok;
}
bool LicenseModel::sqlUpdateRow(int row, QMap<QString, QVariant> setData, const QModelIndex &parent)
{
    if(setData.isEmpty()) return false;

    QString queryStr;
    QSqlQuery query;
    bool ok = false;
    QQueue<QVariant> bindval;
    TreeItem *parentItem = itemDataFromIndex(parent);
    QMapIterator<QString, QVariant> i(setData);
    queryStr = QString("UPDATE %1 SET ").arg(tabName);
    while (i.hasNext()) {
        i.next();
        queryStr += QString("%1 = ?,").arg(i.key());
        bindval.enqueue(i.value());
    }
    queryStr += "RV = ?";
    if(setData.value(colTabName.rv).toInt() == 254)
        bindval.enqueue(0);
    else
        bindval.enqueue(setData.value(colTabName.rv).toInt()+1);
    queryStr += QString(" WHERE `key` = %1").arg(parentItem->child(row)->data(cIndex.key).toInt());
    query.prepare(queryStr);
    while(!bindval.empty())
        query.addBindValue(bindval.dequeue());
    ok = query.exec();
    if(!ok) lastErr = query.lastError();
    else{
        ok = updateRow(row,parent);
        if(ok) emit dataChanged(parent.child(row,0), parent.child(row,0));
    }
    return ok;
}
bool LicenseModel::queryDelItem(TreeItem* item)
{
    QSqlQuery query;
    if(!query.exec(QString("DELETE FROM %1 WHERE `key` = %2").arg(tabName).arg(item->data(cIndex.key).toInt()))){
        lastErr = query.lastError();
        return false;
    }
    return true;
}

QModelIndex LicenseModel::indexFromItemData(TreeItem* itemData)
{
    int itemrow = itemData->childNumber();
    return createIndex(itemrow,0,itemData);
}

QModelIndex LicenseModel::moveItem(int movedId, int newParentId)
{
    QModelIndex indexFrom, indexTo;
    QSqlQuery query;
    TreeItem *newParent;
    TreeItem *item;
    newParent = search(rootItemData,newParentId);
    item = search(rootItemData,movedId);
    indexTo = indexFromItemData(newParent);
    indexFrom = indexFromItemData(item);
    copyItem(indexFrom,indexTo,newParent->childCount());
    removeRow(indexFrom.row(),indexFrom.parent());
    item = newParent->child(newParent->childCount()-1);
    item->setData(2,newParent->data(1));
    query.exec(QString("UPDATE %1 SET CodGroupPO = %2 WHERE CodPO = %3").arg(tabName).arg(newParentId).arg(movedId));
    return index(newParent->childCount()-1,0,indexTo);
}

void LicenseModel::copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position)
{
    TreeItem *item;
    insertRow(position,indexTo);
    item = itemDataFromIndex(index(position,0,indexTo));
    for(int i = 0;i<columnCount(indexFrom);i++)
        item->setData(i,data(index(indexFrom.row(),i,indexFrom.parent())));
    if(rowCount(indexFrom)>0){
        for(int i = 0;i<rowCount(indexFrom);i++){
            if(rowCount(indexFrom.child(i,0))>0)
                copyItem(indexFrom.child(i,0),index(position,0,indexTo),rowCount(index(position,0,indexTo)));
            else{
                insertRow(rowCount(indexTo.child(position,0)),indexTo.child(position,0));
                item = itemDataFromIndex(index(rowCount(indexTo.child(position,0))-1,0,indexTo.child(position,0)));
                for(int j = 0;j<columnCount(indexFrom.child(i,0));j++){
                    item->setData(j,data(index(indexFrom.child(i,0).row(),j,indexFrom)));
                }
            }
        }
    }
}

TreeItem* LicenseModel::search(TreeItem* note, int id)
{
    int i = 0;
    TreeItem* childs;
    TreeItem* temp;
    while(i<note->childCount()){
        childs = note->child(i);
        if(childs->data(cIndex.key).toInt() == id)
            return childs;
        else{
            if(childs->childCount()>0){
                temp = search(childs, id);
                if(temp->data(cIndex.key).toInt() != childs->data(cIndex.key).toInt())
                    return temp;
            }
        }
        i++;
    }
    return note;
}
QModelIndex LicenseModel::findData(int idData)
{
    TreeItem* item = search(rootItemData,idData);
    if(item == rootItemData)
        return QModelIndex();
    else
        return indexFromItemData(item);
}
TreeItem* LicenseModel::InitTree(){
    QSqlQuery query;
    TreeItem *all = 0;

    if(filter.isEmpty())
        query.exec(primaryQuery);
    else
        query.exec(QString("%1WHERE %2").arg(primaryQuery).arg(filter));

    if (query.lastError().type() != QSqlError::NoError){
        qDebug()<<query.lastError().text();
        return all;
    }

    all = new TreeItem(rootData);
    if(query.size()>0){
        while(query.next())
        {
            int i;
            all->insertChildren(all->childCount(),1,all->columnCount());
            for(i = 0;i<query.record().count();i++)
                all->child(all->childCount()-1)->setData(i,query.value(i));
            all->child(all->childCount()-1)->setData(i,1);
        }
        if (showParent){
            QSqlQuery queryChildren;
            bool ok;
            for(int i = 0; i<all->childCount();i++){
                TreeItem *child = all->child(i);
                int licenseKey = child->data(cIndex.key).toInt();
                ok = queryChildren.exec(
                            QString("SELECT device.name, device.id, device.NetworkName, typedevice.IconPath, device.InventoryN, device.CodOrganization, departments.Name FROM device "
                                    "INNER JOIN typedevice ON device.CodTypeDevice = typedevice.CodTypeDevice "
                                    "LEFT OUTER JOIN departments ON device.CodOrganization = departments.id "
                                    "WHERE device.id IN "
                                    "(SELECT p.id FROM device n, devtree t, device p "
                                    "WHERE n.id IN "
                                    "(SELECT CodDevice FROM licenseanddevice WHERE CodLicense = %1) "
                                    "AND n.id = t.id AND t.parent_id = p.id) "
                                    "ORDER BY device.type DESC;"
                                    ).arg(licenseKey));
                if(ok){
                    if(queryChildren.size() > 0){
                        while(queryChildren.next()){
                            child->insertChildren(child->childCount(),1,child->columnCount());
                            child->child(child->childCount()-1)->setData(cIndex.namePO,queryChildren.value(2).toString() + " / " + queryChildren.value(0).toString());
                            child->child(child->childCount()-1)->setData(cIndex.key,queryChildren.value(1));
                            child->child(child->childCount()-1)->setData(cIndex.ico,queryChildren.value(3));
                            child->child(child->childCount()-1)->setData(cIndex.invN,queryChildren.value(4));
                            child->child(child->childCount()-1)->setData(cIndex.codOrganization,queryChildren.value(5));
                            child->child(child->childCount()-1)->setData(cIndex.nameOrg,queryChildren.value(6));
                            child->child(child->childCount()-1)->setData(cIndex.isLicense,0);
                        }
                    }
                }else{
                    lastErr.setDatabaseText(tr("При получении спика устройств лицензии %1 с кодом %2 возникла ошибка:\n%3")
                                              .arg(child->data(cIndex.namePO).toString())
                                              .arg(licenseKey)
                                              .arg(queryChildren.lastError().text()));
                    lastErr.setType(QSqlError::UnknownError);
                    return all;
                }
            }
        }
    }
    return all;
}
QSqlError LicenseModel::lastError()
{
    return lastErr;
}
void LicenseModel::setShowParent(bool sp)
{
    showParent = sp;
}
QString LicenseModel::aliasModelTable()
{
    return aliasTable;
}
QString LicenseModel::nameModelTable()
{
    return tabName;
}
