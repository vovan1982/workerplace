#include <QtWidgets>
#include "headers/sqltreemodel.h"
#include "headers/treeitem.h"

SqlTreeModel::SqlTreeModel(QObject *parent): QAbstractItemModel(parent){
    filter = "";
    setHideRoot();
    QVector<QVariant> rootData;
    rootData << "Name";rootData << "Id";rootData << "Parent_id";
    rootData << "Firma";rootData << "FilPred";rootData << "RV";
    rootItemData = new TreeItem(rootData);
}

SqlTreeModel::~SqlTreeModel(){
    delete rootItemData;
}

QModelIndex SqlTreeModel::index(int row, int column,
                             const QModelIndex& parent) const {
    if (!rootItemData || row < 0 || column < 0)
        return QModelIndex();
    TreeItem *parentItem = itemDataFromIndex(parent);
    TreeItem *childItem = parentItem->child(row);
    if (!childItem)
        return QModelIndex();
    return createIndex(row, column, childItem);
}

bool SqlTreeModel::hasChildren(const QModelIndex& parent) const {
    TreeItem *parentItem = itemDataFromIndex(parent);
    if (!parentItem) return false;
    else return (parentItem->childCount() > 0);
}

QModelIndex SqlTreeModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
        return QModelIndex();

    TreeItem *childItem = itemDataFromIndex(child);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItemData)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int SqlTreeModel::rowCount(const QModelIndex& index) const {

    if (index.column() > 0)
        return 0;
    TreeItem *item = itemDataFromIndex(index);
    if ( item )
        return item->childCount();
    else
        return 0;
}

int SqlTreeModel::columnCount(const QModelIndex& /* index */ ) const {
    return rootItemData->columnCount();
}

QVariant SqlTreeModel::data(const QModelIndex& index, int role) const {
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
                if (index.column() == 0 && item->data(3) == 1)
                    return QIcon(":/128x128/network/ico/network_128x128.png");
                else if (index.column() == 0 && item->data(4) == 1)
                    return QIcon(":/128x128/network/ico/network_local_128x128.png");
                else if (index.column() == 0 && item->data(3) == 0 && item->data(4) == 0)
                    return QIcon(":/128x128/network/ico/network_place_128x128.png");
                else
                    return QVariant();
        }
    }
    return QVariant();
}

void SqlTreeModel::setRootItemData(TreeItem *item){
    beginResetModel();
    delete rootItemData;
    rootItemData = item;
    endResetModel();
}

TreeItem* SqlTreeModel::itemDataFromIndex(const QModelIndex& index) const {
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItemData;
}

Qt::ItemFlags SqlTreeModel::flags(const QModelIndex& index) const {
    if ( index.isValid() ){
        TreeItem *item = itemDataFromIndex(parent(index));
//        TreeItem *item2 = itemDataFromIndex(index);
        if(item == rootItemData)
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        else
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
//        else if(item2->data(3).toInt() == 1 || item2->data(4).toInt() == 1)
//            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
//        else
//            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    else
        return Qt::ItemIsDropEnabled;
}

QVariant SqlTreeModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItemData->data(section);

    return QVariant();
}

bool SqlTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItemData->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

