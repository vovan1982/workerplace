#include <QtWidgets>
#include <QtSql>
#include <QMenu>
#include <QMovie>
#include <QMessageBox>
#include <QClipboard>
#include "headers/device.h"
#include "headers/movedevice.h"
#include "headers/addeditdevice.h"
#include "headers/workplacemodel.h"
#include "headers/selectworkplace.h"
#include "headers/devicemodel.h"
#include "headers/devicemodelcontrol.h"
#include "headers/filterdevice.h"
#include "headers/journalhistorymoved.h"
#include "headers/lockdatabase.h"

Device::Device(QWidget *parent, bool wpMode, int wpId, int wpFirmId, bool readOnly) :
    QWidget(parent), m_wpMode(wpMode), m_wpId(wpId), m_wpFirmId(wpFirmId), m_readOnly(readOnly)
{
    setupUi(this);
    if(m_readOnly) setWindowTitle(windowTitle()+tr(" - [Только чттение]"));
    loadIndicatorIsShowed = false;
    filterIsSet = false;
    deviceFilter = "";
    wpDeviceFilter = "";
    QActionGroup *ag = new QActionGroup(this);
    ag->setExclusive(true);
    if(wpMode){
        groupBox->setVisible(false);
        groupBox_4->setVisible(false);
        buttonClose->setVisible(false);
        horizontalLayout_3->removeItem(horizontalSpacer);
        ag->addAction(actionAddDevice);
        wpDeviceFilter = QString("dev.id IN (SELECT id FROM device WHERE CodWorkerPlace = %1)")
                         .arg(wpId);
        deviceFilter = wpDeviceFilter;
    }
    ag->addAction(actionAddNewDevice);
    ag->addAction(actionAddNewDiviceInComposition);
    if(!wpMode)ag->addAction(actionDelDevice);
    ag->addAction(actionEditDevice);
    ag->addAction(actionMoveDevice);
    ag->addAction(actionHistoryMoved);
    ag->addAction(actionCopyDeviceInBufer);
    menu = new QMenu(tr("Устройства"), this);
    menu->addActions(ag->actions());
    buttonDevice->setMenu(menu);

    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());

    if(!wpMode)
        devModel = new DeviceModelControl(deviceView,deviceView,"default",db,wpDeviceFilter,false);
    else{
        devModel = new DeviceModelControl(deviceView,deviceView,"workPlace",db,wpDeviceFilter,false);
    }
    connect(devModel,SIGNAL(dataIsPopulated()),this,SLOT(dataIsLoaded()));
    connect(devModel,SIGNAL(devModelUpdated()),this,SIGNAL(devModelUpdated()));
    connect(buttonUpdate,SIGNAL(clicked()),this,SLOT(updateDevModel()));
    connect(deviceView,SIGNAL(collapsed(QModelIndex)),this,SLOT(setCurIndexIfCollapsed(QModelIndex)));
    connect(deviceView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setAccessToActions(QModelIndex)));
    connect(deviceView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onDevMenu(const QPoint &)));
    connect(deviceView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(doubleClickedDeviceView(QModelIndex)));
    connect(this, SIGNAL(loadIndicatorShowed()), SLOT(showLoadIndicator()), Qt::QueuedConnection);

    if(!wpMode){
        wpModel = new WorkPlaceModel(this);
        connect(deviceView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                            this, SLOT(convertIndexForUpdateWp(QModelIndex)));
    }else{
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("SELECT Name FROM workerplace WHERE CodWorkerPlace = %1").arg(m_wpId));
        if(!ok)
            qDebug()<<query.lastError().text();
        else{
            if(query.size() > 0){
                query.next();
                wpName = query.value(0).toString();
            }else
                wpName = "";
        }
    }
    devModel->updateDevModel();
}
void Device::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
void Device::showEvent(QShowEvent *e)
{
    QWidget::showEvent( e );
    if (!loadIndicatorIsShowed){
        emit loadIndicatorShowed();
        loadIndicatorIsShowed = true;
    }
}
void Device::dataIsLoaded()
{
    loadIndicatorIsShowed = true;
    if(devModel->model()->lastError().type() == QSqlError::NoError){
        if(devModel->model()->rowCount() != 0)
            devModel->setCurrentIndexFirstRow();
        actionAddDevice->setEnabled(true);
        actionAddNewDevice->setEnabled(true);
        buttonUpdate->setEnabled(true);
        buttonCopyAllInBufer->setEnabled(true);
        setAccessToActions();
        emit loadIndicatorClosed();
    }else{
        emit loadIndicatorClosed();
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Ошибка получения данных об устройствах:\n %1").arg(devModel->model()->lastError().text()),
                                 tr("Закрыть"));
    }
}
void Device::onDevMenu(const QPoint &p){
   menu->exec(deviceView->viewport()->mapToGlobal(p));
}
void Device::doubleClickedDeviceView(const QModelIndex &index){
    QModelIndex curViewIndex = devModel->realModelIndex(index);
    if(devModel->model()->rowCount(devModel->model()->index(curViewIndex.row(),0,curViewIndex.parent())) <= 0 || curViewIndex.column() > 0)
        on_actionEditDevice_triggered();
}
void Device::convertIndexForUpdateWp(const QModelIndex &index)
{
    updateWpModel(devModel->realModelIndex(index));
}
void Device::updateWpModel(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    QSqlQuery query;
    bool ok;
    filial->clear();
    organization->clear();
    ok = query.exec(QString("select p.id, p.name, p.Firm from departments n, tree t, departments p where n.id =  "
                       "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = "
                       "(SELECT CodWorkerPlace FROM device WHERE id = %1)) "
                       "and n.id = t.id and t.parent_id = p.id and (p.FP = 1 or p.Firm = 1);")
                    .arg(devModel->model()->index(index.row(),
                                                  devModel->model()->cIndex.id,
                                                  index.parent()
                                                  ).data().toInt()
                         )
                    );
    if(!ok)
        qDebug()<<query.lastError().text();
    else{
        if(query.size() > 0){
            while(query.next()){
                if(query.value(2).toInt())
                    organization->setText(query.value(1).toString());
                else
                    filial->setText(query.value(1).toString());
            }
        }else
            organization->setText(devModel->model()->data(
                                      devModel->model()->index(curViewIndex.row(),
                                                               devModel->model()->cIndex.orgName,
                                                               curViewIndex.parent()
                                                               )
                                      ).toString()
                                  );
    }
    populateWpModel(devModel->model()->index(index.row(),
                                             devModel->model()->cIndex.id,
                                             index.parent()
                                             ).data().toInt()
                    );
    if(wpModel->rowCount() == 0)
        clearWpForm();
    else{
        if(wpModel->index(0,8).data().toInt() != 1 && wpModel->index(0,9).data().toInt() != 1){
            WPName->setText(wpModel->index(0,3).data().toString());
            WPUser->setText(wpModel->index(0,5).data().toString());
            WPDivision->setText(wpModel->index(0,2).data().toString());
        }else{
            WPName->setText(wpModel->index(0,3).data().toString());
            WPUser->setText("");
            WPDivision->setText("");
        }
    }
}

