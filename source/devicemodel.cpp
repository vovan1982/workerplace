#include <QSqlError>
#include <QDir>
#include "headers/devicemodel.h"
#include "headers/treeitem.h"
#include "headers/populatedevicethread.h"
#include "headers/devicethreadworker.h"

DeviceModel::DeviceModel(const QString &dbConnectionName, const QSqlDatabase &connectionData, QObject *parent, bool inThread, bool isEmpty):
    QAbstractItemModel(parent),
    m_inThread(inThread),
    m_isEmpty(isEmpty)
{
    queueUpdate = false;
    filter = "";
    tabName = "device";
    aliasTable = "dev";
    treeTable = "devtree";
    primaryQuery = QString("SELECT typedevice.Name  AS TypeDevName"
                           ", %2.id"
                           ", %2.parent_id"
                           ", %2.CodTypeDevice"
                           ", %2.CodOrganization"
                           ", %2.CodWorkerPlace"
                           ", departments.Name AS OrgName"
                           ", workerplace.Name AS WpName"
                           ", %2.Name"
                           ", %2.NetworkName"
                           ", %2.CodDomainWg"
                           ", domainwg.Name AS DomainWgName"
                           ", %2.InventoryN"
                           ", %2.SerialN"
                           ", %2.CodProducer"
                           ", producer.Name AS ProducerName"
                           ", %2.CodProvider"
                           ", provider.Name AS ProviderName"
                           ", %2.DatePurchase"
                           ", %2.DatePosting"
                           ", %2.EndGuarantee"
                           ", %2.Price"
                           ", %2.CodState"
                           ", statedev.Name AS StateName"
                           ", %2.Note"
                           ", typedevice.Type"
                           ", typedevice.IconPath"
                           ", %2.RV"
                           ", %2.DetailDescription"
                           " FROM "
                           "%1 %2 "
                           "LEFT OUTER JOIN domainwg "
                           "ON %2.CodDomainWg = domainwg.CodDomainWg "
                           "LEFT OUTER JOIN statedev "
                           "ON %2.CodState = statedev.CodState "
                           "LEFT OUTER JOIN typedevice "
                           "ON %2.CodTypeDevice = typedevice.CodTypeDevice "
                           "LEFT OUTER JOIN departments "
                           "ON %2.CodOrganization = departments.id "
                           "LEFT OUTER JOIN workerplace "
                           "ON %2.CodWorkerPlace = workerplace.CodWorkerPlace "
                           "LEFT OUTER JOIN provider "
                           "ON %2.CodProvider = provider.CodProvider "
                           "LEFT OUTER JOIN producer "
                           "ON %2.CodProducer = producer.CodProducer ").arg(tabName).arg(aliasTable);
    QVector<QVariant> rootData;
    rootData << "TypeDevName";
    cIndex.typeDevName = 0;
    rootData << "Id";
    cIndex.id = 1;
    colTabName.id = "id";
    rootData << "Parent_id";
    cIndex.parent_id = 2;
    colTabName.parent_id = "parent_id";
    rootData << "CodTypeDevice";
    cIndex.codTypeDevice = 3;
    colTabName.codTypeDevice = "CodTypeDevice";
    rootData << "CodOrganization";
    cIndex.codOrganization = 4;
    colTabName.codOrganization = "CodOrganization";
    rootData << "CodWorkerPlace";
    cIndex.codWorkerPlace = 5;
    colTabName.codWorkerPlace = "CodWorkerPlace";
    rootData << "OrgName";
    cIndex.orgName = 6;
    rootData << "WpName";
    cIndex.wpName = 7;
    rootData << "Name";
    cIndex.name = 8;
    colTabName.name = "name";
    rootData << "NetworkName";
    cIndex.networkName = 9;
    colTabName.networkName = "networkName";
    rootData << "CodDomainWg";
    cIndex.codDomainWg = 10;
    colTabName.codDomainWg = "CodDomainWg";
    rootData << "DomainWgName";
    cIndex.domainWgName = 11;
    rootData << "InventoryN";
    cIndex.inventoryN = 12;
    colTabName.inventoryN = "InventoryN";
    rootData << "SerialN";
    cIndex.serialN = 13;
    colTabName.serialN = "SerialN";
    rootData << "CodProducer";
    cIndex.codProducer = 14;
    colTabName.codProducer = "CodProducer";
    rootData << "ProducerName";
    cIndex.producerName = 15;
    rootData << "CodProvider";
    cIndex.codProvider = 16;
    colTabName.codProvider = "CodProvider";
    rootData << "ProviderName";
    cIndex.providerName = 17;
    rootData << "DatePurchase";
    cIndex.datePurchase = 18;
    colTabName.datePurchase = "DatePurchase";
    rootData << "DatePosting";
    cIndex.datePosting = 19;
    colTabName.datePosting = "DatePosting";
    rootData << "EndGuarantee";
    cIndex.endGuarantee = 20;
    colTabName.endGuarantee = "EndGuarantee";
    rootData << "Price";
    cIndex.price = 21;
    colTabName.price = "Price";
    rootData << "CodState";
    cIndex.codState = 22;
    colTabName.codState = "CodState";
    rootData << "StateName";
    cIndex.stateName = 23;
    rootData << "Note";
    cIndex.note = 24;
    colTabName.note = "Note";
    rootData << "Type";
    cIndex.type = 25;
    rootData << "IconPath";
    cIndex.iconPath = 26;
    rootData << "RV";
    cIndex.rv = 27;
    colTabName.rv = "RV";
    rootData << "DetailDescription";
    cIndex.detailDescription = 28;
    colTabName.detailDescription = "DetailDescription";
    rootItemData = new TreeItem(rootData);
    if(!isEmpty){
        if(inThread){
            m_populateDeviceThread = new PopulateDeviceThread(dbConnectionName,connectionData);
            connect(m_populateDeviceThread,SIGNAL(result(TreeItem*)),this,SLOT(setNewTree(TreeItem*)));
            connect(m_populateDeviceThread,SIGNAL(ready(bool)),this,SLOT(setReadyThread(bool)));
            connect(m_populateDeviceThread,SIGNAL(lastErrors(QSqlError)),this,SLOT(setLastError(QSqlError)));
            m_populateDeviceThread->start();
        }else{
            m_deviceThreadWorker = new DeviceThreadWorker(dbConnectionName,connectionData);
            connect(m_deviceThreadWorker,SIGNAL(result(TreeItem*)),this,SLOT(setNewTree(TreeItem*)));
            connect(m_deviceThreadWorker,SIGNAL(lastErrors(QSqlError)),this,SLOT(setLastError(QSqlError)));
            select();
        }
    }
}

