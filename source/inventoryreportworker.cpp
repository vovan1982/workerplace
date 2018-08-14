#include "headers/createinventoryreport.h"
#include "headers/devicemodel.h"
#include "headers/devicemodelcontrol.h"
#include "headers/inventoryreportworker.h"

InventoryReportWorker::InventoryReportWorker(const QMap<QString, QMap<QString, int> > &data, const QSqlDatabase &connectionData) : m_data(data), m_connectionData(connectionData)
{

}

InventoryReportWorker::~InventoryReportWorker()
{
    delete cInvRep;
    delete orgTexModel;
}

QList<InventoryReportWorker::departments> InventoryReportWorker::depList(int parentId, QString depName, const QSqlDatabase &connection, int iteranion)
{
    QList<departments> dep;
    departments depitem;
    QSqlQuery queryDep(connection);

    if(iteranion > 0){
        depitem.id = parentId;
        depitem.name = depName;
        dep.append(depitem);
    }
    queryDep.exec("BEGIN;");
    if(!queryDep.exec(QString("SELECT "
                  "name,id,parent_id,firm,fp,RV "
                  "FROM departments "
                  "WHERE id IN (SELECT c.id FROM "
                  "(SELECT * FROM departments) n, "
                  "(SELECT * FROM tree) t, "
                  "(SELECT * FROM departments) c "
                  "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND t.level > 0) "
                                               "ORDER BY firm DESC, fp DESC, id;"
                      "COMMIT;").arg(parentId))){
        return dep;
    }
    if(queryDep.size() > 0){
        while (queryDep.next()) {
           dep.append(depList(queryDep.value(1).toInt(),depName+"/"+queryDep.value(0).toString(),connection,++iteranion));
        }
    }
    return dep;
}

