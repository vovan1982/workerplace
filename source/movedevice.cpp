#include <QtSql>
#include <QtGui>
#include <QMessageBox>
#include <QStandardItemModel>
#include "headers/movedevice.h"
#include "headers/edittable.h"
#include "headers/selectworkplace.h"
#include "headers/selectdevice.h"
#include "headers/delegats.h"
#include "headers/devicemodel.h"
#include "headers/addexistinguser.h"
#include "headers/lockdatabase.h"

MoveDevice::MoveDevice(QWidget *parent, bool singlMode, const QList<QVariant> &curWPData,
                       const QList<QVariant> &devData, bool addWPMode, bool moveFromWp) :
        QDialog(parent),
        m_singlMode(singlMode),
        m_addWPMode(addWPMode),
        m_moveFromWp(moveFromWp){
    setupUi(this);
    lockedControl = new LockDataBase(this);
    lockedControlLicense = new LockDataBase(this);
    curOrgId = 0; curFilPredId = 0; curDepId = 0; curWPId = 0;
    newOrgId = 0; newFilPredId = 0; newDepId = 0; newWPId = 0;
    newOrgTex->setData(0); typeDevForMove = 0;
    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());
    devModel = new DeviceModel("moveDevice",db,this);
    moveDevModel = new QStandardItemModel(0,devModel->columnCount()-1,this);
    moveDevModel->setHeaderData(devModel->cIndex.typeDevName, Qt::Horizontal,tr("Тип устройства"));
    moveDevModel->setHeaderData(devModel->cIndex.name, Qt::Horizontal,tr("Наименование"));
    moveDevModel->setHeaderData(devModel->cIndex.networkName, Qt::Horizontal,tr("Сетевое имя"));
    moveDevModel->setHeaderData(devModel->cIndex.domainWgName, Qt::Horizontal,tr("Домен/Рабочая группа"));
    moveDevModel->setHeaderData(devModel->cIndex.inventoryN, Qt::Horizontal,tr("Инвентарный №"));
    moveDevModel->setHeaderData(devModel->cIndex.serialN, Qt::Horizontal,tr("Серийный №"));
    moveDevModel->setHeaderData(devModel->cIndex.producerName, Qt::Horizontal,tr("Производитель"));
    moveDevModel->setHeaderData(devModel->cIndex.providerName, Qt::Horizontal,tr("Поставщик"));
    moveDevModel->setHeaderData(devModel->cIndex.datePurchase, Qt::Horizontal,tr("Дата покупки"));
    moveDevModel->setHeaderData(devModel->cIndex.datePosting, Qt::Horizontal,tr("Дата оприходования"));
    moveDevModel->setHeaderData(devModel->cIndex.endGuarantee, Qt::Horizontal,tr("Конец гарантии"));
    moveDevModel->setHeaderData(devModel->cIndex.price, Qt::Horizontal,tr("Стоимость"));
    moveDevModel->setHeaderData(devModel->cIndex.stateName, Qt::Horizontal,tr("Состояние"));
    moveDevModel->setHeaderData(devModel->cIndex.note, Qt::Horizontal,tr("Примечание"));

    deviceForMoveView->setModel(moveDevModel);
    deviceForMoveView->setColumnHidden(devModel->cIndex.id,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.parent_id,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codTypeDevice,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codOrganization,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codWorkerPlace,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.orgName,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.wpName,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codDomainWg,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codProducer,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codProvider,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.codState,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.type,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.iconPath,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.rv,true);
    deviceForMoveView->setColumnHidden(devModel->cIndex.detailDescription,true);
    deviceForMoveView->setItemDelegateForColumn(devModel->cIndex.price,new DoubleSpinBoxDelegat(0.00,9999999.99,0.01,2,deviceForMoveView));
    deviceForMoveView->resizeColumnToContents(devModel->cIndex.typeDevName);
    deviceForMoveView->resizeColumnToContents(devModel->cIndex.name);

    performer->setVisibleClearButtron(false);
    performer->setEnabledRunButtron(false);

    if(singlMode){
        typeDevForMove = devData.value(devModel->cIndex.type).toInt();
        fillCurPlace(curWPData.value(0).toInt(),curWPData.value(1).toString(),curWPData.value(2).toInt());
        addDevForMove(devData);
    }
    if(addWPMode){
        fillNewPlace(curWPData.value(0).toInt(),curWPData.value(1).toString(),curWPData.value(2).toInt());
        selectNew->setEnabled(false);
        MoveDevice::setWindowTitle(tr("Добавление устройств"));
        groupBox->setTitle(tr("ИЗ:"));
        groupBox_2->setTitle(tr("В:"));
        groupBox_3->setTitle(tr("Устройства для добавления"));
        moveDev->setText(tr("Добавить"));
        moveDev->setIcon(QIcon(":/16x16/apply/ico/apply_16x16.png"));
    }
    if(moveFromWp){
        fillCurPlace(curWPData.value(0).toInt(),curWPData.value(1).toString(),curWPData.value(2).toInt());
        selectCur->setEnabled(false);
    }
    dateMoved->setDateTime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    populateCBox("CodCause","cause","",tr("<Выберите причину>"),cause);
}
void MoveDevice::on_selectCur_clicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this, newOrgId, true);
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(fillCurPlace(int,QString,int)));
    swp->setAttribute(Qt::WA_DeleteOnClose);
    swp->exec();
}
void MoveDevice::fillCurPlace(int wpId, const QString &wpName, int orgId)
{
    curWP->setText(wpName);
    curWPId = wpId;
    curOrgId = orgId;
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT Name FROM departments WHERE id = %1").arg(orgId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    query.next();
    curOrg->setText(query.value(0).toString());
    ok = query.exec(QString("SELECT departments.id, departments.Name, departments.FP, departments.Firm FROM workerplace "
                            "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                            "WHERE workerplace.CodWorkerPlace = %1")
                    .arg(wpId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next();
        curDepId = query.value(0).toInt();
        if(query.value(2).toInt() != 1 && query.value(3).toInt() != 1)
            curDep->setText(query.value(1).toString());
        else
            curDep->setText("");
    }else{
        curDepId = 0;
        curDep->setText("");
    }
    ok = query.exec(QString("select p.id, p.Name from departments n, tree t, departments p "
                            "where n.id = %1 and n.id = t.id and t.parent_id = p.id AND p.FP = 1;").arg(curDepId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next();
        curFilPredId = query.value(0).toInt();
        curFilPred->setText(query.value(1).toString());
    }else{
        curFilPredId = 0;
        curFilPred->setText("");
    }
    if(!m_singlMode){
        if(curWPId != 0 && newWPId != 0)
            addDeviceForMove->setEnabled(true);
    }else
        selectNew->setEnabled(true);
    performer->setEnabledRunButtron(true);
}
void MoveDevice::on_selectNew_clicked()
{
    SelectWorkPlace *swp;
    if(m_singlMode && typeDevForMove == 1)
        swp = new SelectWorkPlace(this, curOrgId,true,true,curWPId);
    else
        swp = new SelectWorkPlace(this, curOrgId,true);
    swp->setAttribute(Qt::WA_DeleteOnClose);
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(fillNewPlace(int,QString,int)));
    swp->exec();
}
void MoveDevice::fillNewPlace(int wpId, const QString &wpName, int orgId)
{
    newOrgId = orgId;
    newWP->setText(wpName);
    newWPId = wpId;
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT Name FROM departments WHERE id = %1").arg(orgId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    query.next();
    newOrg->setText(query.value(0).toString());
    ok = query.exec(QString("SELECT departments.id, departments.Name, departments.FP, departments.Firm FROM workerplace "
                            "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                            "WHERE workerplace.CodWorkerPlace = %1")
                    .arg(wpId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next();
        newDepId = query.value(0).toInt();
        if(query.value(2).toInt() != 1 && query.value(3).toInt() != 1)
            newDep->setText(query.value(1).toString());
        else
            newDep->setText("");
    }else{
        newDepId = 0;
        newDep->setText("");
    }
    ok = query.exec(QString("select p.id, p.Name from departments n, tree t, departments p "
                            "where n.id = %1 and n.id = t.id and t.parent_id = p.id AND p.FP = 1;").arg(newDepId));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next();
        newFilPredId = query.value(0).toInt();
        newFilPred->setText(query.value(1).toString());
    }else{
        newFilPredId = 0;
        newFilPred->setText("");
    }
    if(!m_singlMode){
        newOrgTex->setEnabled(true);
        newOrgTex->clearButton_clicked();
        if(curWPId != 0 && newWPId != 0)
            addDeviceForMove->setEnabled(true);
    }else{
        moveDev->setEnabled(dataForMoveEntered());
        if(typeDevForMove == 0){
            newOrgTex->setEnabled(true);
        }
    }
}
void MoveDevice::on_newOrgTex_runButtonClicked()
{
    QString filter;
    if(!m_singlMode)
        filter = QString("%2.id IN (SELECT id FROM device WHERE CodWorkerPlace = %1)"
                         " AND typedevice.Type = 1").arg(newWPId).arg(devModel->aliasModelTable());
    else
        filter = QString("%3.id IN (SELECT id FROM device WHERE CodWorkerPlace = %1)"
                         " AND typedevice.Type = 1 AND %3.id != %2")
                .arg(newWPId)
                .arg(moveDevModel->data(moveDevModel->index(0,devModel->cIndex.parent_id)).toString())
                .arg(devModel->aliasModelTable());
    SelectDevice *sd = new SelectDevice(this,filter,false,false,true,true);
    connect(sd,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(setOrgTex(QList<QVariant>)));
    sd->setAttribute(Qt::WA_DeleteOnClose);
    sd->exec();
}
void MoveDevice::setOrgTex(const QList<QVariant> &orgTex)
{
    newOrgTex->setText(orgTex.value(devModel->cIndex.name).toString());
    newOrgTex->setData(orgTex.value(devModel->cIndex.id));
}
void MoveDevice::on_addDeviceForMove_clicked()
{
    QString filter, dontShowDev,dontShowChildDev;
    SelectDevice *sdm;
    dontShowDev = ""; dontShowChildDev = "";
    if(moveDevModel->rowCount()>0){
        dontShowDev = QString("AND %1.id NOT IN ( ").arg(devModel->aliasModelTable());
        dontShowChildDev = QString("AND %2.parent_id NOT IN ( ").arg(devModel->aliasModelTable());
        if(moveDevModel->rowCount() == 1){
            dontShowDev += moveDevModel->data(moveDevModel->index(0,devModel->cIndex.id)).toString();
            dontShowChildDev += moveDevModel->data(moveDevModel->index(0,devModel->cIndex.id)).toString();
        }else{
            dontShowDev += moveDevModel->data(moveDevModel->index(0,devModel->cIndex.id)).toString();
            dontShowChildDev += moveDevModel->data(moveDevModel->index(0,devModel->cIndex.id)).toString();
            for(int i = 1; i<moveDevModel->rowCount();i++){
                dontShowDev += ","+moveDevModel->data(moveDevModel->index(i,devModel->cIndex.id)).toString();
                dontShowChildDev += ","+moveDevModel->data(moveDevModel->index(i,devModel->cIndex.id)).toString();
            }
        }
        dontShowDev += " )";
        dontShowChildDev += " )";
    }
    if(newOrgTex->data().toInt() != 0){
        filter = QString("%5.id IN (SELECT id FROM %6 WHERE CodWorkerPlace = %1)"
                         "AND (%5.id IN(select p.id from %6 n, %7 t, %6 p where "
                         "n.id in (select %5.id from %6 %5 "
                         "LEFT OUTER JOIN typedevice "
                         "ON %5.CodTypeDevice = typedevice.CodTypeDevice "
                         "where typedevice.Type = 0 and %5.parent_id !=0) "
                         "and n.id = t.id and t.parent_id = p.id and t.level > 0 GROUP BY p.id) or typedevice.Type = 0)"
                         "AND %5.id != %2 AND %5.parent_id != %2 %3 %4")
                .arg(curWPId)
                .arg(newOrgTex->data().toInt())
                .arg(dontShowDev)
                .arg(dontShowChildDev)
                .arg(devModel->aliasModelTable())
                .arg(devModel->nameModelTable())
                .arg(devModel->nameModelTreeTable());
        sdm = new SelectDevice(this,filter,true,true,false,true);
    }else{
        if((curWPId == newWPId) && newOrgTex->data().toInt() == 0){
            filter = QString("!(dev.parent_id = 0 AND typedevice.Type = 0) "
                             "AND %4.id IN (SELECT id FROM %5 WHERE CodWorkerPlace = %1) "
                             "AND (%4.id IN(select p.id from %5 n, %6 t, %5 p where "
                             "n.id in (select %4.id from %5 %4 "
                             "LEFT OUTER JOIN typedevice "
                             "ON %4.CodTypeDevice = typedevice.CodTypeDevice "
                             "where typedevice.Type = 0 and %4.parent_id !=0) "
                             "and n.id = t.id and t.parent_id = p.id and t.level > 0 GROUP BY p.id) or typedevice.Type = 0) "
                             "%2 %3")
                    .arg(curWPId)
                    .arg(dontShowDev)
                    .arg(dontShowChildDev)
                    .arg(devModel->aliasModelTable())
                    .arg(devModel->nameModelTable())
                    .arg(devModel->nameModelTreeTable());
            sdm = new SelectDevice(this,filter,true,true,false,true);
        }else{
            filter = QString("%4.id IN (SELECT id FROM %5 WHERE CodWorkerPlace = %1) %2 %3")
                    .arg(curWPId)
                    .arg(dontShowDev)
                    .arg(dontShowChildDev)
                    .arg(devModel->aliasModelTable())
                    .arg(devModel->nameModelTable());
            sdm = new SelectDevice(this,filter,true,false,false,true);
        }
    }
    connect(sdm,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(addDevForMove(QList<QVariant>)));
    connect(this,SIGNAL(lockedError(QString,bool)),sdm,SLOT(lockedErrorMessage(QString,bool)));
    sdm->setAttribute(Qt::WA_DeleteOnClose);
    sdm->exec();
}
void MoveDevice::addDevForMove(const QList<QVariant> &dev)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT c.id FROM %1 n, %2 t, %1 c WHERE n.id = %3 AND n.id = t.parent_id AND t.id = c.id;")
                    .arg(devModel->nameModelTable())
                    .arg(devModel->nameModelTreeTable())
                    .arg(dev.value(devModel->cIndex.id).toInt()));
    if(!ok){
        if(!m_singlMode)
            emit lockedError(tr("Не удалось получить список устройств для блокировки:\n %1\n").arg(query.lastError().text()),false);
        else
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось получить список устройств для блокировки:\n %1\n").arg(query.lastError().text()),
                                 tr("Закрыть"));
        return;
    }
    QList<int> lockedId;
    while(query.next())
        lockedId<<query.value(0).toInt();

    if(!lockedControl->recordsIsLosked(lockedId,
                                       devModel->colTabName.id,
                                       devModel->nameModelTable()))
    {
        if(lockedControl->lastError().type() != QSqlError::NoError){
            if(!m_singlMode)
                emit lockedError(tr("Не удалось получить информацию о блокировке:\n %1\n").arg(lockedControl->lastError().text()),false);
            else
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n").arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            return;
        }else{
            ok = query.exec(QString("SELECT %1, %2, %3 FROM %4 WHERE %5 = %6")
                            .arg(devModel->colTabName.rv)
                            .arg(devModel->colTabName.parent_id)
                            .arg(devModel->colTabName.codWorkerPlace)
                            .arg(devModel->nameModelTable())
                            .arg(devModel->colTabName.id)
                            .arg(dev.value(devModel->cIndex.id).toInt()));
            if(ok){
                query.next();
                if(dev.value(devModel->cIndex.rv).toInt() != query.value(0).toInt()){
                    if(query.value(2).toInt() != dev.value(devModel->cIndex.codWorkerPlace).toInt()){
                        if(!m_singlMode)
                            emit lockedError(tr("Устройство было перемещено,\n"
                                                "выберите устройство с нового места положения."),true);
                        else
                            QMessageBox::warning(this,tr("Ошибка!!!"),
                                                 tr("Устройство было перемещено,\n"
                                                    "выберите устройство с нового места положения."),
                                                 tr("Закрыть"));
                        return;
                    }else{
                        if(query.value(1).toInt() != dev.value(devModel->cIndex.parent_id).toInt()){
                            if(!m_singlMode)
                                emit lockedError(tr("Устройство было перемещено,\n"
                                                    "выберите устройство с нового места положения."),true);
                            else
                                QMessageBox::warning(this,tr("Ошибка!!!"),
                                                     tr("Устройство было перемещено,\n"
                                                        "выберите устройство с нового места положения."),
                                                     tr("Закрыть"));
                            return;
                        }else{
                            if(!m_singlMode)
                                emit lockedError(tr("Запись была изменена, будут загружены новые данные записи.\n"
                                                    "Повторите выбор записи."),true);
                            else
                                QMessageBox::warning(this,tr("Ошибка!!!"),
                                                     tr("Запись была изменена, будут загружены новые данные записи.\n"
                                                        "Повторите выбор записи."),
                                                     tr("Закрыть"));
                            return;
                        }
                    }
                }
            }else{
                if(!m_singlMode){
                    emit lockedError(tr("Не удалось получить информацию о версии записи:\n %1\n")
                                     .arg(query.lastError().text()),true);
                    return;
                }else
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось получить информацию о версии записи:\n %1\n")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
            }
            if(!lockedControl->lockListRecord(lockedId,
                                              devModel->colTabName.id,
                                              devModel->nameModelTable())){
                if(!m_singlMode)
                    emit lockedError(tr("Не удалось заблокировать выбраное устройство:\n %1\n").arg(lockedControl->lastError().text()),false);
                else
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось заблокировать выбраное устройство:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
    }else{
        if(!m_singlMode)
            emit lockedError(tr("Устройство или его состовляющие заблокированны.\n"
                                "Пользователь выполнивший блокировку: %1")
                             .arg(lockedControl->recordBlockingUser()),false);
        else
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Устройство или его состовляющие заблокированны.\n"
                                    "Пользователь выполнивший блокировку: %1")
                                 .arg(lockedControl->recordBlockingUser()),
                                 tr("Закрыть"));
        return;
    }
    moveDevModel->insertRow(moveDevModel->rowCount());
    deviceForMoveView->setCurrentIndex(moveDevModel->index(moveDevModel->rowCount()-1,0));
    for(int i = 0; i<dev.count();i++){
        moveDevModel->setData(moveDevModel->index(moveDevModel->rowCount()-1,i),dev.value(i));
    }
    if(moveDevModel->data(moveDevModel->index(moveDevModel->rowCount()-1,devModel->cIndex.iconPath)).toString().isEmpty() ||
            moveDevModel->data(moveDevModel->index(moveDevModel->rowCount()-1,devModel->cIndex.iconPath)).toString().isNull()){
        moveDevModel->item(moveDevModel->rowCount()-1)->setIcon(
                    QIcon(QDir::currentPath()+"/ico/typedeviceico/Default.png"));
    }else{
        moveDevModel->item(moveDevModel->rowCount()-1)->setIcon(
                    QIcon(QDir::currentPath()+"/ico/typedeviceico/"+
                         moveDevModel->data(moveDevModel->index(moveDevModel->rowCount()-1,devModel->cIndex.iconPath)).toString()));
    }
    if(!m_singlMode){
        delDeviceForMove->setEnabled(true);
        cancel->setEnabled(true);
        moveDev->setEnabled(dataForMoveEntered());
        newOrgTex->setEnabled(false);
        selectCur->setEnabled(false);
        selectNew->setEnabled(false);
    }else{
        newOrgTex->setEnabled(false);
        selectCur->setEnabled(false);
    }
    updateLockRecord();
}
void MoveDevice::on_cancel_clicked()
{
    if(moveDevModel->rowCount() <= 0)
        return;
    QSqlQuery query;
    bool ok;
    QString listId;
    listId = moveDevModel->index(0,devModel->cIndex.id).data().toString();
    for(int i = 1;i<moveDevModel->rowCount();i++)
        listId += ","+moveDevModel->index(i,devModel->cIndex.id).data().toString();
    ok = query.exec(QString("SELECT c.id FROM %1 n, %2 t, %1 c WHERE n.id IN (%3) AND n.id = t.parent_id AND t.id = c.id;")
                    .arg(devModel->nameModelTable())
                    .arg(devModel->nameModelTreeTable())
                    .arg(listId));
    moveDevModel->removeRows(0,moveDevModel->rowCount());
    delDeviceForMove->setEnabled(false);
    cancel->setEnabled(false);
    moveDev->setEnabled(dataForMoveEntered());
    if(!m_moveFromWp)
        selectCur->setEnabled(true);
    newOrgTex->setEnabled(true);
    if(!m_addWPMode)
        selectNew->setEnabled(true);

    if(!ok){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось снять блокировку:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }

    QList<int> lockedId;
    while(query.next())
        lockedId<<query.value(0).toInt();
    lockedControl->stopLockListRecordThread();
    if(!lockedControl->unlockListRecord(lockedId,
                                      devModel->colTabName.id,
                                      devModel->nameModelTable())){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось снять блокировку:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    }
}
void MoveDevice::on_delDeviceForMove_clicked()
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT c.id FROM %1 n, %2 t, %1 c WHERE n.id = %3 AND n.id = t.parent_id AND t.id = c.id;")
                    .arg(devModel->nameModelTable())
                    .arg(devModel->nameModelTreeTable())
                    .arg(moveDevModel->index(deviceForMoveView->currentIndex().row(),devModel->cIndex.id).data().toInt()));
    moveDevModel->removeRow(deviceForMoveView->currentIndex().row());
    if(moveDevModel->rowCount()<=0){
        delDeviceForMove->setEnabled(false);
        cancel->setEnabled(false);
        moveDev->setEnabled(dataForMoveEntered());
        newOrgTex->setEnabled(true);
        if(!m_moveFromWp)
            selectCur->setEnabled(true);
        if(!m_addWPMode)
            selectNew->setEnabled(true);
    }
    if(!ok){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось разблокировать запись:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }
    QList<int> lockedId;
    while(query.next())
        lockedId<<query.value(0).toInt();
    lockedControl->stopLockListRecordThread();
    if(!lockedControl->unlockListRecord(lockedId,
                                      devModel->colTabName.id,
                                      devModel->nameModelTable()))
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось разблокировать запись:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    updateLockRecord();
}
void MoveDevice::on_moveDev_clicked()
{
    if(m_singlMode){
        if((curWPId == newWPId) && newOrgTex->data().toInt() == 0 && moveDevModel->data(moveDevModel->index(0,devModel->cIndex.parent_id)).toInt() <= 0){
            QMessageBox::information(this, tr("Внимание"),
                                     tr("Вы не указали Оргтехнику назначения!!!\n"
                                        "Выберите Оргтехнику в \"Новое место нахождения\"."),
                                     tr("Закрыть"));
            return;
        }
    }
    bool moveLicenseToNewOrg = false;
    QSqlQuery query, addHistoryQuery;
    QString devForMove = "(";
    bool ok;

    devForMove += moveDevModel->data(moveDevModel->index(0,devModel->cIndex.id)).toString();
    for(int i = 1; i < moveDevModel->rowCount(); i++)
        devForMove += ","+moveDevModel->data(moveDevModel->index(i,devModel->cIndex.id)).toString();
    devForMove += ")";

    if(newOrgTex->data().toInt() == 0 && curOrgId != newOrgId){
        int button = QMessageBox::question(this, tr("Внимание"),
                                 tr("Вы указали для перемещения другую организацию,"
                                    "при перемещении в другую организацию вы можите вместе с выбранными устройствами "
                                    "переместить привязанные к ним лицензии. Если вы не хотите перемещать лицензии вместе "
                                    "с устройствами, тогда связи выбранных устройств с лицензиями будт удалены и "
                                    "лицензии не будут перемещены.\n\nВы хотите переместить лицензии вместе с устройствами?"),
                                 tr("Да"),tr("Нет"),"",1,1);
        if(button == 0){
            ok = query.exec(QString("SELECT CodLicense FROM licenseanddevice WHERE CodDevice IN %2").arg(devForMove));
            if(!ok){
                QMessageBox::warning(this, tr("Ошибка!!!"),
                                     tr("Не удалось получить список лицензий привязанных к устройствам, лицензии не будут перемещены:\n%1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
            }else{
                if(query.size() > 0){
                    while(query.next())
                        lockedLicenseId<<query.value(0).toInt();
                    if(lockedControlLicense->recordsIsLosked(lockedLicenseId,"`key`","licenses")){
                        for(int i = 0; i<lockedLicenseId.size();i++){
                            if(lockedControlLicense->recordIsLosked(lockedLicenseId.value(i),"`key`","licenses")){
                                int button = QMessageBox::question(this,tr("Ошибка!!!"),
                                                                   tr("Как минимум одна лицензия заблокирована.\n"
                                                                      "Пользователь выполнивший блокировку: %1.\n\n"
                                                                      "Перемещение лицензий не возможно, вы хотите продолжить "
                                                                      "перемещение устройств без лицензий?")
                                                                   .arg(lockedControlLicense->recordBlockingUser()),
                                                                   tr("Да"),tr("Нет"),"",1,1);
                                if(button == 1)
                                    return;
                            }
                        }
                    }else{
                        if(!lockedControlLicense->lockListRecord(lockedLicenseId,"`key`","licenses")){
                            int button = QMessageBox::question(this,tr("Ошибка!!!"),
                                                               tr("Не удалось заблокировать записи перемещаемых лицензий:\n %1\n\n"
                                                                  "Перемещение лицензий не возможно, вы хотите продолжить "
                                                                  "перемещение устройств без лицензий?")
                                                               .arg(lockedControlLicense->lastError().text()),
                                                               tr("Да"),tr("Нет"),"",1,1);
                            if(button == 1)
                                return;
                        }else{
                            moveLicenseToNewOrg = true;
                            lockedControlLicense->lockListRecordThread(lockedLicenseId,"`key`","licenses");
                        }
                    }
                }
            }
        }
    }

    // Если было укзанно родительское устройство
    if(newOrgTex->data().toInt() != 0){
        // изменяем родителя и рабочее место у выбранных устройств
        ok = query.exec(QString("UPDATE %4 SET parent_id = %1, CodWorkerPlace = %2 WHERE id IN %3")
                        .arg(newOrgTex->data().toInt()).arg(newWPId).arg(devForMove).arg(devModel->nameModelTable()));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось изменить рабочее место у устройств:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }

        // Если была выбранна другая организация, изменяем организацию у устройств
        if(curOrgId != newOrgId){
            ok = query.exec(QString("UPDATE %3 SET CodOrganization = %1 WHERE id IN %2")
                            .arg(newOrgId).arg(devForMove).arg(devModel->nameModelTable()));
            if(!ok){
                QMessageBox::warning(this, tr("Ошибка!!!"),
                                     tr("Не удалось изменить организацию у устройств:\n%1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
                return;
            }
        }

        // Обновляем версию записей
        ok = query.exec(QString("SELECT id, rv FROM %2 WHERE id IN %1")
                        .arg(devForMove).arg(devModel->nameModelTable()));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось получить информацию о версиях записей устройств:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(query.size() > 0){
            QMap<int,int> rowVersion;
            while(query.next()){
                if(query.value(1).toInt() == 254)
                    rowVersion[query.value(0).toInt()] = 0;
                else
                    rowVersion[query.value(0).toInt()] = query.value(1).toInt()+1;
            }
            QMap<int,int>::const_iterator i;
            for (i = rowVersion.constBegin(); i != rowVersion.constEnd(); ++i){
                ok = query.exec(QString("UPDATE %3 SET RV = %1 WHERE id = %2")
                                .arg(i.value()).arg(i.key()).arg(devModel->nameModelTable()));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                         tr("Не удалось обновить версию записей перемещаемых устройств:\n%1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                    return;
                }
            }
        }
    }else{
        // Если родительское устройство небыло выбранно
        // Удаляем родителей устройств
        ok = query.exec(QString("UPDATE %2 SET parent_id = 0 WHERE id IN %1")
                        .arg(devForMove).arg(devModel->nameModelTable()));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось внести изменения в базу данных:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        // Изменяем рабочие места у выбранных устройств и их детей
        ok = query.exec(QString("BEGIN; "
                                "DROP TEMPORARY TABLE IF EXISTS my_temp_table; "
                                "CREATE TEMPORARY TABLE my_temp_table(id INT NOT NULL); "
                                "INSERT INTO my_temp_table ( "
                                "SELECT c.id FROM %3 n, %4 t, %3 c WHERE "
                                "n.id IN %2 AND n.id = t.parent_id AND t.id = c.id); "
                                "UPDATE %3 SET CodWorkerPlace = %1 WHERE id IN (SELECT id FROM my_temp_table); "
                                "DROP TEMPORARY TABLE IF EXISTS my_temp_table; "
                                "COMMIT; ")
                        .arg(newWPId).arg(devForMove).arg(devModel->nameModelTable()).arg(devModel->nameModelTreeTable()));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось внести изменения в базу данных:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        // Если была выбранна другая организация, изменяем организацию у устройств и привязанных к ним лицензий
        if(curOrgId != newOrgId){
            ok = query.exec(QString("BEGIN; "
                                    "DROP TEMPORARY TABLE IF EXISTS my_temp_table; "
                                    "CREATE TEMPORARY TABLE my_temp_table(id INT NOT NULL); "
                                    "INSERT INTO my_temp_table ( "
                                    "SELECT c.id FROM %3 n, %4 t, %3 c WHERE "
                                    "n.id IN %2 AND n.id = t.parent_id AND t.id = c.id); "
                                    "UPDATE %3 SET CodOrganization = %1 WHERE id IN (SELECT id FROM my_temp_table); "
                                    "DROP TEMPORARY TABLE IF EXISTS my_temp_table; "
                                    "COMMIT; ")
                            .arg(newOrgId).arg(devForMove).arg(devModel->nameModelTable()).arg(devModel->nameModelTreeTable()));
            if(!ok){
                QMessageBox::warning(this, tr("Ошибка!!!"),
                                     tr("Не удалось внести изменения в базу данных:\n%1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
                return;
            }
            // Если был получен положительный ответ на вопрос о перемещении лицензий
            if(moveLicenseToNewOrg){
                ok = query.exec(QString("UPDATE licenses SET CodOrganization = %1 WHERE `key` IN ("
                                        "SELECT CodLicense FROM licenseanddevice WHERE CodDevice IN %2)")
                                .arg(newOrgId).arg(devForMove));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                         tr("Не удалось переместить привязанные лицензии, связм лицензий с устройсвами будут удалены:\n%1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                    ok = query.exec(QString("DELETE FROM licenseanddevice WHERE CodDevice IN %1").arg(devForMove));
                    if(!ok){
                        QMessageBox::warning(this, tr("Ошибка!!!"),
                                             tr("Не удалось удалить привязки лицензий к перемещаемым устройствам:\n%1")
                                             .arg(query.lastError().text()),
                                             tr("Закрыть"));
                    }
                }else{
                    ok = query.exec(QString("SELECT `key`, rv FROM licenses WHERE `key` IN ("
                                            "SELECT CodLicense FROM licenseanddevice WHERE CodDevice IN %2)")
                                    .arg(devForMove));
                    if(!ok)
                        QMessageBox::warning(this, tr("Ошибка!!!"),
                                             tr("Не удалось получить информацию о версиях записей перемещаемых лицензий:\n%1")
                                             .arg(query.lastError().text()),
                                             tr("Закрыть"));
                    else{
                        if(query.size() > 0){
                            QMap<int,int> rowVersion;
                            while(query.next()){
                                if(query.value(1).toInt() == 254)
                                    rowVersion[query.value(0).toInt()] = 0;
                                else
                                    rowVersion[query.value(0).toInt()] = query.value(1).toInt()+1;
                            }
                            QMap<int,int>::const_iterator i;
                            for (i = rowVersion.constBegin(); i != rowVersion.constEnd(); ++i){
                                ok = query.exec(QString("UPDATE licenses SET RV = %1 WHERE `key` = %2")
                                                .arg(i.value()).arg(i.key()));
                                if(!ok){
                                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                                         tr("Не удалось обновить версии строк перемещаемых лицензий:\n%1")
                                                         .arg(query.lastError().text()),
                                                         tr("Закрыть"));
                                }
                            }
                        }
                    }
                }
                lockedControlLicense->stopLockListRecordThread();
                if(!lockedControlLicense->unlockListRecord(lockedLicenseId,"`key`","licenses"))
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось разблокировать запись:\n %1\n")
                                         .arg(lockedControlLicense->lastError().text()),
                                         tr("Закрыть"));
                lockedLicenseId.clear();
            }else{
                // Если от перемещения лицензий отказались
                ok = query.exec(QString("DELETE FROM licenseanddevice WHERE CodDevice IN %1").arg(devForMove));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                         tr("Не удалось удалить привязки лицензий к перемещаемым устройствам:\n%1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                }
            }
        }

        ok = query.exec(QString("SELECT c.id, c.rv FROM %2 n, %3 t, %2 c "
                                "WHERE n.id IN %1 AND n.id = t.parent_id AND t.id = c.id")
                        .arg(devForMove).arg(devModel->nameModelTable()).arg(devModel->nameModelTreeTable()));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось получить информацию о версиях записей устройств:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
        }
        if(query.size() > 0){
            QMap<int,int> rowVersion;
            while(query.next()){
                if(query.value(1).toInt() == 254)
                    rowVersion[query.value(0).toInt()] = 0;
                else
                    rowVersion[query.value(0).toInt()] = query.value(1).toInt()+1;
            }
            QMap<int,int>::const_iterator i;
            for (i = rowVersion.constBegin(); i != rowVersion.constEnd(); ++i){
                ok = query.exec(QString("UPDATE %3 SET RV = %1 WHERE id = %2")
                                .arg(i.value()).arg(i.key()).arg(devModel->nameModelTable()));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                         tr("Не удалось обновить версию записей перемещаемых устройств:\n%1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                }
            }
        }
    }

    // Записываем перемещения в журнал
    QString oldPlace, newPlace;
    oldPlace = curOrg->text();
    newPlace = newOrg->text();
    if (!curFilPred->text().isNull() && !curFilPred->text().isEmpty())
        oldPlace += "/"+curFilPred->text();
    if (!newFilPred->text().isNull() && !newFilPred->text().isEmpty())
        newPlace += "/"+newFilPred->text();
    if (!curDep->text().isNull() && !curDep->text().isEmpty())
        oldPlace += "/"+curDep->text();
    if (!newDep->text().isNull() && !newDep->text().isEmpty())
        newPlace += "/"+newDep->text();
    oldPlace += "/"+curWP->text();
    newPlace += "/"+newWP->text();

    // Записываем в журнал перемещения устройствв
    for(int i = 0; i < moveDevModel->rowCount(); i++){
        addHistoryQuery.prepare("INSERT INTO historymoved (CodMovedDevice,DateMoved,OldPlace,NewPlace,"
                                "CodCause,CodPerformer,Note,TypeHistory,CodOldPlace,CodNewPlace) VALUES (?,?,?,?,?,?,?,?,?,?)");
        addHistoryQuery.addBindValue(moveDevModel->data(moveDevModel->index(i,devModel->cIndex.id)).toInt());
        addHistoryQuery.addBindValue(dateMoved->dateTime());
        addHistoryQuery.addBindValue(oldPlace);
        addHistoryQuery.addBindValue(newPlace);
        addHistoryQuery.addBindValue(cause->itemData(cause->currentIndex()).toInt());
        addHistoryQuery.addBindValue(performer->data().toInt());
        addHistoryQuery.addBindValue(note->toPlainText());
        addHistoryQuery.addBindValue(0);
        addHistoryQuery.addBindValue(curWPId);
        addHistoryQuery.addBindValue(newWPId);
        addHistoryQuery.exec();
        if (addHistoryQuery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить историю перемещения устройств:\n %1")
                                     .arg(addHistoryQuery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
    }
    // Записываем в журнал перемещения лицензий
    if(newOrgTex->data().toInt() == 0 && curOrgId != newOrgId &&moveLicenseToNewOrg){
        ok = query.exec(QString("SELECT CodLicense FROM licenseanddevice WHERE CodDevice IN %1").arg(devForMove));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось получить список перемещаемых лицензий:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        while(query.next()){
            addHistoryQuery.prepare("INSERT INTO historymoved (CodMovedLicense,DateMoved,OldPlace,NewPlace,"
                                    "CodCause,CodPerformer,Note,TypeHistory) VALUES (?,?,?,?,?,?,?,?)");
            addHistoryQuery.addBindValue(query.value(0).toInt());
            addHistoryQuery.addBindValue(dateMoved->dateTime());
            addHistoryQuery.addBindValue(curOrg->text());
            addHistoryQuery.addBindValue(newOrg->text());
            addHistoryQuery.addBindValue(cause->itemData(cause->currentIndex()).toInt());
            addHistoryQuery.addBindValue(performer->data().toInt());
            addHistoryQuery.addBindValue(tr("Перемещение совместно с привязанным устройством"));
            addHistoryQuery.addBindValue(1);
            addHistoryQuery.exec();
            if (addHistoryQuery.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось добавить историю перемещения лицензий:\n %1")
                                         .arg(addHistoryQuery.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
    }

    // Выводим сообщение об успешном перемещении
    if(!m_singlMode){
        emit devIsMoved();
        if(!m_addWPMode){
            if(!m_moveFromWp){
                on_cancel_clicked();
                QMessageBox::information(this, tr(" "),
                                         tr("Перемещенье выполненно успешно!"),
                                         tr("Закрыть"));
            }else{
                MoveDevice::close();
                QMessageBox::information(this, tr(" "),
                                         tr("Перемещенье выполненно успешно!"),
                                         tr("Закрыть"));
            }
        }else{
            MoveDevice::close();
            QMessageBox::information(this, tr(" "),
                                     tr("Устройства успешно добавленны!"),
                                     tr("Закрыть"));
        }
    }else{
        emit devIsMoved(moveDevModel->data(moveDevModel->index(0,devModel->cIndex.type)).toInt(),newOrgTex->data().toInt());
        MoveDevice::close();
        QMessageBox::information(this, tr(" "),
                                 tr("Перемещенье выполненно успешно!"),
                                 tr("Закрыть"));
    }
}
void MoveDevice::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    cBox->addItem(nullStr);
    QSqlQuery query;
    if(filter.isNull() || filter.isEmpty())
        query.exec(QString("SELECT %2,name FROM %1;").arg(tableName).arg(idName));
    else
        query.exec(QString("SELECT %2,name FROM %1 WHERE %3;").arg(tableName).arg(idName).arg(filter));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось заполнить комбобокс %2:\n %1")
                                 .arg(query.lastError().text())
                                 .arg(cBox->objectName()),
                                 tr("Закрыть"));
        return;
    }
    cBox->setMaxCount(query.size()+1);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}
void MoveDevice::on_note_textChanged()
{
    if(note->toPlainText().count() > 255){
        QString str;
        str = note->toPlainText().mid(0,255);
        note->setText(str);
        QTextCursor cursor = note->textCursor();
        cursor.movePosition(QTextCursor::End);
        note->setTextCursor(cursor);
    }
}
void MoveDevice::on_buttonEditCause_clicked()
{
    CeditTable edittable(this,QString("cause"));
    edittable.setWindowTitle(tr("Редактирование причин"));
    edittable.exec();
    cause->clear();
    populateCBox("CodCause","cause","",tr("<Выберите причину>"),cause);
}
void MoveDevice::on_performer_runButtonClicked()
{
    AddExistingUser *aeu = new AddExistingUser(this,curOrgId);
    aeu->setWindowTitle(tr("Выбор исполнителя"));
    aeu->setAddButtonName(tr("Выбрать"));
    connect(aeu,SIGNAL(userAdded(QString,int)),this,SLOT(setPerformer(QString,int)));
    aeu->exec();
}
void MoveDevice::setPerformer(const QString &performerName, int performerId)
{
    performer->setText(performerName);
    performer->setData(performerId);
    moveDev->setEnabled(dataForMoveEntered());
}
bool MoveDevice::dataForMoveEntered()
{
    if(cause->currentIndex() > 0 && !performer->data().isNull() && performer->data().toInt() > 0 && moveDevModel->rowCount() > 0 &&
            curWPId > 0 && newWPId > 0)
        return true;
    else
        return false;
}
void MoveDevice::on_cause_currentIndexChanged(int)
{
    moveDev->setEnabled(dataForMoveEntered());
}
void MoveDevice::updateLockRecord()
{
    if(moveDevModel->rowCount() <= 0)
        return;
    QSqlQuery query;
    bool ok;
    QString listId;

    listId = moveDevModel->index(0,devModel->cIndex.id).data().toString();
    for(int i = 1;i<moveDevModel->rowCount();i++)
        listId += ","+moveDevModel->index(i,devModel->cIndex.id).data().toString();
    ok = query.exec(QString("SELECT c.id FROM %1 n, %2 t, %1 c WHERE n.id IN (%3) AND n.id = t.parent_id AND t.id = c.id;")
                    .arg(devModel->nameModelTable())
                    .arg(devModel->nameModelTreeTable())
                    .arg(listId));
    if(!ok){
        lockedControl->stopLockListRecordThread();
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }

    QList<int> lockedId;
    while(query.next())
        lockedId<<query.value(0).toInt();
    lockedControl->stopLockListRecordThread();
    lockedControl->lockListRecordThread(lockedId,devModel->colTabName.id,devModel->nameModelTable());
}
void MoveDevice::on_closeButton_clicked()
{
    on_cancel_clicked();
    reject();
}
void MoveDevice::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
void MoveDevice::closeEvent(QCloseEvent *event){
    on_cancel_clicked();
    event->accept();
}