DeviceModel::~DeviceModel(){
    delete rootItemData;
    if(!m_isEmpty){
        if(m_inThread){
            m_populateDeviceThread->quit();
            m_populateDeviceThread->wait();
            m_populateDeviceThread->~PopulateDeviceThread();
        }
        else
            m_deviceThreadWorker->~DeviceThreadWorker();
    }
}

QModelIndex DeviceModel::index(int row, int column,
                             const QModelIndex& parent) const {
    if (!rootItemData || row < 0 || column < 0)
        return QModelIndex();
    TreeItem *parentItem = itemDataFromIndex(parent);
    TreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();
    return createIndex(row, column, childItem);
}

bool DeviceModel::hasChildren(const QModelIndex& parent) const {
    TreeItem *parentItem = itemDataFromIndex(parent);
    if (!parentItem) return false;
    else return (parentItem->childCount() > 0);
//    {
//        if(parent == QModelIndex())
//            return (parentItem->childCount() > 0);
//        QSqlQuery query;
//        bool ok;
//        ok = query.exec(QString("SELECT count(c.id) FROM %2 n, %3 t, %2 c WHERE "
//                                "n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND t.level > 0")
//                        .arg(parentItem->data(cIndex.id).toInt())
//                        .arg(tabName)
//                        .arg(treeTable));
//        if (!ok) {/*lastErr = query.lastError();*/ return false;}
//        query.next();
//        return (query.value(0).toInt() > 0);
//    }
}