void Device::clearWpForm()
{
    WPName->setText("");
    WPUser->setText("");
    WPDivision->setText("");
}

void Device::populateWpModel(int filter)
{
    wpModel->setFilter(QString("CodWorkerPlace IN (SELECT CodWorkerPlace FROM device WHERE id = %1) ")
                       .arg(filter));
    wpModel->select();
    if (wpModel->lastError().type() != QSqlError::NoError){
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка обновления рабочего места:\n %1").arg(wpModel->lastError().text()),
                                 tr("Закрыть"));
    }
}

void Device::setDeviceFilter(const QString &filter)
{
    if(!wpDeviceFilter.isEmpty() && !filter.isEmpty())
        deviceFilter = wpDeviceFilter+" AND "+filter;
    if(!wpDeviceFilter.isEmpty() && filter.isEmpty())
        deviceFilter = wpDeviceFilter;
    if(wpDeviceFilter.isEmpty() && !filter.isEmpty())
        deviceFilter = filter;
    if(wpDeviceFilter.isEmpty() && filter.isEmpty())
        deviceFilter = "";
    showLoadIndicator();
    devModel->setDevFilter(deviceFilter);
    if(filter.isNull() || filter.isEmpty()){
        filterIsSet = false;
    }else{
        filterIsSet = true;
    }
}

