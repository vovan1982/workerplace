#include <QtWidgets>
#include "headers/workplace.h"
#include "headers/sqltreemodel.h"
#include "headers/movetreeitem.h"
#include "headers/addeditdepartments.h"
#include "headers/addeditworkerplace.h"
#include "headers/addeditfirm.h"
#include "headers/addeditfilpred.h"
#include "headers/addworkerplace.h"
#include "headers/workplacemodel.h"
#include "headers/lockdatabase.h"

WorkPlace::WorkPlace(QWidget *parent) :
    QWidget(parent){
    setupUi(this);
    tablName = "departments";
    QActionGroup *ag = new QActionGroup(this);
    QActionGroup *ag2 = new QActionGroup(this);
    ag->setExclusive(true);
    ag->addAction(actionAddWP);
    ag->addAction(actionDelWP);
    ag->addAction(actionEditWP);
    ag->addAction(actionMoveWP);
    ag->addAction(actionMoveWPAll);
    ag2->addAction(actionAddNewFirm);
    ag2->addAction(actionAddFilPred);
    ag2->addAction(actionAddDepart);
    ag2->addAction(actionDelDepart);
    ag2->addAction(actionEditDepart);
    ag2->addAction(actionMoveDepart);
    menu = new QMenu(tr("Подразделения"), this);
    menu2 = new QMenu(tr("Рабочие места/Склады"), this);
    menu->addActions(ag2->actions());
    menu2->addActions(ag->actions());
    actionsButton->setMenu(menu);
    actionsButton_2->setMenu(menu2);

    setMoveMode(0,0);
    setMoveWpMode(0,0);
    delMode = false;
    cancelMove = false;
    modelDepartment = new SqlTreeModel(treeViewDepartments);
    modelDepartment->setTable(tablName);
    modelDepartment->setHeaderData(0,Qt::Horizontal,tr("Подразделения"));
    modelDepartment->setRootName("<Все>");
    modelDepartment->select();

    treeViewDepartments->setModel(modelDepartment);
    treeViewDepartments->expandAll();
    treeViewDepartments->setColumnHidden(1,true);
    treeViewDepartments->setColumnHidden(2,true);
    treeViewDepartments->setColumnHidden(3,true);
    treeViewDepartments->setColumnHidden(4,true);
    treeViewDepartments->setColumnHidden(5,true);
    treeViewDepartments->setCurrentIndex(modelDepartment->index(0,0));

    wpModel = new WorkPlaceModel(treeViewWorkPlace);

    treeViewWorkPlace->setModel(wpModel);
    wpModel->sort(3,Qt::AscendingOrder);
    treeViewWorkPlace->setColumnHidden(0,true);
    treeViewWorkPlace->setColumnHidden(1,true);
    treeViewWorkPlace->setColumnHidden(2,true);
    treeViewWorkPlace->setColumnHidden(4,true);
    treeViewWorkPlace->setColumnHidden(8,true);
    treeViewWorkPlace->setColumnHidden(9,true);
    treeViewWorkPlace->setColumnHidden(10,true);
    treeViewWorkPlace->setColumnHidden(11,true);
    treeViewWorkPlace->resizeColumnToContents(3);
    treeViewWorkPlace->resizeColumnToContents(5);
    treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));

    connect(treeViewWorkPlace, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onWPMenu(const QPoint &)));
    connect(treeViewWorkPlace, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_actionEditWP_triggered()));
    connect(treeViewDepartments,SIGNAL(collapsed(QModelIndex)),treeViewDepartments,SLOT(setCurrentIndex(QModelIndex)));
    connect(treeViewDepartments, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onDepMenu(const QPoint &)));
    connect(treeViewDepartments, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(doubleClickedDepView(QModelIndex)));
    connect(treeViewDepartments->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updateWPModel(QModelIndex)));
    connect( (QObject*) treeViewWorkPlace->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortWPClick()));
}
void WorkPlace::onWPMenu(const QPoint &p){
   menu2->exec(treeViewWorkPlace->viewport()->mapToGlobal(p));
}
void WorkPlace::onDepMenu(const QPoint &p){
   menu->exec(treeViewDepartments->viewport()->mapToGlobal(p));
}
void WorkPlace::doubleClickedDepView(const QModelIndex &index){
    if(modelDepartment->rowCount(index) <= 0)
        on_actionEditDepart_triggered();
}
void WorkPlace::populateWPModel(const QString &filter)
{
    wpModel->setFilter(filter);
    wpModel->select();
}
void WorkPlace::updateWPModel(const QModelIndex &idx)
{
    QString filter;
    if(modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toInt() == 0){
        actionAddFilPred->setEnabled(false);
        actionAddDepart->setEnabled(false);
        actionDelDepart->setEnabled(false);
        actionEditDepart->setEnabled(false);
        actionMoveDepart->setEnabled(false);
    }else{
        actionAddFilPred->setEnabled(true);
        actionAddDepart->setEnabled(true);
        actionDelDepart->setEnabled(true);
        actionEditDepart->setEnabled(true);
        if(modelDepartment->data(modelDepartment->index(idx.row(),3,idx.parent())).toInt() == 1 ||
           modelDepartment->data(modelDepartment->index(idx.row(),4,idx.parent())).toInt() == 1)
            actionMoveDepart->setEnabled(false);
        else
            actionMoveDepart->setEnabled(true);
    }
    if(idx != QModelIndex() && modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toInt() != 0){
        if(wpSubsidiaryDep->isChecked())
            filter = "CodDepartment = " + modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toString();
        else
            filter = QString("coddepartment IN ( SELECT c.id FROM "
                             "(SELECT * FROM departments) n, "
                             "(SELECT * FROM tree) t, "
                             "(SELECT * FROM departments) c "
                             "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id)")
                    .arg(modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toInt());
        populateWPModel(filter);
        treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
    }else{
        populateWPModel();
        treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
    }
    if(wpModel->rowCount() <= 0){
        if(modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toInt() != 0){
            if(wpSubsidiaryDep->isChecked())
                actionAddWP->setEnabled(true);
            else
                actionAddWP->setEnabled(false);
        }else
            actionAddWP->setEnabled(false);
        actionDelWP->setEnabled(false);
        actionEditWP->setEnabled(false);
        actionMoveWP->setEnabled(false);
        actionMoveWPAll->setEnabled(false);
    }else{
        actionDelWP->setEnabled(true);
        actionEditWP->setEnabled(true);
        if(modelDepartment->data(modelDepartment->index(idx.row(),1,idx.parent())).toInt() != 0){
            if(wpSubsidiaryDep->isChecked()){
                actionAddWP->setEnabled(true);
                actionMoveWPAll->setEnabled(true);
            }else{
                actionAddWP->setEnabled(false);
                actionMoveWPAll->setEnabled(false);
            }
            actionMoveWP->setEnabled(true);
        }else{
            actionAddWP->setEnabled(false);
            actionMoveWP->setEnabled(false);
            actionMoveWPAll->setEnabled(false);
        }
    }
}
void WorkPlace::on_wpSubsidiaryDep_clicked()
{
    updateWPModel(treeViewDepartments->currentIndex());
}
void WorkPlace::setMoveMode(int newP, int moveM)
{
    moveMode = moveM;
    newParentId = newP;
}
void WorkPlace::setMoveWpMode(int newD, int moveWpM)
{
    moveWpMode = moveWpM;
    newDepWpId = newD;
}
void WorkPlace::on_actionAddNewFirm_triggered()
{
    AddEditFirm *af = new AddEditFirm(this, tablName);
    connect(af,SIGNAL(newFirmAdd(QString,int)),this,SLOT(addNewFirm(QString,int)));
    af->exec();
}
void WorkPlace::addNewFirm(const QString &name, int id)
{
    QModelIndex index = modelDepartment->index(0,0,QModelIndex());
    modelDepartment->insertRow(modelDepartment->rowCount(index),index);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index),name);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,1,index),id);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,2,index),0);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,3,index),1);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,4,index),0);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,5,index),0);
    treeViewDepartments->setCurrentIndex(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index));
    treeViewDepartments->setFocus();
}
void WorkPlace::on_actionAddFilPred_triggered()
{
    if (modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),3,treeViewDepartments->currentIndex().parent())).toInt() == 1){
        AddEditFilPred *afp = new AddEditFilPred(this, tablName, modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt());
        connect(afp,SIGNAL(newFilPredAdd(QString,int)),this,SLOT(addNewFilPred(QString,int)));
        afp->exec();
    }else{
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),tr("Для добавления Филиала/Представительства необходимо выбрать Фирму.\n"
                                                           "Подразделение выбранное вами не является фирмой."),tr("Закрыть"));
    }
}
void WorkPlace::addNewFilPred(const QString &name, int id)
{
    QModelIndex index = treeViewDepartments->currentIndex();
    modelDepartment->insertRow(modelDepartment->rowCount(index),index);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index),name);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,1,index),id);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,2,index),
                                       modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt());
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,3,index),0);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,4,index),1);
    modelDepartment->setDataWithOutSQL(modelDepartment->index(modelDepartment->rowCount(index)-1,5,index),0);
    treeViewDepartments->setCurrentIndex(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index));
    treeViewDepartments->setFocus();
}
void WorkPlace::on_actionAddDepart_triggered()
{
    if(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt() != 0){
        AddEditDepartments *ad = new AddEditDepartments(this);
        connect(ad,SIGNAL(newDepartmentAdd(QString,bool)),this,SLOT(addDepartment(QString,bool)));
        ad->exec();
        treeViewDepartments->setFocus();
    }else
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),tr("Подразделения нельзя создавать в основном разделе\n"
                                                           "Выберите фирму, филиал/представительство или другое подразделение."),tr("Закрыть"));
}
void WorkPlace::addDepartment(const QString &name, bool multiMode)
{
    QModelIndex index = treeViewDepartments->currentIndex();
    modelDepartment->sqlInsertRow(modelDepartment->rowCount(index),index);
    modelDepartment->setData(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index),name);
    if(!multiMode)
        treeViewDepartments->setCurrentIndex(modelDepartment->index(modelDepartment->rowCount(index)-1,0,index));
    treeViewDepartments->setFocus();
}
void WorkPlace::on_actionDelDepart_triggered()
{
    int button;
    bool wpl = false;
    bool ddep = false;
    QSqlQuery query;
    QModelIndex index = treeViewDepartments->currentIndex();
    if(isFirm(index)){
        button = QMessageBox::critical(this,tr("ВНИМАНИЕ!!!"),tr("При удалении Фирмы будут уделены все филиалы/представительства,\n"
                                                                 "подразделения, рабочие места, устройства, лицензии,\n"
                                                                 "сетевые настройки связанные с этой фирмой.\n"
                                                                 "Вы действительно хотите удалить указанную фирму?")
                                       ,tr("Удалить"),tr("Отмена"),"",1);
        if(button == 1)
            return;
        else{
            modelDepartment->sqlRemoveRow(index.row(),index.parent());
            treeViewDepartments->setFocus();
            return;
        }
    }
    if(isFilPred(index)){
        button = QMessageBox::critical(this,tr("ВНИМАНИЕ!!!"),tr("При удалении филиала/представительства будут уделены все ,\n"
                                                                 "подразделения, рабочие места, устройства, лицензии,\n"
                                                                 "сетевые настройки связанные с этим филиалом/представительством.\n"
                                                                 "Вы действительно хотите удалить указанный филиал/представительство?")
                                       ,tr("Удалить"),tr("Отмена"),"",1);
        if(button == 1)
            return;
        else{
            modelDepartment->sqlRemoveRow(index.row(),index.parent());
            treeViewDepartments->setFocus();
            return;
        }
    }
    if(modelDepartment->rowCount(index) > 0)
        ddep = true;
    query.exec(QString("SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ( "
                       "SELECT c.id FROM (SELECT * FROM departments) n, "
                       "(SELECT * FROM tree) t, "
                       "(SELECT * FROM departments) c "
                       "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id)")
               .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
    if(query.size() > 0){
        wpl = true;
        query.clear();
    }
    if(ddep && wpl){
        query.exec(QString("SELECT id FROM device WHERE CodWorkerPlace IN ("
                           "SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ("
                           "SELECT c.id FROM "
                           "(SELECT * FROM departments) n, (SELECT * FROM tree) t, (SELECT * FROM departments) c WHERE "
                           "n.id = %1 AND n.id = t.parent_id AND t.id = c.id))")
                   .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
        if(query.size() > 0){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Невозможно удалить данное подразделение\n"
                                        "потому что, на рабочих местах присутствуют устройства.\n"
                                        "Переместите все устройства со всех рабочих мест,\n"
                                        "после чего повторите попытку."),
                                     tr("Закрыть"));
            return;
        }
        query.exec(QString("SELECT `key` FROM licenses WHERE CodWorkerPlace IN ("
                           "SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ("
                           "SELECT c.id FROM "
                           "(SELECT * FROM departments) n, (SELECT * FROM tree) t, (SELECT * FROM departments) c WHERE "
                           "n.id = %1 AND n.id = t.parent_id AND t.id = c.id))")
                   .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
        if(query.size() > 0){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Невозможно удалить данное подразделение\n"
                                        "потому что, на рабочих местах присутствуют лицензии.\n"
                                        "Переместите все лицензии со всех рабочих мест,\n"
                                        "после чего повторите попытку."),
                                     tr("Закрыть"));
            return;
        }
        button = QMessageBox::question(this,tr("ВНИМАНИЕ!!!"),tr("Удаляемое подразделение содержит дочернии подразделения и рабочие места.\n"
                                                                 "При удалении данного подразделения, дочернии подразделения и рабочие места будут удалены.\n"
                                                                 "Хотите переместить их в другое подразделение?")
                                       ,tr("Удалить"),tr("Переместить"),tr("Отмена"),2);
        switch(button){
        case 0: modelDepartment->sqlRemoveRow(index.row(),index.parent());
            treeViewDepartments->setFocus();
            break;
        case 1: delMode = true;
            moveWpDep(true,true);
            if(!cancelMove){
                modelDepartment->sqlRemoveRow(index.row(),index.parent());
                treeViewDepartments->setFocus();
                updateWPModel(treeViewDepartments->currentIndex());
            }
            cancelMove = false;
            delMode = false;
            break;
        }
        return;
    }
    if(ddep && !wpl){
        button = QMessageBox::question(this,tr("ВНИМАНИЕ!!!"),tr("Удаляемое подразделение содержит дочернии подразделения.\n"
                                                                 "При удалении данного подразделения, дочернии подразделения будут удалены.\n"
                                                                 "Хотите переместить их в другое подразделение?")
                                       ,tr("Удалить"),tr("Переместить"),tr("Отмена"),2);
        switch(button){
        case 0: modelDepartment->sqlRemoveRow(index.row(),index.parent());
            treeViewDepartments->setFocus();
            break;
        case 1: delMode = true;
            moveWpDep(true,false);
            if(!cancelMove){
                modelDepartment->sqlRemoveRow(index.row(),index.parent());
                treeViewDepartments->setFocus();
                updateWPModel(treeViewDepartments->currentIndex());
            }
            cancelMove = false;
            delMode = false;
            break;
        }
        return;
    }
    if(!ddep && wpl){
        QSqlQuery query;
        query.exec(QString("SELECT id FROM device WHERE CodWorkerPlace IN ("
                           "SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ("
                           "SELECT c.id FROM "
                           "(SELECT * FROM departments) n, (SELECT * FROM tree) t, (SELECT * FROM departments) c WHERE "
                           "n.id = %1 AND n.id = t.parent_id AND t.id = c.id))")
                   .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
        if(query.size() > 0){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Невозможно удалить данное подразделение\n"
                                        "потому что, на рабочих местах присутствуют устройства.\n"
                                        "Переместите все устройства со всех рабочих мест,\n"
                                        "после чего повторите попытку."),
                                     tr("Закрыть"));
            return;
        }
        query.exec(QString("SELECT `key` FROM licenses WHERE CodWorkerPlace IN ("
                           "SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ("
                           "SELECT c.id FROM "
                           "(SELECT * FROM departments) n, (SELECT * FROM tree) t, (SELECT * FROM departments) c WHERE "
                           "n.id = %1 AND n.id = t.parent_id AND t.id = c.id))")
                   .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
        if(query.size() > 0){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Невозможно удалить данное подразделение\n"
                                        "потому что, на рабочих местах присутствуют лицензии.\n"
                                        "Переместите все лицензии со всех рабочих мест,\n"
                                        "после чего повторите попытку."),
                                     tr("Закрыть"));
            return;
        }
        button = QMessageBox::question(this,tr("ВНИМАНИЕ!!!"),tr("Удаляемое подразделение содержит рабочие места.\n"
                                                                 "При удалении данного подразделения, рабочие места будут удалены.\n"
                                                                 "Хотите переместить их в другое подразделение?")
                                       ,tr("Удалить"),tr("Переместить"),tr("Отмена"),2);
        switch(button){
        case 0: modelDepartment->sqlRemoveRow(index.row(),index.parent());
            treeViewDepartments->setFocus();
            break;
        case 1: delMode = true;
            moveWpDep(false,true);
            if(!cancelMove){
                modelDepartment->sqlRemoveRow(index.row(),index.parent());
                treeViewDepartments->setFocus();
                updateWPModel(treeViewDepartments->currentIndex());
            }
            cancelMove = false;
            delMode = false;
            break;
        }
        return;
    }
    if(!ddep && !wpl){
        button = QMessageBox::question(this,tr("Удаление подразделения"),tr("Вы действительно хотите удалить данное подразделение?.")
                                       ,tr("Удалить"),tr("Отмена"),"",1);
        if(button == 0)
            modelDepartment->sqlRemoveRow(index.row(),index.parent());
        treeViewDepartments->setFocus();
    }
}
void WorkPlace::on_actionEditDepart_triggered()
{
    QModelIndex index = treeViewDepartments->currentIndex();
    if(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt() == 0)
        return;
    bool readOnly = false;
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->recordIsLosked(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt(),
                                      "id",
                                      modelDepartment->tableName())){
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
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT RV FROM %1 WHERE id = %2")
                    .arg(tablName)
                    .arg(modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt()));
    if(ok){
        query.next();
        if(modelDepartment->data(modelDepartment->index(index.row(),5,index.parent())).toInt() != query.value(0).toInt()){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Запись была изменена, будут загружены новые данные записи"),
                                 tr("Закрыть"));
            modelDepartment->updateRow(index.row());
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версии записи:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    }
    if(modelDepartment->data(modelDepartment->index(index.row(),3,index.parent())).toInt() == 1){
        AddEditFirm *ef = new AddEditFirm(this,
                                          tablName,
                                          true,
                                          modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt(),
                                          readOnly);
        if(readOnly)
            ef->setWindowTitle(tr("Редактирование фирмы - [Только чтение]"));
        else
            ef->setWindowTitle(tr("Редактирование фирмы"));
        connect(ef,SIGNAL(editFirm(QString)),this,SLOT(editDepartment(QString)));
        ef->exec();
        return;
    }
    if(modelDepartment->data(modelDepartment->index(index.row(),4,index.parent())).toInt() == 1){
        AddEditFilPred *efp = new AddEditFilPred(this,
                                                 tablName,
                                                 modelDepartment->data(modelDepartment->index(index.row(),2,index.parent())).toInt(),
                                                 true,
                                                 modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt(),
                                                 readOnly);
        if(readOnly)
            efp->setWindowTitle(tr("Редактирование филиала/представительства - [Только чтение]"));
        else
            efp->setWindowTitle(tr("Редактирование филиала/представительства"));
        connect(efp,SIGNAL(editFilPred(QString)),this,SLOT(editDepartment(QString)));
        efp->exec();
        return;
    }
    AddEditDepartments *aed = new AddEditDepartments(this,
                                             true,
                                             modelDepartment->data(index).toString(),
                                             tablName,
                                             modelDepartment->data(modelDepartment->index(index.row(),1,index.parent())).toInt(),
                                             readOnly);
    if(readOnly)
        aed->setWindowTitle(tr("Редактирование подразделения - [Только чтение]"));
    else
        aed->setWindowTitle(tr("Редактирование подразделения"));
    connect(aed,SIGNAL(newDepartmentAdd(QString,bool)),this,SLOT(editDepartment(QString)));
    aed->exec();
}
void WorkPlace::editDepartment(const QString &name)
{
    QModelIndex index = treeViewDepartments->currentIndex();
    modelDepartment->setData(modelDepartment->index(index.row(),0,index.parent()),name);
    treeViewDepartments->setFocus();
}
void WorkPlace::on_actionMoveDepart_triggered()
{
    moveWpDep(true,false);
    cancelMove = false;
}
void WorkPlace::on_actionMoveWP_triggered()
{
    moveWpDep(false,true);
    cancelMove = false;
}
void WorkPlace::on_actionMoveWPAll_triggered()
{
    delMode = true;
    moveWpDep(false,true);
    cancelMove = false;
    delMode = false;
}
void WorkPlace::moveWpDep(bool depMode, bool wpMode)
{
    if(depMode && !wpMode)
        if(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt() == 0)
            return;
    if(!depMode && wpMode)
        if(wpModel->rowCount() == 0)
            return;

    QSqlQuery query;
    bool ok;
    bool isFilP = false;
    int firmFilId = 0;
    int parentDep = 0;
    QString nameItem;
    QModelIndex curIndex = treeViewDepartments->currentIndex();

    if(depMode){
        nameItem = modelDepartment->data(curIndex).toString();
        parentDep = modelDepartment->data(modelDepartment->index(curIndex.row(),1,curIndex.parent())).toInt();
    }
    if(wpMode && delMode){
        nameItem = tr("Все рабочие места");
        parentDep = modelDepartment->data(modelDepartment->index(curIndex.row(),1,curIndex.parent())).toInt();
    }else if(wpMode && !delMode){
        nameItem = wpModel->data(wpModel->index(treeViewWorkPlace->currentIndex().row(),3)).toString();
        parentDep = wpModel->data(wpModel->index(treeViewWorkPlace->currentIndex().row(),1)).toInt();
    }

    ok = query.exec(QString("SELECT p.id FROM "
               "(SELECT * FROM departments) n, "
               "(SELECT * FROM tree) t, "
               "(SELECT * FROM departments) p "
               "WHERE "
               "n.id = %1 AND n.id = t.id AND t.parent_id = p.id AND p.FP = 1;")
               .arg(parentDep));
    if(ok){
        if(query.size()>0){
            isFilP = true;
            query.next();
            firmFilId = query.value(0).toInt();
        }else{
            ok = query.exec(QString("SELECT p.id FROM "
                                    "(SELECT * FROM departments) n, "
                                    "(SELECT * FROM tree) t, "
                                    "(SELECT * FROM departments) p "
                                    "WHERE "
                                    "n.id = %1 AND n.id = t.id AND t.parent_id = p.id AND p.Firm = 1;")
                            .arg(parentDep));
            if(ok){
                if(query.size()>0){
                    isFilP = false;
                    query.next();
                    firmFilId = query.value(0).toInt();
                }
            }else
                qDebug()<<query.lastError().text();
        }
    }else
        qDebug()<<query.lastError().text();
    MoveTreeItem *mti = new MoveTreeItem(this,nameItem,
                                         parentDep,
                                         modelDepartment->data(modelDepartment->index(curIndex.row(),2,curIndex.parent())).toInt(),
                                         "departments",
                                         modelDepartment->rowCount(curIndex),
                                         delMode,
                                         isFilP,
                                         firmFilId,
                                         depMode,
                                         wpMode);
    connect(mti,SIGNAL(newParentItem(int,int)),this,SLOT(setMoveMode(int,int)));
    connect(mti,SIGNAL(newParentItemWp(int,int)),this,SLOT(setMoveWpMode(int,int)));
    if(mti->exec()){
        if (depMode){
            QModelIndex index;
            if (moveMode == 0){
                index = modelDepartment->moveItem(parentDep,
                                                  newParentId);
                treeViewDepartments->setCurrentIndex(index);
                treeViewDepartments->setFocus();
                treeViewDepartments->expand(index.parent());
            }else if (moveMode == 1){
                QList<int> movedIds;
                for(int i = 0;i<modelDepartment->rowCount(curIndex);i++)
                    movedIds<<modelDepartment->data(modelDepartment->index(i,1,curIndex)).toInt();
                foreach(int id, movedIds){
                    index = modelDepartment->moveItem(id, newParentId);
                }
                treeViewDepartments->setCurrentIndex(index.parent());
                treeViewDepartments->setFocus();
                treeViewDepartments->expand(index.parent());
            }
        }
        if(wpMode){
            if (moveWpMode == 0){
                ok = query.exec(QString("UPDATE workerplace SET CodDepartment = %1 WHERE CodWorkerPlace = %2")
                                .arg(newDepWpId)
                                .arg(wpModel->data(wpModel->index(treeViewWorkPlace->currentIndex().row(),0)).toInt()));
                if(ok){
                    updateWPModel(treeViewDepartments->currentIndex());
                    treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
                }else
                    qDebug()<<query.lastError().text();
            }else if (moveWpMode == 1){
                ok = query.exec(QString("UPDATE workerplace SET CodDepartment = %1 WHERE CodWorkerPlace IN "
                                        "(SELECT w.codworkerplace FROM (SELECT * FROM workerplace) w WHERE coddepartment = %2)")
                                .arg(newDepWpId)
                                .arg(modelDepartment->data(modelDepartment->index(
                                                               curIndex.row(),
                                                               1,
                                                               curIndex.parent())).toInt()));

                if(ok){
                    updateWPModel(treeViewDepartments->currentIndex());
                    treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
                }else
                    qDebug()<<query.lastError().text();
            }else if (moveWpMode == 2){
                ok = query.exec(QString("UPDATE workerplace SET CodDepartment = %1 WHERE CodWorkerPlace IN "
                                        "(SELECT CodWorkerPlace FROM workerplace WHERE coddepartment IN ( "
                                        "SELECT c.id FROM (SELECT * FROM departments) n, "
                                        "(SELECT * FROM tree) t, "
                                        "(SELECT * FROM departments) c "
                                        "WHERE n.id = %2 AND n.id = t.parent_id AND t.id = c.id))")
                                .arg(newDepWpId)
                                .arg(modelDepartment->data(modelDepartment->index(
                                                               curIndex.row(),
                                                               1,
                                                               curIndex.parent())).toInt()));
                if(ok){
                    updateWPModel(treeViewDepartments->currentIndex());
                    treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
                }else
                    qDebug()<<query.lastError().text();
            }
        }
    }else
        cancelMove = true;
}
bool WorkPlace::isFirm(const QModelIndex &ind)
{
    if(modelDepartment->data(modelDepartment->index(ind.row(),3,ind.parent())).toInt() == 1)
        return true;
    else
        return false;
}
bool WorkPlace::isFilPred(const QModelIndex &ind)
{
    if(modelDepartment->data(modelDepartment->index(ind.row(),4,ind.parent())).toInt() == 1)
        return true;
    else
        return false;
}
void WorkPlace::on_actionAddWP_triggered()
{
    if(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt() != 0){
        AddWorkerPlace *aw = new AddWorkerPlace(this);
        connect(aw,SIGNAL(wpAdded(QString,QString,QString,bool,int)),this,SLOT(newWpAdded(QString,QString,QString,bool,int)));
        aw->exec();
    }else{
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),tr("Выберите подразделение для создания рабочего места.\n"),tr("Закрыть"));
    }
}
void WorkPlace::newWpAdded(const QString &wpName, const QString &wpInternalNum, const QString &wpLocation, bool openEdit, int wh)
{
    bool ok;
    if (wh == 0){
        ok = wpModel->insertRecord(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt(),
                                   wpName,0,wpInternalNum,wpLocation);
    }else{
        ok = wpModel->insertRecord(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,treeViewDepartments->currentIndex().parent())).toInt(),
                                   wpName,0,wpInternalNum,wpLocation,wh);
    }
    if(!ok)
        qDebug()<<wpModel->lastError().text();
    for(int i=0;i < wpModel->rowCount(); i++){
        if(wpModel->lastInsertId().toInt() == wpModel->data(wpModel->index(i,0)).toInt()){
            treeViewWorkPlace->setCurrentIndex(wpModel->index(i,3));
            break;
        }
    }
    actionDelWP->setEnabled(true);
    actionEditWP->setEnabled(true);
    if(modelDepartment->data(modelDepartment->index(treeViewDepartments->currentIndex().row(),1,
                                                    treeViewDepartments->currentIndex().parent()))
        .toInt() != 0){
        actionAddWP->setEnabled(true);
        actionMoveWP->setEnabled(true);
        actionMoveWPAll->setEnabled(true);
    }else{
        actionAddWP->setEnabled(false);
        actionMoveWP->setEnabled(false);
        actionMoveWPAll->setEnabled(false);
    }
    if(openEdit)
        on_actionEditWP_triggered();
}
void WorkPlace::wpDataModelUpdate()
{
    int curWpId = wpModel->data(wpModel->index(treeViewWorkPlace->currentIndex().row(),0)).toInt();
    wpModel->select();
    for(int i=0;i < wpModel->rowCount(); i++){
        if(curWpId == wpModel->data(wpModel->index(i,0)).toInt()){
            treeViewWorkPlace->setCurrentIndex(wpModel->index(i,3));
            break;
        }
    }
}
void WorkPlace::on_actionEditWP_triggered()
{
    QSqlQuery query;
    int firmId = 0;
    QModelIndex index = treeViewWorkPlace->currentIndex();
    bool ok;
    ok = query.exec(QString("SELECT p.id FROM departments n, tree t, departments p "
                            "WHERE n.id = %1 AND n.id = t.id AND t.parent_id = p.id AND p.Firm = 1")
                    .arg(wpModel->data(wpModel->index(index.row(),1)).toInt()));
    if(ok){
        if(query.size()>0){
            query.next();
            firmId = query.value(0).toInt();
        }else{
            QMessageBox::warning(this,tr("Ошибка!!!"),tr("Не удалось получить данные о Фирме!!!"),tr("Закрыть"));
            return;
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить данные о Фирме:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }
    bool readOnly = false;
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->recordIsLosked(wpModel->data(wpModel->index(index.row(),0)).toInt(),
                                      "CodWorkerPlace",
                                      wpModel->tableName())){
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
    ok = query.exec(QString("SELECT RV FROM %1 WHERE CodWorkerPlace = %2")
                    .arg(wpModel->tableName())
                    .arg(wpModel->data(wpModel->index(index.row(),0)).toInt()));
    if(ok){
        query.next();
        if(wpModel->data(wpModel->index(index.row(),11)).toInt() != query.value(0).toInt()){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Запись была изменена, будут загружены новые данные записи"),
                                 tr("Закрыть"));
            wpDataModelUpdate();
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версии записи:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    }
    AddEditWorkerPlace *aew = new AddEditWorkerPlace(this,firmId,wpModel->dataRecord(index.row()),readOnly,wpModel->tableName());
    if(readOnly)
        aew->setWindowTitle(aew->windowTitle()+tr(" - [Только чтение]"));
    connect(aew,SIGNAL(wpDataChange()),this,SLOT(wpDataModelUpdate()));
    aew->setWindowFlags( Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    aew->exec();
}
void WorkPlace::on_actionDelWP_triggered()
{
    int button;
    bool ok;
    QModelIndex index = treeViewWorkPlace->currentIndex();
    QSqlQuery query;
    query.exec(QString("SELECT id FROM device WHERE CodWorkerPlace = %1")
               .arg(wpModel->data(wpModel->index(index.row(),0)).toInt()));
    if(query.size() > 0){
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Невозможно удалить данное рабочее место.\n"
                                    "Переместите все устройства с данного рабочего места,\n"
                                    "после чего повторите попытку."),
                                 tr("Закрыть"));
        return;
    }
    query.exec(QString("SELECT `key` FROM licenses WHERE CodWorkerPlace = %1")
               .arg(wpModel->data(wpModel->index(index.row(),0)).toInt()));
    if(query.size() > 0){
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Невозможно удалить данное рабочее место.\n"
                                    "Переместите все лицензии с данного рабочего места,\n"
                                    "после чего повторите попытку."),
                                 tr("Закрыть"));
        return;
    }
    button = QMessageBox::question(this,tr("Удаление рабочего места"),tr("Вы действительно хотите удалить данное рабочее место?"),
                                   tr("Да"),tr("Нет"),"",1,1);
    if(button)
        return;

    ok = wpModel->removeRecord(wpModel->data(wpModel->index(index.row(),0)).toInt());
    if(ok){
        if(wpModel->rowCount() > 0){
            if(wpModel->rowCount() != index.row())
                treeViewWorkPlace->setCurrentIndex(wpModel->index(index.row(),3));
            else
                treeViewWorkPlace->setCurrentIndex(wpModel->index(index.row()-1,3));
            treeViewWorkPlace->setFocus();
        }
    }else{
        qDebug()<<wpModel->lastError().text();
    }
}

void WorkPlace::on_close_clicked()
{
    emit closeWorkPlaceWin();
}
void WorkPlace::sortWPClick()
{
    treeViewWorkPlace->setCurrentIndex(wpModel->index(0,3));
}
void WorkPlace::changeEvent(QEvent *e)
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