QModelIndex DeviceModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
        return QModelIndex();

    TreeItem *childItem = itemDataFromIndex(child);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItemData)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int DeviceModel::rowCount(const QModelIndex& index) const {

    if (index.column() > 0)
        return 0;
    TreeItem *item = itemDataFromIndex(index);
    if ( item )
        return item->childCount();
    else
        return 0;
}

int DeviceModel::columnCount(const QModelIndex& /* index */ ) const {
    return rootItemData->columnCount();
}

//bool DeviceModel::canFetchMore(const QModelIndex & parent) const {
//    if (parent.column() > 0) return false;
//    QSqlQuery query;
//    bool ok;
//    TreeItem *item = itemDataFromIndex(parent);
//    if ( item ){
//        ok = query.exec(QString("SELECT count(c.id) FROM %2 n, %3 t, %2 c WHERE "
//                                "n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND t.level > 0")
//                        .arg(item->data(cIndex.id).toInt())
//                        .arg(tabName)
//                        .arg(treeTable));
//        if (!ok) {/*lastErr = query.lastError();*/ return false;}
//        query.next();
//        return (item->childCount() < query.value(0).toInt());
//    }else
//        return false;
//}

//void DeviceModel::fetchMore(const QModelIndex & parent)
//{
//    QSqlQuery query;
//    bool ok;
//    TreeItem *parentItem = itemDataFromIndex(parent);
//    ok = query.exec(QString("%1"
//            "WHERE %5.id IN "
//            "(SELECT c.id FROM %3 n, %4 t, %3 c WHERE "
//            "n.id = %2 AND n.id = t.parent_id AND t.id = c.id AND t.level > 0)"
//            "ORDER BY "
//            "parent_id"
//            ", id")
//            .arg(primaryQuery)
//            .arg(parentItem->data(cIndex.id).toInt())
//            .arg(tabName)
//            .arg(treeTable)
//            .arg(aliasTable));
//    if (!ok) {lastErr = query.lastError(); return;}
//    if(query.size()>0){
//        beginInsertRows(parent, parentItem->childCount(), query.record().count());
//        while(query.next()){
//            ok = parentItem->insertChildren(parentItem->childCount(),1,parentItem->columnCount());
//            if(!ok){ endInsertRows(); return;}
//            for(int i = 0;i<query.record().count();i++)
//                parentItem->child(parentItem->childCount()-1)->setData(i,query.value(i));
//        }
//        endInsertRows();
//    }else
//     return;
//    layoutChanged();
//}

QVariant DeviceModel::data(const QModelIndex& index, int role) const {
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
            if (index.column() == 0 && !item->data(cIndex.iconPath).toString().isNull() && !item->data(cIndex.iconPath).toString().isEmpty())
                return QIcon(QDir::currentPath()+"/ico/typedeviceico/"+item->data(cIndex.iconPath).toString());
        }
    }
    return QVariant();
}

void DeviceModel::setRootItemData(TreeItem *item){
    if(item != 0){
        beginResetModel();
        delete rootItemData;
        rootItemData = item;
        endResetModel();
    }
    emit newTreeIsSet();
}

TreeItem* DeviceModel::itemDataFromIndex(const QModelIndex& index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItemData;
}

Qt::ItemFlags DeviceModel::flags(const QModelIndex& index) const {
    if(index.column() > cIndex.id && index.column() != cIndex.orgName && index.column() != cIndex.wpName && index.column() != cIndex.producerName &&
            index.column() != cIndex.providerName && index.column() != cIndex.stateName && index.column() != cIndex.iconPath)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant DeviceModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItemData->data(section);

    return QVariant();
}

bool DeviceModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItemData->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