void Device::updateDevModel()
{
    showLoadIndicator();
    devModel->updateDevModel();
}

void Device::insertNewRowOfDB(int id)
{
    QModelIndex parent, curIndex;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(devModel->model()->index(curViewIndex.row(),
                                devModel->model()->cIndex.type,
                                curViewIndex.parent())
            .data().toInt() == 1)
        parent = devModel->model()->index(curViewIndex.row(),0,curViewIndex.parent());
    else
        parent = curViewIndex.parent();
    devModel->model()->insertRow(devModel->model()->rowCount(parent),parent);
    curIndex = devModel->model()->index(devModel->model()->rowCount(parent)-1,0,parent);
    devModel->model()->setDataWithOutSQL(devModel->model()->index(curIndex.row(),devModel->model()->cIndex.id,curIndex.parent()),id);
    devModel->model()->updateRow(curIndex.row(),curIndex.parent());
    deviceView->setCurrentIndex(devModel->realViewIndex(curIndex));
}

void Device::insertNewRowOfDB(int id, int parent_id)
{
    QModelIndex parent, curIndex;
    if(parent_id > 0 )
        parent = devModel->model()->findData(parent_id);
    else
        parent = QModelIndex();
    devModel->model()->insertRow(devModel->model()->rowCount(parent),parent);
    curIndex = devModel->model()->index(devModel->model()->rowCount(parent)-1,0,parent);
    devModel->model()->setDataWithOutSQL(devModel->model()->index(curIndex.row(),devModel->model()->cIndex.id,curIndex.parent()),id);
    devModel->model()->updateRow(curIndex.row(),curIndex.parent());
    deviceView->setCurrentIndex(devModel->realViewIndex(curIndex));
}

void Device::updateDevRow()
{
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    devModel->model()->updateRow(curViewIndex.row(),curViewIndex.parent());
}