bool SqlTreeModel::setData(const QModelIndex& index,
                        const QVariant& value, int role){
    if ( role != Qt::EditRole )
        return false;
    bool ok = true;
    QSqlQuery query;
    int col = index.column();
    int rowVersion;
    TreeItem *item = itemDataFromIndex(index);
    if(SqlTreeModel::data(SqlTreeModel::index(index.row(),5,index.parent())).toInt() == 254)
        rowVersion = 0;
    else
        rowVersion = SqlTreeModel::data(SqlTreeModel::index(index.row(),5,index.parent())).toInt()+1;
    if (col == 0){
        ok = item->setData(col,value.toString());
        if(ok)
            ok = query.exec(QString("UPDATE %1 SET name = '%2', RV = %4 WHERE id = %3")
                            .arg(tabName)
                            .arg(value.toString())
                            .arg(item->data(1).toInt())
                            .arg(rowVersion));
        if(ok) item->setData(5,rowVersion);
    }
    if (col == 2){
        ok = item->setData(col,value.toInt());
        if(ok)
            ok = query.exec(QString("UPDATE %1 SET parent_id = '%2', RV = %4 WHERE id = %3")
                            .arg(tabName)
                            .arg(value.toInt())
                            .arg(item->data(1).toInt())
                            .arg(rowVersion));
        if(ok) item->setData(5,rowVersion);
    }
    if (col == 3){
        ok = item->setData(col,value.toInt());
        if(ok)
            ok = query.exec(QString("UPDATE %1 SET firm = '%2', RV = %4 WHERE id = %3")
                            .arg(tabName)
                            .arg(value.toInt())
                            .arg(item->data(1).toInt())
                            .arg(rowVersion));
        if(ok) item->setData(5,rowVersion);
    }
    if (col == 4){
        ok = item->setData(col,value.toInt());
        if(ok)
            ok = query.exec(QString("UPDATE %1 SET fp = '%2', RV = %4 WHERE id = %3")
                            .arg(tabName)
                            .arg(value.toInt())
                            .arg(item->data(1).toInt())
                            .arg(rowVersion));
        if(ok) item->setData(5,rowVersion);
    }
    if (ok)
        emit dataChanged(index, index);
    return ok;
}

bool SqlTreeModel::setDataWithOutSQL(const QModelIndex& index,
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

void SqlTreeModel::setTable(const QString& tableName)
{
    tabName = tableName;
}

void SqlTreeModel::setRootName(const QString& rootName)
{
    rtName = rootName;
}

void SqlTreeModel::setFilter(const QString& filters)
{
    filter = filters;
}

void SqlTreeModel::setHideRoot(bool ok)
{
    hideRoot = ok;
}

QString SqlTreeModel::tableName() const
{
    return tabName;
}

void SqlTreeModel::select()
{
    setRootItemData(InitTree());
}
bool SqlTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginInsertColumns(parent, position, position + columns - 1);
    success = rootItemData->insertColumns(position, columns);
    endInsertColumns();

    return success;
}
bool SqlTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    bool success;

    beginRemoveColumns(parent, position, position + columns - 1);
    success = rootItemData->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItemData->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}
bool SqlTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(!parent.isValid())
        return false;
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

bool SqlTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (!parent.isValid())
        return false;
    if (!parent.child(row,0).isValid())
        return false;
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

bool SqlTreeModel::sqlInsertRow(int row, const QModelIndex &parent)
{
    if(!parent.isValid()) return false;
    bool ok;
    QSqlQuery query;
    TreeItem *parentItem = itemDataFromIndex(parent);
    ok = query.exec(QString("INSERT INTO %1 (parent_id, name) VALUES (%2, '<Новое>')")
                    .arg(tabName)
                    .arg(parentItem->data(1).toInt()));
    if(ok) ok = insertRow(row,parent);
    if(ok){
        parentItem->child(row)->setData(0,QString("<Новое>"));
        parentItem->child(row)->setData(1,query.lastInsertId().toInt());
        parentItem->child(row)->setData(2,parentItem->data(1).toInt());
        parentItem->child(row)->setData(3,0);
        parentItem->child(row)->setData(4,0);
    }
    return ok;
}

bool SqlTreeModel::sqlRemoveRow(int row, const QModelIndex &parent)
{
    if(!parent.isValid()) return false;
    bool ok;
    TreeItem *parentItem = itemDataFromIndex(parent);
    ok = queryDelItem(parentItem->child(row));
    if(ok) ok = removeRow(row,parent);
    return ok;
}

bool SqlTreeModel::updateRow(int row, const QModelIndex &parent)
{
    bool ok;
    QSqlQuery query;
    int id;
    TreeItem *parentItem = itemDataFromIndex(parent);
    id = parentItem->child(row)->data(1).toInt();
    ok = query.exec(QString("name,id,parent_id,firm,fp,RV FROM %1 WHERE id = %2").arg(tabName).arg(id));
    if(!ok) qDebug()<<query.lastError().text();
    else{
        query.next();
        for(int i = 0; i<query.record().count(); i++)
            parentItem->child(row)->setData(i,query.value(i));
    }
    if (ok)
        emit dataChanged(parent.child(row,0), parent.child(row,0));
    return ok;
}