bool DeviceModel::setData(const QModelIndex& index,
                        const QVariant& value, int role){
    if ( role != Qt::EditRole )
        return false;
    bool ok;
    QSqlQuery query;
    int col = index.column();
    TreeItem *item = itemDataFromIndex(index);
    if(col > cIndex.id && col != cIndex.orgName && col != cIndex.wpName && col != cIndex.producerName &&
            col != cIndex.providerName && col != cIndex.stateName && col != cIndex.iconPath){
        ok = item->setData(col,value);
        if(ok){
            if(value.isNull()){
                if((col > cIndex.id && col < cIndex.orgName) || col == cIndex.name || col == cIndex.codState || col == cIndex.type)
                    ok = false;
                else{
                    ok = query.exec(QString("UPDATE %1 SET %3 = NULL WHERE id = %2")
                                    .arg(tabName)
                                    .arg(item->data(cIndex.id).toInt())
                                    .arg(rootItemData->data(col).toString()));
                    if(!ok) lastErr = query.lastError();
                }
            }else{
                ok = query.exec(QString("UPDATE %1 SET %4 = '%2' WHERE id = %3")
                                .arg(tabName)
                                .arg(value.toString())
                                .arg(item->data(cIndex.id).toInt())
                                .arg(rootItemData->data(col).toString()));
                if(!ok) lastErr = query.lastError();
            }
        }
    }else
        ok = false;
    if (ok)
        emit dataChanged(index, index);
    return ok;
}

bool DeviceModel::setDataWithOutSQL(const QModelIndex& index,
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

void DeviceModel::setFilter(const QString& filters)
{
    filter = filters;
}

void DeviceModel::initEmpty()
{
    QVector<QVariant> rootData;
    for(int i=0; i<rootItemData->data().size(); i++)
        rootData << rootItemData->data(i);
    TreeItem *emptyModel = new TreeItem(rootData);
    setNewTree(emptyModel);
}

void DeviceModel::select()
{
    queueUpdate = true;
    QMap<QString,QString> forQuery;
    QVector<QVariant> rootData;
    forQuery["primaryQuery"] = primaryQuery;
    forQuery["filter"] = filter;
    forQuery["tabName"] = tabName;
    forQuery["aliasTable"] = aliasTable;
    forQuery["columnId"] = QString("%1").arg(cIndex.id);
    forQuery["columnParent_Id"] = QString("%1").arg(cIndex.parent_id);
    for(int i=0; i<rootItemData->data().size(); i++)
        rootData << rootItemData->data(i);
    if(m_inThread){
        if(threadIsReady)
            m_populateDeviceThread->selectData(forQuery,rootData);
    }else{
        m_deviceThreadWorker->createDeviceTree(forQuery,rootData);
    }

}

void DeviceModel::setNewTree(TreeItem* deviceTree)
{
    setRootItemData(deviceTree);
}

void DeviceModel::setReadyThread(bool isReady)
{
    threadIsReady = isReady;
    if(threadIsReady){
        if(queueUpdate){
            QMap<QString,QString> forQuery;
            QVector<QVariant> rootData;
            forQuery["primaryQuery"] = primaryQuery;
            forQuery["filter"] = filter;
            forQuery["tabName"] = tabName;
            forQuery["aliasTable"] = aliasTable;
            forQuery["columnId"] = QString("%1").arg(cIndex.id);
            forQuery["columnParent_Id"] = QString("%1").arg(cIndex.parent_id);
            for(int i=0; i<rootItemData->data().size(); i++)
                rootData << rootItemData->data(i);
            m_populateDeviceThread->selectData(forQuery,rootData);
        }
    }
}

bool DeviceModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItemData->insertColumns(position, columns);
    endInsertColumns();

    return success;
}
bool DeviceModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItemData->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItemData->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}
bool DeviceModel::insertRows(int row, int count, const QModelIndex &parent)
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