void Device::afterMove(int type, int orgTexId)
{
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(type == 1){
        if(!m_wpMode){
            devModel->model()->updateRow(curViewIndex.row());
            organization->setText(devModel->model()->data(
                                      devModel->model()->index(curViewIndex.row(),
                                                               devModel->model()->cIndex.orgName)
                                      ).toString());
            if(devModel->model()->rowCount(curViewIndex) > 0)
                for(int i=0;i<devModel->model()->rowCount(curViewIndex);i++)
                    devModel->model()->updateRow(i,curViewIndex);
            updateWpModel(curViewIndex);
        }else{
            devModel->model()->removeRow(curViewIndex.row());
            emit devModelUpdated();
        }
    }
    if(type == 0 &&
            devModel->model()->data(
                devModel->model()->index(curViewIndex.row(),
                                         devModel->model()->cIndex.parent_id,
                                         curViewIndex.parent())
                ).toInt() == 0 &&
            orgTexId == 0){
        if(!m_wpMode){
            devModel->model()->updateRow(curViewIndex.row(),curViewIndex.parent());
            organization->setText(devModel->model()->data(
                                      devModel->model()->index(curViewIndex.row(),
                                                               devModel->model()->cIndex.orgName,
                                                               curViewIndex.parent())
                                      ).toString());
            if(devModel->model()->rowCount(curViewIndex) > 0)
                for(int i=0;i<devModel->model()->rowCount(curViewIndex);i++)
                    devModel->model()->updateRow(i,curViewIndex);
            updateWpModel(curViewIndex);
        }else{
            devModel->model()->removeRow(curViewIndex.row());
            emit devModelUpdated();
        }
    }
    if(type == 0 &&
            devModel->model()->data(
                devModel->model()->index(curViewIndex.row(),
                                         devModel->model()->cIndex.parent_id,
                                         curViewIndex.parent())
                ).toInt() == 0 &&
            orgTexId != 0){
        int id = devModel->model()->data(
                    devModel->model()->index(curViewIndex.row(),
                                             devModel->model()->cIndex.id,
                                             curViewIndex.parent())
                    ).toInt();

        devModel->setDevFilter(deviceFilter);

        QModelIndex index = devModel->model()->findData(id);
        if(index.isValid()){
            deviceView->setCurrentIndex(devModel->realViewIndex(index));
            deviceView->expand(curViewIndex.parent());
        }else
            devModel->setCurrentIndexFirstRow();
        emit devModelUpdated();
    }
    if(type == 0 &&
            devModel->model()->data(
                devModel->model()->index(curViewIndex.row(),
                                         devModel->model()->cIndex.parent_id,
                                         curViewIndex.parent())
                ).toInt() != 0 &&
            orgTexId == 0){
        int id = devModel->model()->data(
                    devModel->model()->index(curViewIndex.row(),
                                             devModel->model()->cIndex.id,
                                             curViewIndex.parent())
                    ).toInt();

        devModel->setDevFilter(deviceFilter);

        QModelIndex index = devModel->model()->findData(id);
        if(index.isValid())
            deviceView->setCurrentIndex(devModel->realViewIndex(index));
        else
            devModel->setCurrentIndexFirstRow();
        emit devModelUpdated();
    }
    if(type == 0 &&
            devModel->model()->data(
                devModel->model()->index(curViewIndex.row(),
                                         devModel->model()->cIndex.parent_id,
                                         curViewIndex.parent())
                ).toInt() != 0 &&
            orgTexId != 0){
        int id = devModel->model()->data(
                    devModel->model()->index(curViewIndex.row(),
                                             devModel->model()->cIndex.id,
                                             curViewIndex.parent())
                    ).toInt();

        devModel->setDevFilter(deviceFilter);

        QModelIndex index = devModel->model()->findData(id);
        if(index.isValid()){
            deviceView->setCurrentIndex(devModel->realViewIndex(index));
            deviceView->expand(curViewIndex.parent());
        }else
            devModel->setCurrentIndexFirstRow();
        emit devModelUpdated();
    }
}

void Device::setCurIndexIfCollapsed(const QModelIndex &index)
{
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(curViewIndex.parent() == devModel->realModelIndex(index))
        deviceView->setCurrentIndex(index);
}

void Device::setAccessToActions(const QModelIndex &index)
{
    QModelIndex curViewIndex = devModel->realModelIndex(index);
    if(devModel->model()->rowCount() == 0){
        actionAddNewDiviceInComposition->setEnabled(false);
        actionDelDevice->setEnabled(false);
        actionMoveDevice->setEnabled(false);
        actionEditDevice->setEnabled(false);
        actionHistoryMoved->setEnabled(false);
        actionCopyDeviceInBufer->setEnabled(false);
        buttonMove->setEnabled(false);
        buttonDeviceFilter->setEnabled(filterIsSet);
        buttonClearDeviceFilter->setEnabled(filterIsSet);
        if(m_readOnly){
            actionAddDevice->setEnabled(false);
            actionAddNewDevice->setEnabled(false);
        }
    }else{
        if(!m_readOnly){
            if(curViewIndex.isValid()){
                actionDelDevice->setEnabled(true);
                actionMoveDevice->setEnabled(true);
                actionEditDevice->setEnabled(true);
                actionHistoryMoved->setEnabled(true);
                actionCopyDeviceInBufer->setEnabled(true);
                buttonMove->setEnabled(true);
                buttonDeviceFilter->setEnabled(true);
                buttonClearDeviceFilter->setEnabled(filterIsSet);
                if(devModel->model()->index(curViewIndex.row(),
                                            devModel->model()->cIndex.parent_id,
                                            curViewIndex.parent()
                                            ).data().toInt() == 0){
                    if(devModel->model()->index(curViewIndex.row(),
                                                devModel->model()->cIndex.type,
                                                curViewIndex.parent()
                                                ).data().toInt() == 1){
                        actionAddNewDiviceInComposition->setEnabled(true);
                    }else{
                        actionAddNewDiviceInComposition->setEnabled(false);
                    }
                }else{
                    actionAddNewDiviceInComposition->setEnabled(true);
                }
            }
        }else{
            actionDelDevice->setEnabled(false);
            actionMoveDevice->setEnabled(false);
            actionEditDevice->setEnabled(true);
            actionHistoryMoved->setEnabled(true);
            actionCopyDeviceInBufer->setEnabled(true);
            buttonMove->setEnabled(false);
            buttonDeviceFilter->setEnabled(true);
            buttonClearDeviceFilter->setEnabled(filterIsSet);
            actionAddDevice->setEnabled(false);
            actionAddNewDevice->setEnabled(false);
            actionAddNewDiviceInComposition->setEnabled(false);
        }
    }
}

