#include <QSqlQuery>
#include "headers/treeitem.h"
#include "headers/devicethreadworker.h"

DeviceThreadWorker::DeviceThreadWorker(const QString &dbConnectionName, const QSqlDatabase &connectionData, QObject *parent) :
    QObject(parent), m_dbConnectionName(dbConnectionName)
{

    if(QSqlDatabase::connectionNames().contains(dbConnectionName+"_thread")){
        connectionData.removeDatabase(dbConnectionName+"_thread");
    }
    db = QSqlDatabase::addDatabase("QMYSQL",dbConnectionName+"_thread");
    db.setHostName(connectionData.hostName());
    db.setDatabaseName(connectionData.databaseName());
    db.setPort(connectionData.port());
    db.setUserName(connectionData.userName());
    db.setPassword(connectionData.password());
    db.open();
    lastErr = db.lastError();
}

DeviceThreadWorker::~DeviceThreadWorker()
{
    if(db.isOpen())
        db.close();
}

TreeItem* DeviceThreadWorker::search(TreeItem* note, int id, int colId)
{
    int i = 0;
    TreeItem* childs;
    TreeItem* temp;
    while(i<note->childCount()){
        childs = note->child(i);
        if(childs->data(colId).toInt() == id)
            return childs;
        else{
            if(childs->childCount()>0){
                temp = search(childs, id, colId);
                if(temp->data(colId).toInt() != childs->data(colId).toInt())
                    return temp;
            }
        }
        i++;
    }
    return note;
}

TreeItem* DeviceThreadWorker::searchInRootNode(TreeItem* node, int id, int colId)
{
    int i = 0;
    TreeItem* childs;
    while(i < node->childCount()){
        childs = node->child(i);
        if(childs->data(colId).toInt() == id)
            return childs;
        i++;
    }
    return node;
}

void DeviceThreadWorker::createDeviceTree(const QMap<QString,QString> &forQuery, const QVector<QVariant> &rootData)
{
    TreeItem *all = 0;
    if(lastErr.type() != QSqlError::NoError){
        emit lastErrors(lastErr);
        emit result(all);
        return;
    }
    if(!db.isOpen()){
        if(!db.open()){
            lastErr = db.lastError();
            if(lastErr.type() != QSqlError::NoError)
                emit lastErrors(lastErr);
            emit result(all);
            return;
        }
    }
    QSqlQuery query(db);
    QSqlQuery minParentQuery(db);
    int minParent;
    QString primaryQuery, filter, tabName, aliasTable;

    int colId, colParent_Id;

    primaryQuery = forQuery.value("primaryQuery");
    filter = forQuery.value("filter");
    tabName = forQuery.value("tabName");
    aliasTable = forQuery.value("aliasTable");
    colId = forQuery.value("columnId").toInt();
    colParent_Id = forQuery.value("columnParent_Id").toInt();
    if(filter.isEmpty()){
        query.exec("BEGIN;");
        query.exec(QString("%1"
                           "ORDER BY "
                           "typedevice.Type DESC, parent_id; "
                           "COMMIT;").arg(primaryQuery));
        minParentQuery.exec(QString("SELECT MIN(parent_id) FROM %1").arg(tabName));
    }else{
        query.exec("BEGIN;");
        query.exec(QString("%1"
                           "WHERE %2 "
                           "ORDER BY "
                           "typedevice.Type DESC, parent_id; "
                           "COMMIT;").arg(primaryQuery).arg(filter));
        minParentQuery.exec(QString("SELECT MIN(%3.parent_id) FROM %1 %3 "
                                    "LEFT OUTER JOIN domainwg "
                                    "ON %3.CodDomainWg = domainwg.CodDomainWg "
                                    "LEFT OUTER JOIN statedev "
                                    "ON %3.CodState = statedev.CodState "
                                    "LEFT OUTER JOIN typedevice "
                                    "ON %3.CodTypeDevice = typedevice.CodTypeDevice "
                                    "LEFT OUTER JOIN departments "
                                    "ON %3.CodOrganization = departments.id "
                                    "LEFT OUTER JOIN workerplace "
                                    "ON %3.CodWorkerPlace = workerplace.CodWorkerPlace "
                                    "LEFT OUTER JOIN provider "
                                    "ON %3.CodProvider = provider.CodProvider "
                                    "LEFT OUTER JOIN producer "
                                    "ON %3.CodProducer = producer.CodProducer "
                                    " WHERE %2")
                            .arg(tabName)
                            .arg(filter)
                            .arg(aliasTable));
    }
    if (query.lastError().type() != QSqlError::NoError){
        emit lastErrors(query.lastError());
        emit result(all);
        return;
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
            if(query.value(colParent_Id).toInt() == minParent){
                all->insertChildren(all->childCount(),1,all->columnCount());
                for(int i = 0;i<query.record().count();i++)
                    all->child(all->childCount()-1)->setData(i,query.value(i));
            }else{
                note = searchInRootNode(all, query.value(colParent_Id).toInt(),colId);
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
            note = search(all, buffer.value(f+2).toInt(),colId);
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
    emit result(all);
}
