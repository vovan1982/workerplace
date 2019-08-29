#include "headers/treeitem.h"
#include "headers/licensemodelworker.h"

LicenseModelWorker::LicenseModelWorker(const QMap<QString, QVariant> &credentials,
                                       const QMap<QString, QString> &forQuery,
                                       const QVector<QVariant> &rootData,
                                       const QMap<QString, int> &indexColumn,
                                       bool showParent) :
    m_credentials(credentials),
    m_rootData(rootData),
    m_indexColumn(indexColumn),
    m_showParent(showParent)
{
    primaryQuery = forQuery.value("primaryQuery");
    filter = forQuery.value("filter");
}

LicenseModelWorker::~LicenseModelWorker()
{

}

void LicenseModelWorker::setFilter(const QString &queryFilter)
{
    filter = queryFilter;
}

void LicenseModelWorker::setShowParent(bool sp)
{
    m_showParent = sp;
}

void LicenseModelWorker::process()
{
    QSqlDatabase::removeDatabase("licensemodelworker_connect");
    QSqlDatabase db = QSqlDatabase::addDatabase(m_credentials.value("driver").toString(),"licensemodelworker_connect");
    db.setHostName(m_credentials.value("host").toString());
    db.setPort(m_credentials.value("port").toInt());
    db.setUserName(m_credentials.value("login").toString());
    db.setPassword(m_credentials.value("pass").toString());
    db.setDatabaseName(m_credentials.value("databaseName").toString());
    if(!db.open()){
        emit error(tr("Не удалось подключиться к серверу mysql: %1").arg(db.lastError().text()));
        emit finished();
        return;
    }

   QSqlQuery query(db);
   TreeItem *all = new TreeItem(m_rootData);

    if(filter.isEmpty())
        query.exec(primaryQuery);
    else
        query.exec(QString("%1WHERE %2").arg(primaryQuery).arg(filter));

    if (query.lastError().type() != QSqlError::NoError){
        emit error(query.lastError().text());
        emit result(all);
        emit finished();
        return;
    }

    if(query.size()>0){
        while(query.next())
        {
            int i;
            all->insertChildren(all->childCount(),1,all->columnCount());
            for(i = 0;i<query.record().count();i++)
                all->child(all->childCount()-1)->setData(i,query.value(i));
            all->child(all->childCount()-1)->setData(i,1);
        }
        if (m_showParent){
            QSqlQuery queryChildren(db);
            bool ok;
            for(int i = 0; i<all->childCount();i++){
                TreeItem *child = all->child(i);
                int licenseKey = child->data(m_indexColumn.value("key")).toInt();
                ok = queryChildren.exec(
                            QString("SELECT device.name, device.id, device.NetworkName, typedevice.IconPath, device.InventoryN, device.CodOrganization, departments.Name FROM device "
                                    "INNER JOIN typedevice ON device.CodTypeDevice = typedevice.CodTypeDevice "
                                    "LEFT OUTER JOIN departments ON device.CodOrganization = departments.id "
                                    "WHERE device.id IN "
                                    "(SELECT p.id FROM device n, devtree t, device p "
                                    "WHERE n.id IN "
                                    "(SELECT CodDevice FROM licenseanddevice WHERE CodLicense = %1) "
                                    "AND n.id = t.id AND t.parent_id = p.id) "
                                    "ORDER BY typedevice.type DESC;"
                                    ).arg(licenseKey));
                if(ok){
                    if(queryChildren.size() > 0){
                        while(queryChildren.next()){
                            child->insertChildren(child->childCount(),1,child->columnCount());
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("namePO"),queryChildren.value(2).toString() + " / " + queryChildren.value(0).toString());
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("key"),queryChildren.value(1));
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("ico"),queryChildren.value(3));
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("invN"),queryChildren.value(4));
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("codOrganization"),queryChildren.value(5));
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("nameOrg"),queryChildren.value(6));
                            child->child(child->childCount()-1)->setData(m_indexColumn.value("isLicense"),0);
                        }
                    }
                }else{
                    emit error(tr("При получении спика устройств лицензии %1 с кодом %2 возникла ошибка:\n%3")
                                              .arg(child->data(m_indexColumn.value("namePO")).toString())
                                              .arg(licenseKey)
                                              .arg(queryChildren.lastError().text()));
                    emit result(all);
                    emit finished();
                    return;
                }
            }
        }
    }
    emit result(all);
    emit finished();
}