void Device::afterMove()
{
    showLoadIndicator();
    devModel->updateDevModel();
}

void Device::on_actionAddNewDevice_triggered()
{
    AddEditDevice *aed;
    if(!m_wpMode){
        aed = new AddEditDevice(this);
        connect(aed,SIGNAL(deviceAdded()),this,SLOT(updateDevModel()));
    }else{
        aed = new AddEditDevice(this,1,0,m_wpId,wpName,false,false,QList<QVariant>(),true,m_wpFirmId);
        connect(aed,SIGNAL(deviceAdded(int,int)),this,SLOT(insertNewRowOfDB(int,int)));
    }
    aed->exec();
}

void Device::on_actionAddDevice_triggered()
{
    QList<QVariant> workPlace;
    workPlace<<m_wpId;workPlace<<wpName;workPlace<<m_wpFirmId;
    MoveDevice *md = new MoveDevice(this,false,workPlace,QList<QVariant>(),true);
    connect(md,SIGNAL(devIsMoved()),this,SLOT(afterMove()));
    md->setAttribute(Qt::WA_DeleteOnClose);
    md->exec();
}

void Device::on_actionDelDevice_triggered()
{
    int button;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(devModel->model()->index(curViewIndex.row(),
                                devModel->model()->cIndex.type,
                                curViewIndex.parent())
            .data().toInt() == 1)
        button = QMessageBox::question(this,tr("Удаление устройства"),
                                       tr("Внимание!!!\n"
                                          "При удалении устройства будет удален\n"
                                          "весь состав выбранного устройства!!!\n"
                                          "Вы уверены что хотите удалить данное устройство?"),
                                       tr("Да"),tr("Нет"),"",1,1);
    else
        button = QMessageBox::question(this,tr("Удаление устройства"),
                                       tr("Вы уверены что хотите удалить данное устройство?"),
                                       tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    devModel->model()->sqlRemoveRow(curViewIndex.row(),curViewIndex.parent());
    if(devModel->model()->rowCount() <= 0){
        if(!m_wpMode){
            organization->setText("");
            filial->setText("");
            clearWpForm();
        }
        setAccessToActions();
    }
}

void Device::on_actionMoveDevice_triggered()
{
    QList<QVariant> device;
    QList<QVariant> workPlace;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    for(int i = 0;i<devModel->model()->columnCount();i++)
        device<<devModel->model()->data(
                    devModel->model()->index(curViewIndex.row(),
                                             i,
                                             curViewIndex.parent()
                                             )
                    );
    if(!m_wpMode){
        workPlace<<wpModel->index(0,0).data();workPlace<<wpModel->index(0,3).data();
    }else{
        workPlace<<m_wpId;workPlace<<wpName;
    }
    workPlace<<devModel->model()->data(
                   devModel->model()->index(curViewIndex.row(),
                                            devModel->model()->cIndex.codOrganization,
                                            curViewIndex.parent()
                                            )
                   ).toInt();
    MoveDevice *md = new MoveDevice(this,true,workPlace,device);
    connect(md,SIGNAL(devIsMoved(int,int)),this,SLOT(afterMove(int,int)));
    md->setAttribute(Qt::WA_DeleteOnClose);
    md->exec();
}

void Device::on_actionAddNewDiviceInComposition_triggered()
{
    AddEditDevice *aed;
    int parentId;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(devModel->model()->index(curViewIndex.row(),
                                devModel->model()->cIndex.type,
                                curViewIndex.parent())
            .data().toInt() == 1)
        parentId = devModel->model()->index(curViewIndex.row(),
                                            devModel->model()->cIndex.id,
                                            curViewIndex.parent())
                .data().toInt();
    else
        parentId = devModel->model()->index(curViewIndex.row(),
                                            devModel->model()->cIndex.parent_id,
                                            curViewIndex.parent())
                .data().toInt();
    if(!m_wpMode)
        aed = new AddEditDevice(this,0,parentId,wpModel->data(wpModel->index(0,0)).toInt(),
                                wpModel->data(wpModel->index(0,3)).toString(),true);
    else
        aed = new AddEditDevice(this,0,parentId,m_wpId,wpName,true);
    connect(aed,SIGNAL(deviceAdded(int)),this,SLOT(insertNewRowOfDB(int)));
    aed->exec();
}

void Device::on_actionEditDevice_triggered()
{
    int wpId;
    QList<QVariant> r;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    bool readOnly = false;

    if(!m_readOnly){
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->recordIsLosked(devModel->model()->data(devModel->model()->index(curViewIndex.row(),
                                                                                           devModel->model()->cIndex.id,
                                                                                           curViewIndex.parent())).toInt(),
                                          devModel->model()->colTabName.id,
                                          devModel->model()->nameModelTable())){
            if(lockedControl->lastError().type() != QSqlError::NoError)
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
        }else{
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Запись заблокированна пользователем: %1")
                                     .arg(lockedControl->recordBlockingUser()),
                                     tr("Закрыть"));
            readOnly = true;
        }
    }else
        readOnly = m_readOnly;

    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT %4.%1, %4.%2, typedevice.Type, %4.%3 FROM %5 %4 "
                            "LEFT OUTER JOIN typedevice "
                            "ON %4.CodTypeDevice = typedevice.CodTypeDevice "
                            "WHERE %4.%6 = %7")
                    .arg(devModel->model()->colTabName.rv)
                    .arg(devModel->model()->colTabName.parent_id)
                    .arg(devModel->model()->colTabName.codWorkerPlace)
                    .arg(devModel->model()->aliasModelTable())
                    .arg(devModel->model()->nameModelTable())
                    .arg(devModel->model()->colTabName.id)
                    .arg(devModel->model()->data(devModel->model()->index(curViewIndex.row(),
                                                                          devModel->model()->cIndex.id,
                                                                          curViewIndex.parent())).toInt()));
    if(ok){
        query.next();
        if(devModel->model()->index(curViewIndex.row(), devModel->model()->cIndex.rv,
                                    curViewIndex.parent()).data().toInt() != query.value(0).toInt()){
            if(query.value(3).toInt() != devModel->model()->index(curViewIndex.row(), devModel->model()->cIndex.codWorkerPlace,
                                                                  curViewIndex.parent()).data().toInt()){
                QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                         tr("Устройство было перемещено,\n"
                                            "откройте устройство с нового места положения"),
                                         tr("Закрыть"));
                afterMove(query.value(2).toInt(),query.value(1).toInt());
                return;
            }else{
                if(query.value(1).toInt() != devModel->model()->index(curViewIndex.row(), devModel->model()->cIndex.parent_id,
                                                                      curViewIndex.parent()).data().toInt()){
                    QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                             tr("Устройство было перемещено,\n"
                                                "откройте устройство с нового места положения"),
                                             tr("Закрыть"));
                    afterMove(query.value(2).toInt(),query.value(1).toInt());
                    return;
                }else{
                    QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                             tr("Запись была изменена, будут загружены новые данные записи"),
                                             tr("Закрыть"));
                    devModel->model()->updateRow(curViewIndex.row(),curViewIndex.parent());
                }
            }
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версии записи:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    }
    if(m_wpMode)
        wpId = m_wpId;
    else
        wpId = wpModel->index(0,0).data().toInt();
    for(int i=0; i<devModel->model()->columnCount(curViewIndex);i++)
        r.append(devModel->model()->data(devModel->model()->index(curViewIndex.row(),i,curViewIndex.parent())));
    AddEditDevice *aed = new AddEditDevice(this,r.value(devModel->model()->cIndex.type).toInt(),0,
                                           wpId,"",true,true,r,false,0,readOnly);
    aed->setAttribute(Qt::WA_DeleteOnClose);
    connect(aed,SIGNAL(deviceDataChanged()),this,SLOT(updateDevRow()));
    aed->exec();
}