bool DeviceModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(itemDataFromIndex(parent)->child(row)->data(cIndex.id).toInt() == 0)
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
bool DeviceModel::updateRow(int row, const QModelIndex &parent)
{
    bool ok;
    QSqlQuery query;
    int id;
    TreeItem *parentItem = itemDataFromIndex(parent);
    id = parentItem->child(row)->data(cIndex.id).toInt();
    ok = query.exec(QString("%1WHERE %3.id = %2").arg(primaryQuery).arg(id).arg(aliasTable));
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

bool DeviceModel::sqlInsertRow(int row, int codOrganization, int codWorkerPlace, int codTypeDevice, const QString &name,
                               int codState, int type, QMap<QString,QVariant> colValue,const QModelIndex &parent)
{
    bool ok;
    QString field, val, queryStr;
    QQueue<QVariant> bindval;
    QSqlQuery query;
    int parentId, lastId;
    TreeItem *parentItem = itemDataFromIndex(parent);
    if(type == 1)
        parentId = 0;
    else if(parentItem == rootItemData)
        parentId = 0;
    else
        parentId = parentItem->data(cIndex.id).toInt();
    field = "("; val = "(";
    field += "parent_id,CodOrganization,CodWorkerPlace,CodTypeDevice,name,CodState"; val += "?,?,?,?,?,?";
    bindval.enqueue(parentId);
    bindval.enqueue(codOrganization);
    bindval.enqueue(codWorkerPlace);
    bindval.enqueue(codTypeDevice);
    bindval.enqueue(name);
    bindval.enqueue(codState);

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
    ok = query.exec(QString("%1WHERE %3.id = %2").arg(primaryQuery).arg(lastId).arg(aliasTable));
    if(ok) ok = insertRow(row,parent);
    else lastErr = query.lastError();
    if(ok){
        query.next();
        for(int i = 0; i<query.record().count(); i++)
            parentItem->child(row)->setData(i,query.value(i));
    }
    return ok;
}

bool DeviceModel::sqlRemoveRow(int row, const QModelIndex &parent)
{
    bool ok;
    TreeItem *parentItem = itemDataFromIndex(parent);
    ok = queryDelItem(parentItem->child(row));
    if(ok) ok = removeRow(row,parent);
    return ok;
}
bool DeviceModel::sqlUpdateRow(int row, QMap<QString,QVariant> setData, const QModelIndex &parent)
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
    queryStr += QString(" WHERE id = %1").arg(parentItem->child(row)->data(cIndex.id).toInt());
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

bool DeviceModel::queryDelItem(TreeItem* item)
{
    QSqlQuery query;
    if(item->childCount()>0){
        if(!query.exec(QString("DELETE FROM %1 WHERE id IN (SELECT c.id FROM (SELECT * FROM %1) n, (SELECT * FROM %3) t, (SELECT * FROM %1) c "
                               "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id);")
                       .arg(tabName)
                       .arg(item->data(cIndex.id).toInt())
                       .arg(treeTable))){
            qDebug()<<query.lastError().text();
            return false;
        }
    }else{
        if(!query.exec(QString("DELETE FROM %1 WHERE id = %2").arg(tabName).arg(item->data(cIndex.id).toInt()))){
            qDebug()<<query.lastError().text();
            return false;
        }
    }
    return true;
}

QModelIndex DeviceModel::indexFromItemData(TreeItem* itemData)
{
    int itemrow = itemData->childNumber();
    return createIndex(itemrow,0,itemData);
}

QModelIndex DeviceModel::moveItem(int movedId, int newParentId)
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
    query.exec(QString("UPDATE %1 SET parent_id = %2 WHERE id = %3").arg(tabName).arg(newParentId).arg(movedId));
    return index(newParent->childCount()-1,0,indexTo);
}

void DeviceModel::copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position)
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

TreeItem* DeviceModel::search(TreeItem* note, int id)
{
    int i = 0;
    TreeItem* childs;
    TreeItem* temp;
    while(i<note->childCount()){
        childs = note->child(i);
        if(childs->data(cIndex.id).toInt() == id)
            return childs;
        else{
            if(childs->childCount()>0){
                temp = search(childs, id);
                if(temp->data(cIndex.id).toInt() != childs->data(cIndex.id).toInt())
                    return temp;
            }
        }
        i++;
    }
    return note;
}