QStringList SqlTreeModel::mimeTypes() const
{
    QStringList types;
    types << "sqltreemodel/data.list";
    return types;
}

QMimeData *SqlTreeModel::mimeData(const QModelIndexList &indexes) const
 {
    TreeItem *item;

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            if(index.column() == 0){
                item = itemDataFromIndex(index);
                stream << item->data(1).toInt();
            }
        }
    }

    mimeData->setData("sqltreemodel/data.list", encodedData);
    return mimeData;
}

bool SqlTreeModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction) return true;
    if (action != Qt::MoveAction) return false;
    if (!data->hasFormat("sqltreemodel/data.list")) return false;
    if (column > 0) return false;

    int beginRow;

    if (row != -1) beginRow = row;
    else if (parent.isValid()) beginRow = rowCount(parent);
    else beginRow = rowCount(index(0,0,QModelIndex()));

    TreeItem *item;
    QModelIndex ind, tmp;
    QModelIndex parentIndex = parent;
    QSqlQuery query, query2;
    bool ok;
    QList<QVariant> buffer;
    int f, n;

    QByteArray encodedData = data->data("sqltreemodel/data.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    QList<int> moveIds;

    while (!stream.atEnd()) {
        int id;
        stream >> id;
        moveIds << id;
    }

    foreach (int id, moveIds) {
        f = 0; n = 0;
        ok = query.exec(QString("SELECT name,id,firm,fp FROM %1 WHERE id = %2").arg(tabName).arg(id));
        if(!ok){
            qDebug()<<query.lastError().text();
            return false;}
        if(parentIndex != QModelIndex()){
            insertRow(beginRow,parentIndex);
        }else{
            parentIndex = index(0,0,QModelIndex());
            insertRow(beginRow,parentIndex);
        }
        item = itemDataFromIndex(index(beginRow,0,parentIndex));
        while (query.next()){
            item->setData(0,query.value(0));
            item->setData(1,query.value(1));
            item->setData(2,SqlTreeModel::data(index(parentIndex.row(),1,parentIndex.parent())));
            item->setData(3,query.value(2));
            item->setData(4,query.value(3));
            item->setData(5,query.value(5));
            query2.exec(QString("UPDATE %1 SET parent_id = '%2' WHERE id = %3")
                        .arg(tabName)
                        .arg(SqlTreeModel::data(index(parentIndex.row(),1,parentIndex.parent())).toInt())
                        .arg(id));
        }
        ind = parentIndex.child(beginRow,0);
        ok = query.exec(QString("SELECT c.name,c.id,c.parent_id,c.firm,c.fp FROM %1 n, tree t, %1 c "
                           "WHERE n.id = %2 AND n.id = t.parent_id AND t.id = c.id AND t.level > 0;").arg(tabName).arg(id));
        if(!ok){
            qDebug()<<query.lastError().text();
            return false;}
        while (query.next()){
            if (query.value(2).toInt() == SqlTreeModel::data(parentIndex.child(beginRow,1)).toInt()){
                insertRow(rowCount(ind),ind);
                item = itemDataFromIndex(index(rowCount(ind)-1,0,ind));
                for(int i=0;i<query.record().count();i++)
                    item->setData(i,query.value(i));
            }else{
                item = search(itemDataFromIndex(ind), query.value(2).toInt());
                if(item != itemDataFromIndex(ind)){
                    tmp = indexFromItemData(item);
                    insertRow(rowCount(tmp),tmp);
                    item = itemDataFromIndex(index(rowCount(tmp)-1,0,tmp));
                    for(int i=0;i<query.record().count();i++)
                        item->setData(i,query.value(i));
                }else{
                    for(int i = 0;i<query.record().count();i++)
                        buffer.append(query.value(i));
                }
            }
        }

        while(buffer.size()>0)
        {
            item = search(itemDataFromIndex(ind), buffer.value(f+2).toInt());
            if(item != itemDataFromIndex(ind)){
                tmp = indexFromItemData(item);
                insertRow(rowCount(tmp),tmp);
                item = itemDataFromIndex(index(rowCount(tmp)-1,0,tmp));
                for(int i=0;i<query.record().count();i++)
                    item->setData(i,buffer.takeAt(f));
                n = 0;
            }else{
                f = f+query.record().count();
                if(f>buffer.size()){
                    f = 0; n++;}
                if(n>2){
                    while(buffer.size()>0){
                        insertRow(rowCount(ind),ind);
                        item = itemDataFromIndex(index(rowCount(ind)-1,0,ind));
                        for(int i=0;i<query.record().count();i++)
                            item->setData(i,buffer.takeAt(f));
                    }
                }
            }
        }
        beginRow++;
    }
    return true;
}

 Qt::DropActions SqlTreeModel::supportedDropActions() const {
     return Qt::MoveAction;
 }