void Device::on_actionHistoryMoved_triggered()
{
    QDialog *d = new QDialog(this);
    int nWidth = 400;
    int nHeight = 200;
    d->setWindowTitle(tr("История перемещений"));
    QVBoxLayout *layout = new QVBoxLayout;
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    JournalHistoryMoved *jhm = new JournalHistoryMoved(d,devModel->model()->data(
                                                           devModel->model()->index(curViewIndex.row(),
                                                                                    devModel->model()->cIndex.id,
                                                                                    curViewIndex.parent()
                                                                                    )
                                                           ).toInt());
    jhm->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhm,SIGNAL(closeJhmWin()),d,SLOT(reject()));
    layout->addWidget(jhm);
    d->setLayout(layout);
    d->layout()->setContentsMargins(0,0,0,0);
    jhm->show();
    QPoint parentPos = this->mapToGlobal(this->pos());
    d->setGeometry(parentPos.x() + this->width()/2 - nWidth/2,
                   parentPos.y() + this->height()/2 - nHeight/2,
                   nWidth, nHeight);
    d->exec();
}

void Device::on_buttonMove_clicked()
{
    MoveDevice *md;
    if(!m_wpMode)
        md = new MoveDevice(this);
    else{
        QList<QVariant> workPlace;
        workPlace<<m_wpId;workPlace<<wpName;workPlace<<m_wpFirmId;
        md = new MoveDevice(this,false,workPlace,QList<QVariant>(),false,true);
    }
    connect(md,SIGNAL(devIsMoved()),this,SLOT(afterMove()));
    md->setAttribute(Qt::WA_DeleteOnClose);
    md->exec();
}