//TreeItem* DeviceModel::searchInRootNode(TreeItem* node, int id)
//{
//    int i = 0;
//    TreeItem* childs;
//    while(i < node->childCount()){
//        childs = node->child(i);
//        if(childs->data(cIndex.id).toInt() == id)
//            return childs;
//        i++;
//    }
//    return node;
//}

QModelIndex DeviceModel::findData(int idData)
{
    TreeItem* item = search(rootItemData,idData);
    if(item == rootItemData)
        return QModelIndex();
    else
        return indexFromItemData(item);
}
//TreeItem* DeviceModel::InitTree(){
//    QSqlQuery query, minParentQuery;
//    int minParent;
//    TreeItem *all = 0;
//    if(filter.isEmpty()){
//        query.exec("BEGIN;");
//        query.exec(QString("%1"
//                           "ORDER BY "
//                           "typedevice.Type DESC, parent_id; "
//                           "COMMIT;").arg(primaryQuery));
//        minParentQuery.exec(QString("SELECT MIN(parent_id) FROM %1").arg(tabName));
//    }else{
//        query.exec("BEGIN;");
//        query.exec(QString("%1"
//                           "WHERE %2 "
//                           "ORDER BY "
//                           "typedevice.Type DESC, parent_id; "
//                           "COMMIT;").arg(primaryQuery).arg(filter));
//        minParentQuery.exec(QString("SELECT MIN(parent_id) FROM %1 %3 WHERE %2").arg(tabName).arg(filter).arg(aliasTable));
//    }
//    if (query.lastError().type() != QSqlError::NoError){
//        lastErr = query.lastError();
//        return all;
//    }
//    if (minParentQuery.lastError().type() != QSqlError::NoError)
//        qDebug()<<minParentQuery.lastError().text();
//    TreeItem *note;
//    QVector<QVariant> rootData;
//    QList<QVariant> buffer;
//    int f = 0, n = 0;
//    for(int i=0; i<rootItemData->data().size(); i++)
//        rootData << rootItemData->data(i);
//    all = new TreeItem(rootData);
//    if(query.size()>0){
//        minParentQuery.next();
//        minParent = minParentQuery.value(0).toInt();
//        while(query.next())
//        {
//            if(query.value(cIndex.parent_id).toInt() == minParent){
//                all->insertChildren(all->childCount(),1,all->columnCount());
//                for(int i = 0;i<query.record().count();i++)
//                    all->child(all->childCount()-1)->setData(i,query.value(i));
//            }else{
//                note = searchInRootNode(all, query.value(cIndex.parent_id).toInt());
//                if(note != all){
//                    note->insertChildren(note->childCount(),1,note->columnCount());
//                    for(int i = 0;i<query.record().count();i++)
//                        note->child(note->childCount()-1)->setData(i,query.value(i));
//                }else{
//                    for(int i = 0;i<query.record().count();i++)
//                        buffer.append(query.value(i));
//                }
//            }
//        }

//        while(buffer.size()>0)
//        {
//            note = search(all, buffer.value(f+2).toInt());
//            if(note != all){
//                note->insertChildren(note->childCount(),1,note->columnCount());
//                for(int i = 0;i<query.record().count();i++)
//                    note->child(note->childCount()-1)->setData(i,buffer.takeAt(f));
//                n = 0;
//            }else{
//                f = f+query.record().count();
//                if(f>buffer.size()){
//                    f = 0; n++;}
//                if(n>2){
//                    while(buffer.size()>0){
//                        all->insertChildren(all->childCount(),1,all->columnCount());
//                        for(int i = 0;i<query.record().count();i++)
//                            all->child(all->childCount()-1)->setData(i,buffer.takeAt(f));
//                    }
//                }
//            }
//        }
//    }
//    return all;
//}
QSqlError DeviceModel::lastError()
{
    return lastErr;
}

void DeviceModel::setLastError(const QSqlError &error)
{
    lastErr = error;
}

QString DeviceModel::aliasModelTable()
{
    return aliasTable;
}
QString DeviceModel::nameModelTable()
{
    return tabName;
}
QString DeviceModel::nameModelTreeTable()
{
    return treeTable;
}