bool SqlTreeModel::queryDelItem(TreeItem* item)
{
    QSqlQuery query;
    if(item->childCount()>0){
        if(!query.exec(QString("DELETE FROM %1 WHERE id IN (SELECT c.id FROM (SELECT * FROM %1) n, (SELECT * FROM tree) t, (SELECT * FROM %1) c "
                               "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id);")
            .arg(tabName)
            .arg(item->data(1).toInt()))){
            qDebug()<<query.lastError().text();
            return false;
        }
    }else{
        if(!query.exec(QString("DELETE FROM %1 WHERE id = %2").arg(tabName).arg(item->data(1).toInt()))){
            qDebug()<<query.lastError().text();
            return false;
        }
    }
    return true;
}

QModelIndex SqlTreeModel::indexFromItemData(TreeItem* itemData)
{
    int itemrow = itemData->childNumber();
    return createIndex(itemrow,0,itemData);
}

QModelIndex SqlTreeModel::moveItem(int movedId, int newParentId)
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

void SqlTreeModel::copyItem(const QModelIndex& indexFrom, const QModelIndex& indexTo, int position)
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

TreeItem* SqlTreeModel::search(TreeItem* note, int id)
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
QModelIndex SqlTreeModel::findData(int idData)
{
    return indexFromItemData(search(rootItemData,idData));
}
TreeItem* SqlTreeModel::InitTree(){
    QSqlQuery query, minParentQuery;
    int minParent;
    TreeItem *root = 0;
    if(filter.isEmpty()){
        query.exec(QString("SELECT name,id,parent_id,firm,fp,RV FROM %1 ORDER BY firm DESC, fp DESC, id").arg(tabName));
        minParentQuery.exec(QString("SELECT MIN(parent_id) FROM %1").arg(tabName));
    }else{
        query.exec(QString("SELECT name,id,parent_id,firm,fp,RV FROM %1 WHERE %2 ORDER BY firm DESC, fp DESC, id").arg(tabName).arg(filter));
        minParentQuery.exec(QString("SELECT MIN(parent_id) FROM %1 WHERE %2").arg(tabName).arg(filter));
    }
    if (query.lastError().type() != QSqlError::NoError){
        qDebug()<<query.lastError().text();
        return root;
    }
    if (minParentQuery.lastError().type() != QSqlError::NoError)
        qDebug()<<minParentQuery.lastError().text();
    TreeItem *all;
    TreeItem *note;
    QVector<QVariant> rootData;
    QList<QVariant> buffer;
    int f = 0, n = 0;
    for(int i=0; i<rootItemData->data().size(); i++)
        rootData << rootItemData->data(i);

    if(!hideRoot){
        root = new TreeItem(rootData);
        root->insertChildren(root->childCount(),1,root->columnCount());

        all = root->child(root->childCount()-1);
        if (rtName.isNull())
            all->setData(0,"/");
        else
            all->setData(0,rtName);

        all->setData(1,0);
        all->setData(2,0);
        all->setData(3,0);
        all->setData(4,0);
        all->setData(5,0);
    }else
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
    if(!hideRoot)
        return root;
    else
        return all;
}