void Device::on_buttonDeviceFilter_clicked()
{
    FilterDevice *f;
    if(m_wpMode)
        f = new FilterDevice(this,true);
    else
        f = new FilterDevice(this);
    connect(f,SIGNAL(setFilter(QString)),this,SLOT(setDeviceFilter(QString)));
    f->exec();
}

void Device::on_buttonClearDeviceFilter_clicked()
{
    setDeviceFilter("");
}

QString Device::readDataModel(const QModelIndex &index){
    QString str = "";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.orgName,index.parent()).data().toString() + "\t";
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("select p.name from departments n, tree t, departments p where n.id =  "
                            "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                            "and n.id = t.id and t.parent_id = p.id and p.FP = 1;")
                    .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    ok = query.exec(QString("SELECT Name FROM departments WHERE id = (SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace = %1)")
                    .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.wpName,index.parent()).data().toString() + "\t";
    ok = query.exec(QString("SELECT FIOSummary FROM users WHERE CodUser = (SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1)")
                    .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.typeDevName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.name,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.networkName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.domainWgName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.inventoryN,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.serialN,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.producerName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.providerName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.datePurchase,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.datePosting,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.endGuarantee,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.price,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.stateName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.note,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.detailDescription,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.id,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.parent_id,index.parent()).data().toString() + "\n";
    return str;
}

