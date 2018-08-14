#include <QSqlError>
#include <QDir>
#include "headers/pomodel.h"
#include "headers/treeitem.h"

PoModel::PoModel(QObject *parent) : QAbstractItemModel(parent) {
    filter = "";
    tabName = "po";
    rootData << "Name";rootData << "CodPO";rootData << "CodGroupPO";rootData << "isGroup";rootData << "CodProducer";
    rootData << "Note";rootData << "Ico";
    rootItemData = new TreeItem(rootData);
}
PoModel::~PoModel(){
    delete rootItemData;
}
QModelIndex PoModel::index(int row, int column,
                             const QModelIndex& parent) const {
    if (!rootItemData || row < 0 || column < 0)
        return QModelIndex();
    TreeItem *parentItem = itemDataFromIndex(parent);
    TreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();
    return createIndex(row, column, childItem);
}
bool PoModel::hasChildren(const QModelIndex& parent) const {
    TreeItem *parentItem = itemDataFromIndex(parent);
    if (!parentItem) return false;
    else return (parentItem->childCount() > 0);
}
QModelIndex PoModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
        return QModelIndex();
    TreeItem *childItem = itemDataFromIndex(child);
    TreeItem *parentItem = childItem->parent();
    if (parentItem == rootItemData)
        return QModelIndex();
    return createIndex(parentItem->childNumber(), 0, parentItem);
}
int PoModel::rowCount(const QModelIndex& index) const {
    if (index.column() > 0)
        return 0;
    TreeItem *item = itemDataFromIndex(index);
    if ( item )
        return item->childCount();
    else
        return 0;
}
int PoModel::columnCount(const QModelIndex& /* index */ ) const {
    return rootItemData->columnCount();
}
QVariant PoModel::data(const QModelIndex& index, int role) const {
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
            if (index.column() == 0 && item->data(3).toInt() == 1)
                return ip.icon(QFileIconProvider::Folder);
            if (index.column() == 0 && item->data(3).toInt() == 0){
                if(item->data(6).toString().isEmpty() || item->data(6).toString().isNull()){
                    return QIcon(QDir::currentPath()+"/ico/typepoico/Default.png");
                }else{
                    return QIcon(QDir::currentPath()+"/ico/typepoico/"+item->data(6).toString());
                }
            }
        }
    }
    return QVariant();
}
void PoModel::setRootItemData(TreeItem *item){
    beginResetModel();
    delete rootItemData;
    rootItemData = item;
    endResetModel();
}
TreeItem* PoModel::itemDataFromIndex(const QModelIndex& index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItemData;
}
Qt::ItemFlags PoModel::flags(const QModelIndex& /*index*/) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
QVariant PoModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItemData->data(section);
    return QVariant();
}
bool PoModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;
    bool result = rootItemData->setData(section, value);
    if (result)
        emit headerDataChanged(orientation, section, section);
    return result;
}
bool PoModel::setData(const QModelIndex& index,
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
                if(col >= 0 && col <= 3)
                    ok = false;
                else{
                    ok = query.exec(QString("UPDATE %1 SET %3 = NULL WHERE CodPO = %2")
                                    .arg(tabName)
                                    .arg(item->data(1).toInt())
                                    .arg(rootItemData->data(col).toString()));
                    if(!ok) lastErr = query.lastError();
                }
            }else{
                if(col == 1)
                    ok = false;
                else{
                    ok = query.exec(QString("UPDATE %1 SET %4 = '%2' WHERE CodPO = %3")
                                    .arg(tabName)
                                    .arg(value.toString())
                                    .arg(item->data(1).toInt())
                                    .arg(rootData.value(col).toString()));
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
bool PoModel::setDataWithOutSQL(const QModelIndex& index,
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
void PoModel::setFilter(const QString& filters)
{
    filter = filters;
}
void PoModel::select()
{
    setRootItemData(InitTree());
}
bool PoModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;
    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItemData->insertColumns(position, columns);
    endInsertColumns();
    return success;
}
bool PoModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;
    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItemData->removeColumns(position, columns);
    endRemoveColumns();
    if (rootItemData->columnCount() == 0)
        removeRows(0, rowCount());
    return success;
}
bool PoModel::insertRows(int row, int count, const QModelIndex &parent)
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

bool PoModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(itemDataFromIndex(parent)->child(row)->data(1).toInt() == 0)
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
bool PoModel::updateRow(int row, const QModelIndex &parent)
{
    bool ok;
    QSqlQuery query;
    int id;
    TreeItem *parentItem = itemDataFromIndex(parent);
    id = parentItem->child(row)->data(1).toInt();
    ok = query.exec(QString("SELECT Name, CodPO, CodGroupPO, isGroup, CodProducer, Note, Ico FROM %1 WHERE CodPO = %2").arg(tabName).arg(id));
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
bool PoModel::sqlInsertRow(int row, const QString &name, int codGroup, const QModelIndex &parent, int isGroup,
                           int codProducer, const QString &note, const QString &icon)
{
    bool ok;
    QString field, val, queryStr;
    QQueue<QVariant> bindval;
    QSqlQuery query;
    int lastId;
    TreeItem *parentItem = itemDataFromIndex(parent);
    field = "("; val = "(";
    field += "CodGroupPO,Name,isGroup"; val += "?,?,?";
    bindval.enqueue(codGroup);
    bindval.enqueue(name);
    bindval.enqueue(isGroup);
    if(codProducer != 0){
        field += ",CodProducer"; val += ",?";
        bindval.enqueue(codProducer);}
    if(!note.isEmpty()){
        field += ",Note"; val += ",?";
        bindval.enqueue(note);}
    if(!icon.isEmpty()){
        field += ",Ico"; val += ",?";
        bindval.enqueue(icon);}
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
    ok = query.exec(QString("SELECT Name, CodPO, CodGroupPO, isGroup, CodProducer, Note, Ico FROM %1 WHERE CodPO = %2").arg(tabName).arg(lastId));
    if(ok) ok = insertRow(row,parent);
    else lastErr = query.lastError();
    if(ok){
        query.next();
        for(int i = 0; i<query.record().count(); i++)
            parentItem->child(row)->setData(i,query.value(i));
    }
    return ok;
}

bool PoModel::sqlRemoveRow(int row, const QModelIndex &parent)
{
    bool ok;
    TreeItem *parentItem = itemDataFromIndex(parent);
    ok = queryDelItem(parentItem->child(row));
    if(ok) ok = removeRow(row,parent);
    return ok;
}

bool PoModel::queryDelItem(TreeItem* item)
{
    QSqlQuery query;
    if(item->childCount()>0){
        if(!query.exec(QString("DELETE FROM %1 WHERE CodPO IN (SELECT c.CodPO FROM (SELECT * FROM %1) n, (SELECT * FROM potree) t, (SELECT * FROM %1) c "
                               "WHERE n.CodPO = %2 AND n.CodPO = t.parent_id AND t.id = c.CodPO);")
                       .arg(tabName)
                       .arg(item->data(1).toInt()))){
            lastErr = query.lastError();
            return false;
        }
    }else{
        if(!query.exec(QString("DELETE FROM %1 WHERE CodPO = %2").arg(tabName).arg(item->data(1).toInt()))){
            lastErr = query.lastError();
            return false;
        }
    }
    return true;
}

QModelIndex PoModel::indexFromItemData(TreeItem* itemData)
{
    int itemrow = itemData->childNumber();
    return createIndex(itemrow,0,itemData);
}

QModelIndex PoModel::moveItem(int movedId, int newParentId)
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

void PoModel::copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position)
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

TreeItem* PoModel::search(TreeItem* note, int id)
{
    int i = 0;
    TreeItem* childs;
    TreeItem* temp;
    while(i<note->childCount()){
        childs = note->child(i);
        if(childs->data(1).toInt() == id)
            return childs;
        else{
            if(childs->childCount()>0){
                temp = search(childs, id);
                if(temp->data(1).toInt() != childs->data(1).toInt())
                    return temp;
            }
        }
        i++;
    }
    return note;
}
QModelIndex PoModel::findData(int idData)
{
    TreeItem* item = search(rootItemData,idData);
    if(item == rootItemData)
        return QModelIndex();
    else
        return indexFromItemData(item);
}
TreeItem* PoModel::InitTree(){
    QSqlQuery query, minParentQuery;
    int minParent = 0;
    TreeItem *all = 0;
    if(filter.isEmpty()){
        query.exec(QString("SELECT Name, CodPO, CodGroupPO, isGroup, CodProducer, Note, Ico FROM %1").arg(tabName));
        minParentQuery.exec(QString("SELECT MIN(CodGroupPO) FROM %1").arg(tabName));
    }else{
        query.exec(QString("SELECT Name, CodPO, CodGroupPO, isGroup, CodProducer, Note, Ico FROM %1 WHERE %2").arg(tabName).arg(filter));
        minParentQuery.exec(QString("SELECT MIN(CodGroupPO) FROM %1 WHERE %2").arg(tabName).arg(filter));
    }
    if (query.lastError().type() != QSqlError::NoError){
        qDebug()<<query.lastError().text();
        return all;
    }
    if (minParentQuery.lastError().type() != QSqlError::NoError)
        qDebug()<<minParentQuery.lastError().text();
    TreeItem *note;
    QList<QVariant> buffer;
    int f = 0, n = 0;
    all = new TreeItem(rootData);
    if(query.size()>0){
        minParentQuery.next();
        minParent = minParentQuery.value(0).toInt();
        while(query.next())
        {
            if(query.value(2).toInt() == minParent){
                all->insertChildren(all->childCount(),1,all->columnCount());
                for(int i = 0;i<query.record().count();i++)
                    all->child(all->childCount()-1)->setData(i,query.value(i));
            }else{
                note = search(all, query.value(2).toInt());
                if(note != all){
                    note->insertChildren(note->childCount(),1,note->columnCount());
                    for(int i = 0;i<query.record().count();i++)
                        note->child(note->childCount()-1)->setData(i,query.value(i));
                }else{
                    for(int i = 0;i<query.record().count();i++)
                        buffer.append(query.value(i));
                }
            }
        }

        while(buffer.size()>0)
        {
            note = search(all, buffer.value(f+2).toInt());
            if(note != all){
                note->insertChildren(note->childCount(),1,note->columnCount());
                for(int i = 0;i<query.record().count();i++)
                    note->child(note->childCount()-1)->setData(i,buffer.takeAt(f));
                n = 0;
            }else{
                f = f+query.record().count();
                if(f>buffer.size()){
                    f = 0; n++;}
                if(n>2){
                    while(buffer.size()>0){
                        all->insertChildren(all->childCount(),1,all->columnCount());
                        for(int i = 0;i<query.record().count();i++)
                            all->child(all->childCount()-1)->setData(i,buffer.takeAt(f));
                    }
                }
            }
        }
    }
    return all;
}
QSqlError PoModel::lastError()
{
    return lastErr;
}