void InventoryReportWorker::process(){
    bool tableIsCreated = false;
    bool depIsCreated = false;
    QList<departments> dep;

    QSqlDatabase db;

    emit progresstext(tr("Подключение базы данных"));

    if(QSqlDatabase::connectionNames().contains("inventoryReport_thread")){
        m_connectionData.removeDatabase("inventoryReport_thread");
    }
    db = QSqlDatabase::addDatabase("QMYSQL","inventoryReport_thread");
    db.setHostName(m_connectionData.hostName());
    db.setDatabaseName(m_connectionData.databaseName());
    db.setPort(m_connectionData.port());
    db.setUserName(m_connectionData.userName());
    db.setPassword(m_connectionData.password());
    db.open();
    lastErr = db.lastError();
    if(!db.isOpen()){
        emit progresstext(tr("Не удалось подключить базу данных"));
        emit finished();
        return;
    }

    QSqlQuery queryWP(db);

    cInvRep = new CreateInventoryReport(this);

    emit progresstext(tr("Загрузка данных об устройствах"));

    orgTexModel = new DeviceModelControl(this,"InventoryReport",db,"",true,false);

    emit progresstext(tr("Получение данных о подразделениях"));

    if(m_data.contains("Firm")){
        cInvRep->createTitle(QString("Инвентаризация по фирме: %1").arg(m_data.value("Firm").firstKey()));
        queryWP.exec("BEGIN;");
        queryWP.exec(QString("SELECT "
                     "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                     "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                     "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                     "workerplace.warehouse, workerplace.RV "
                     "FROM workerplace "
                     "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                     "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                     "WHERE CodDepartment = %1 ORDER BY 4 DESC;"
                     "COMMIT;").arg(m_data.value("Firm").value(m_data.value("Firm").firstKey())));
        dep = depList(m_data.value("Firm").value(m_data.value("Firm").firstKey()),m_data.value("Firm").firstKey(),db);
    }
    if(m_data.contains("Dep")){
        cInvRep->createTitle(QString("Инвентаризация по подразделению: %1").arg(m_data.value("Dep").firstKey()));
        queryWP.exec("BEGIN;");
        queryWP.exec(QString("SELECT "
                     "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                     "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                     "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                     "workerplace.warehouse, workerplace.RV "
                     "FROM workerplace "
                     "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                     "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                     "WHERE CodDepartment = %1 ORDER BY 4 DESC;"
                     "COMMIT;").arg(m_data.value("Dep").value(m_data.value("Dep").firstKey())));
        dep = depList(m_data.value("Dep").value(m_data.value("Dep").firstKey()),m_data.value("Dep").firstKey(),db);
    }
    if(m_data.contains("WP")){
        cInvRep->createTitle(QString("Инвентаризация по Рабочему мету/Складу: %1").arg(m_data.value("WP").firstKey()));
        queryWP.exec("BEGIN;");
        queryWP.exec(QString("SELECT "
                     "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                     "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                     "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                     "workerplace.warehouse, workerplace.RV "
                     "FROM workerplace "
                     "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                     "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                     "WHERE CodWorkerPlace = %1 ORDER BY 4 DESC;"
                     "COMMIT;").arg(m_data.value("WP").value(m_data.value("WP").firstKey())));
    }
    emit progresstext(tr("Построение отчёта"));

    if(queryWP.size()>0)
    {
        while(queryWP.next())
        {
            orgTexModel->setDevFilter(QString("dev.id IN ( "
                                              "SELECT id FROM device WHERE CodWorkerPlace = %1)")
                                      .arg(queryWP.value(0).toInt()));
            if(orgTexModel->model()->rowCount()>0){
                if(!tableIsCreated){
                    cInvRep->createTable();
                    tableIsCreated = true;
                }
                for(int n=0; n<orgTexModel->model()->rowCount();n++){
                    cInvRep->addRec(queryWP.value(3).toString(),orgTexModel->model()->index(n,orgTexModel->model()->cIndex.typeDevName).data().toString(),
                                    orgTexModel->model()->index(n,orgTexModel->model()->cIndex.name).data().toString(),
                                    orgTexModel->model()->index(n,orgTexModel->model()->cIndex.inventoryN).data().toString(),
                                    orgTexModel->model()->index(n,orgTexModel->model()->cIndex.serialN).data().toString());
                    if(!m_data.contains("hideComposition")){
                        if(orgTexModel->model()->rowCount(orgTexModel->model()->index(n,0))>0)
                            for(int k=0; k<orgTexModel->model()->rowCount(orgTexModel->model()->index(n,0));k++)
                                cInvRep->addSubRec(queryWP.value(3).toString(),
                                                   orgTexModel->model()->index(k,orgTexModel->model()->cIndex.typeDevName,
                                                                               orgTexModel->model()->index(n,0)).data().toString(),
                                                   orgTexModel->model()->index(k,orgTexModel->model()->cIndex.name,
                                                                               orgTexModel->model()->index(n,0)).data().toString(),
                                                   orgTexModel->model()->index(k,orgTexModel->model()->cIndex.inventoryN,
                                                                               orgTexModel->model()->index(n,0)).data().toString(),
                                                   orgTexModel->model()->index(k,orgTexModel->model()->cIndex.serialN,
                                                                               orgTexModel->model()->index(n,0)).data().toString());
                    }
                }
            }
        }
        tableIsCreated = false;
    }

    if(m_data.contains("Firm") || (m_data.contains("Dep") && m_data.contains("givenSubDep"))){
        for(int i=0;i<dep.count();i++){
            queryWP.exec("BEGIN;");
            queryWP.exec(QString("SELECT "
                         "workerplace.CodWorkerPlace, workerplace.CodDepartment, departments.Name, "
                         "workerplace.Name, workerplace.PrimaryUser, users.FIOSummary, "
                         "workerplace.InternalNumber, workerplace.Location, departments.Firm, departments.FP, "
                         "workerplace.warehouse, workerplace.RV "
                         "FROM workerplace "
                         "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                         "LEFT OUTER JOIN users ON workerplace.PrimaryUser = users.CodUser "
                         "WHERE CodDepartment = %1 ORDER BY 4 DESC;"
                         "COMMIT;").arg(dep.at(i).id));
            if(queryWP.size()>0)
            {
                while(queryWP.next())
                {
                    orgTexModel->setDevFilter(QString("dev.id IN ( "
                                                      "SELECT id FROM device WHERE CodWorkerPlace = %1)")
                                              .arg(queryWP.value(0).toInt()));
                    if(orgTexModel->model()->rowCount()>0){
                        if(!depIsCreated){
                            cInvRep->createDepName(dep.at(i).name);
                            depIsCreated = true;
                        }
                        if(!tableIsCreated){
                            cInvRep->createTable();
                            tableIsCreated = true;
                        }
                        for(int n=0; n<orgTexModel->model()->rowCount();n++){
                            cInvRep->addRec(queryWP.value(3).toString(),orgTexModel->model()->index(n,
                                                                                                              orgTexModel->model()->cIndex.typeDevName)
                                            .data().toString(),
                                            orgTexModel->model()->index(n,orgTexModel->model()->cIndex.name).data().toString(),
                                            orgTexModel->model()->index(n,orgTexModel->model()->cIndex.inventoryN).data().toString(),
                                            orgTexModel->model()->index(n,orgTexModel->model()->cIndex.serialN).data().toString());
                            if(!m_data.contains("hideComposition")){
                                if(orgTexModel->model()->rowCount(orgTexModel->model()->index(n,0))>0)
                                    for(int k=0; k<orgTexModel->model()->rowCount(orgTexModel->model()->index(n,0));k++)
                                        cInvRep->addSubRec(queryWP.value(3).toString(),
                                                           orgTexModel->model()->index(k,orgTexModel->model()->cIndex.typeDevName,
                                                                                       orgTexModel->model()->index(n,0)).data().toString(),
                                                           orgTexModel->model()->index(k,orgTexModel->model()->cIndex.name,
                                                                                       orgTexModel->model()->index(n,0)).data().toString(),
                                                           orgTexModel->model()->index(k,orgTexModel->model()->cIndex.inventoryN,
                                                                                       orgTexModel->model()->index(n,0)).data().toString(),
                                                           orgTexModel->model()->index(k,orgTexModel->model()->cIndex.serialN,
                                                                                       orgTexModel->model()->index(n,0)).data().toString());
                            }
                        }
                    }
                }
                tableIsCreated = false;
                depIsCreated = false;
            }
        }
    }

    if(db.isOpen()) db.close();
    emit progresstext(tr("Отчёт построен. Вывод..."));
    emit reportCreated(cInvRep->clone());
    emit finished();
}