void Device::on_actionCopyDeviceInBufer_triggered()
{
    QString str = "";
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    QModelIndex index = curViewIndex;
    // шапка
    str += tr("Фирма") + "\t";
    str += tr("Филиал/Представительство") + "\t";
    str += tr("Подразделение") + "\t";
    str += tr("Рабочее место/склад") + "\t";
    str += tr("Пользователь") + "\t";
    str += tr("Тип устройства") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Сетевое имя") + "\t";
    str += tr("Домен/Рабочая группа") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("Серийный №") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Поставщик") + "\t";
    str += tr("Дата покупки") + "\t";
    str += tr("Дата оприходования") + "\t";
    str += tr("Конец гарантии") + "\t";
    str += tr("Цена") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Примечание") + "\t";
    str += tr("Детальное описание") + "\n";
    // данные
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.orgName,index.parent()).data().toString() + "\t";
    if(!m_wpMode){
        str += filial->text() + "\t";
        str += WPDivision->text() + "\t";
    }else{
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("select p.name from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id and p.FP = 1;")
                        .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
        ok = query.exec(QString("SELECT Name FROM departments WHERE id = (SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace = %1)")
                        .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
    }
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.wpName,index.parent()).data().toString() + "\t";
    if(!m_wpMode)
        str += WPUser->text() + "\t";
    else{
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("SELECT FIOSummary FROM users WHERE CodUser = (SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1)")
                        .arg(devModel->model()->index(index.row(),devModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
    }
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.typeDevName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.name,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.networkName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.domainWgName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.inventoryN,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.serialN,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.producerName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.providerName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.datePurchase,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.datePosting,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.endGuarantee,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.price,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.stateName,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.note,index.parent()).data().toString() + "\t";
    str += devModel->model()->index(index.row(),devModel->model()->cIndex.detailDescription,index.parent()).data().toString();
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, tr("Копирование"),tr("Данные скопированны в буфер обмена."),tr("Закрыть"));
}

void Device::on_buttonCopyAllInBufer_clicked()
{
    QString str = "";
    QModelIndex parent, children;
    // шапка
    str += tr("Фирма") + "\t";
    str += tr("Филиал/Представительство") + "\t";
    str += tr("Подразделение") + "\t";
    str += tr("Рабочее место/склад") + "\t";
    str += tr("Пользователь") + "\t";
    str += tr("Тип устройства") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Сетевое имя") + "\t";
    str += tr("Домен/Рабочая группа") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("Серийный №") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Поставщик") + "\t";
    str += tr("Дата покупки") + "\t";
    str += tr("Дата оприходования") + "\t";
    str += tr("Конец гарантии") + "\t";
    str += tr("Цена") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Примечание") + "\t";
    str += tr("Детальное описание") + "\t";
    str += tr("Код") + "\t";
    str += tr("Код родителя") + "\n";
    // данные
    for(int i = 0; i < devModel->model()->rowCount(); i++){
        parent = devModel->model()->index(i,0);
        str += readDataModel(parent);
        if(devModel->model()->rowCount(parent) > 0)
            for(int j = 0; j < devModel->model()->rowCount(parent); j++){
                children = devModel->model()->index(j,0,parent);
                str += readDataModel(children);
            }
    }
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, tr("Копирование"),tr("Данные скопированны в буфер обмена."),tr("Закрыть"));
}

void Device::on_buttonClose_clicked()
{
    emit closeDeviceWin();
}

void Device::showLoadIndicator()
{
    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;
    QLabel *load = new QLabel(deviceView);
    QMovie *movie = new QMovie(":/animations/ico/animation/loading.gif");
    actionAddDevice->setEnabled(false);
    actionAddNewDevice->setEnabled(false);
    buttonUpdate->setEnabled(false);
    buttonCopyAllInBufer->setEnabled(false);
    actionAddNewDiviceInComposition->setEnabled(false);
    actionDelDevice->setEnabled(false);
    actionMoveDevice->setEnabled(false);
    actionEditDevice->setEnabled(false);
    actionHistoryMoved->setEnabled(false);
    actionCopyDeviceInBufer->setEnabled(false);
    buttonMove->setEnabled(false);
    buttonDeviceFilter->setEnabled(false);
    buttonClearDeviceFilter->setEnabled(false);
    load->setMovie(movie);
    load->setAttribute(Qt::WA_DeleteOnClose);
    connect(this,SIGNAL(loadIndicatorClosed()),load,SLOT(close()));
    movie->start();
    screenWidth = deviceView->width();
    screenHeight = deviceView->height();
    windowSize = load->size();
    width = windowSize.width();
    height = windowSize.height();
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 20;
    load->move(x,y);
    load->show();
}
