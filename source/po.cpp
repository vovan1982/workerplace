#include <QtWidgets>
#include <QDir>
#include <QMenu>
#include <QMessageBox>
#include "headers/po.h"
#include "headers/pomodel.h"
#include "headers/editandselectproducer.h"
#include "headers/selecticons.h"
#include "headers/selectpo.h"
#include "headers/lockdatabase.h"

Po::Po(QWidget *parent, int curId) :
    QWidget(parent){
    setupUi(this);
    readOnly = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateLockReferenceBook()));
    poModel = new PoModel(viewPo);
    viewPo->setModel(poModel);
    populatePoModel();
    setAccessToActions();
    for(int i = 1;i<poModel->columnCount();i++)
        viewPo->setColumnHidden(i,true);
    connect(viewPo->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateEditForm(QModelIndex)));
    connect(viewPo,SIGNAL(collapsed(QModelIndex)),viewPo,SLOT(setCurrentIndex(QModelIndex)));
    populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
    if(poModel->rowCount()>0){
        if(curId != 0)
            viewPo->setCurrentIndex(poModel->findData(curId));
        else
            viewPo->setCurrentIndex(poModel->index(0,0,QModelIndex()));
        hideIfGroup(isGroup(viewPo->currentIndex()));
        curIndex = viewPo->currentIndex();
    }
    QActionGroup *ag = new QActionGroup(this);
    ag->setExclusive(true);
    ag->addAction(actionAddGroup);
    ag->addAction(actionAddSubGroup);
    ag->addAction(actionAddPo);
    ag->addAction(actionDel);
    ag->addAction(actionMove);
    menu = new QMenu(tr("Действия"), this);
    menu->addActions(ag->actions());
    actionsButton->setMenu(menu);
    connect(viewPo, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onPoMenu(const QPoint &)));

    LockDataBase *lockedControl = new LockDataBase(this);
    if(lockedControl->referenceBookIsLocked("po")){
        readOnly = true;
        setWindowTitle(windowTitle()+tr(" - [Только чттение]"));
        setAccessToActions();
        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Справочник заблокирован пользователем: %1")
                                 .arg(lockedControl->referenceBookBlockingUser()),
                                 tr("Закрыть"));
    }else{
        if(lockedControl->lastError().type() != QSqlError::NoError){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось получить информацию о блокировке:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
            return;
        }else{
            if(!lockedControl->lockReferenceBook("po"))
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось заблокировать выбранный справочник:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            else
                timer->start(30000);
        }
    }
}
void Po::onPoMenu(const QPoint &p){
   menu->exec(viewPo->viewport()->mapToGlobal(p));
}
void Po::populatePoModel()
{
    poModel->select();
    poModel->setHeaderData(0, Qt::Horizontal,tr("Наименование"));
}
void Po::on_closeButton_clicked()
{
    emit closePoWin();
}
void Po::on_actionAddGroup_triggered()
{
    poModel->sqlInsertRow(poModel->rowCount(),tr("Новая группа"));
    viewPo->setCurrentIndex(poModel->index(poModel->rowCount()-1,0));
    setAccessToActions();
}
void Po::on_actionAddSubGroup_triggered()
{
    QModelIndex parentIndex;
    if(poModel->data(poModel->index(viewPo->currentIndex().row(),3,viewPo->currentIndex().parent())).toInt() == 1){
        parentIndex = viewPo->currentIndex();
        poModel->sqlInsertRow(poModel->rowCount(parentIndex),tr("Новая подгруппа"),
                              poModel->data(poModel->index(parentIndex.row(),1,parentIndex.parent())).toInt(),
                              parentIndex);
    }else{
        parentIndex = viewPo->currentIndex().parent();
        poModel->sqlInsertRow(poModel->rowCount(parentIndex),tr("Новая подгруппа"),
                              poModel->data(poModel->index(parentIndex.row(),1,parentIndex.parent())).toInt(),
                              parentIndex);
    }
    viewPo->setCurrentIndex(poModel->index(poModel->rowCount(parentIndex)-1,0,parentIndex));
}
void Po::on_actionAddPo_triggered()
{
    QModelIndex parentIndex;
    if(poModel->data(poModel->index(viewPo->currentIndex().row(),3,viewPo->currentIndex().parent())).toInt() == 1){
        parentIndex = viewPo->currentIndex();
        poModel->sqlInsertRow(poModel->rowCount(parentIndex),tr("Новое программное обеспечение"),
                              poModel->data(poModel->index(parentIndex.row(),1,parentIndex.parent())).toInt(),
                              parentIndex,0);
    }else{
        parentIndex = viewPo->currentIndex().parent();
        poModel->sqlInsertRow(poModel->rowCount(parentIndex),tr("Новое программное обеспечение"),
                              poModel->data(poModel->index(parentIndex.row(),1,parentIndex.parent())).toInt(),
                              parentIndex,0);
    }
    viewPo->setCurrentIndex(poModel->index(poModel->rowCount(parentIndex)-1,0,parentIndex));
}
void Po::on_actionMove_triggered()
{
    SelectPo *sp = new SelectPo(this,
                            QString("isGroup = 1 AND CodPO NOT IN ( "
                                    "SELECT c.CodPO FROM po n, potree t, po c "
                                    "WHERE n.CodPO = %1 AND n.CodPO = t.parent_id AND t.id = c.CodPO)")
                            .arg(poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt()));
    sp->setWindowTitle(tr("Смена группы ПО"));
    connect(sp,SIGNAL(itemSelected(int)),this,SLOT(moveItem(int)));
    sp->exec();
}
void Po::on_actionDel_triggered()
{
    int button;
    bool ok;
    QSqlQuery query;
    if(poModel->data(poModel->index(viewPo->currentIndex().row(),3,viewPo->currentIndex().parent())).toInt() == 0){
        button = QMessageBox::question(this,tr("Удаление программного обеспечения"),
                                      tr("Вы действительно хотите удалить данное программное обеспечение?"),
                                      tr("Да"),tr("Нет"),"",1,1);
        if(button == 1)
            return;
        ok = query.exec(QString("SELECT * FROM licenses WHERE CodPO = %1")
                   .arg(poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt()));
        if (ok){
            if(query.size()>0)
                QMessageBox::information(this, tr("Внимание"),
                                         tr("Данное ПО невозможно удалить!"),
                                         "Закрыть");
            else
                poModel->sqlRemoveRow(viewPo->currentIndex().row(),viewPo->currentIndex().parent());
        }else{
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось выполнить проверку данного ПО. \n %1").arg(query.lastError().text()),
                                     "Закрыть");
        }
    }else{
        button = QMessageBox::warning(this,tr("Удаление группы программного обеспечения"),
                                      tr("ВНИМАНИЕ!!! При удалении группы ПО будут удалены все подгруппы "
                                         "и всё программое обеспечение находящееся в данной группе и её "
                                         "подгруппах.\n"
                                         "Вы действительно хотите удалить данную группу программного обеспечения?"),
                                      tr("Да"),tr("Нет"),"",1,1);
        if(button == 1)
            return;
        ok = query.exec(QString("SELECT * FROM licenses WHERE CodPO IN ( "
                                "SELECT c.CodPO FROM po n, potree t, po c "
                                "WHERE n.CodPO = %1 AND n.CodPO = t.parent_id AND t.id = c.CodPO AND c.isGroup = 0)")
                        .arg(poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt()));
        if (ok){
            if(query.size()>0)
                QMessageBox::information(this, tr("Внимание"),
                                         tr("Данную группу ПО невозможно удалить!"),
                                         "Закрыть");
            else
                poModel->sqlRemoveRow(viewPo->currentIndex().row(),viewPo->currentIndex().parent());
        }else{
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось выполнить проверку данной группы ПО. \n %1").arg(query.lastError().text()),
                                     "Закрыть");
        }
    }
    setAccessToActions();
}
void Po::moveItem(int newParentId)
{
    if(poModel->data(poModel->index(viewPo->currentIndex().row(),2,viewPo->currentIndex().parent())).toInt() != newParentId)
        viewPo->setCurrentIndex(
                    poModel->moveItem(poModel->data(poModel->index(viewPo->currentIndex().row(),1,
                                                                   viewPo->currentIndex().parent())).toInt(),
                                      newParentId));

}
bool Po::isGroup(const QModelIndex &index)
{
    if(poModel->data(poModel->index(index.row(),3,index.parent())).toInt() == 1)
        return true;
    else
        return false;
}
void Po::hideIfGroup(bool isGr)
{
    label_2->setVisible(!isGr);
    label_3->setVisible(!isGr);
    label_5->setVisible(!isGr);
    producer->setVisible(!isGr);
    editAndSelectProducersButton->setVisible(!isGr);
    note->setVisible(!isGr);
    viewIco->setVisible(!isGr);
    editIcoButton->setVisible(!isGr);
}
void Po::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    cBox->addItem(nullStr,0);
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
void Po::updateEditForm(const QModelIndex &index)
{
    if(saveButton->isEnabled()){
        int button = QMessageBox::question(this,tr("Сохранение"),
                                      tr("Есть не сохранённые данные.\n Хотите сохранить их?"),
                                      tr("Да"),tr("Нет"),"",1,1);
        if(button == 0)
            on_saveButton_clicked();
    }
    namePo->setText(poModel->data(poModel->index(index.row(),0,index.parent())).toString());
    if(isGroup(index)){
        hideIfGroup(true);
        producer->setCurrentIndex(0);
        note->setText("");
        viewIco->setPixmap(QPixmap());
        iconFile = "";
    }else{
        hideIfGroup(false);
        producer->setCurrentIndex(producer->findData(poModel->data(poModel->index(index.row(),4,index.parent())).toInt()));
        note->setText(poModel->data(poModel->index(index.row(),5,index.parent())).toString());
        iconFile = poModel->data(poModel->index(index.row(),6,index.parent())).toString();
        if(poModel->data(poModel->index(index.row(),6,index.parent())).toString().isEmpty() ||
                poModel->data(poModel->index(index.row(),6,index.parent())).toString().isNull())
            viewIco->setPixmap(QPixmap(QDir::currentPath()+"/ico/typepoico/Default.png").scaled(64,64));
        else
            viewIco->setPixmap(QPixmap(QDir::currentPath()+"/ico/typepoico/"+iconFile).scaled(64,64));
    }
    curIndex = index;
    saveButton->setEnabled(false);
}
void Po::on_saveButton_clicked()
{
    if(isGroup(curIndex)){
        poModel->setData(poModel->index(curIndex.row(),0,curIndex.parent()),namePo->text());
    }else{
        poModel->setData(poModel->index(curIndex.row(),0,curIndex.parent()),namePo->text());
        if(producer->itemData(producer->currentIndex()).toInt() <= 0){
            poModel->setData(poModel->index(curIndex.row(),4,curIndex.parent()),QVariant(QVariant::Int));
        }else{
            poModel->setData(poModel->index(curIndex.row(),4,curIndex.parent()),producer->itemData(producer->currentIndex()));
        }
        poModel->setData(poModel->index(curIndex.row(),5,curIndex.parent()),note->text());
        poModel->setData(poModel->index(curIndex.row(),6,curIndex.parent()),iconFile);
    }
    saveButton->setEnabled(false);
}
void Po::on_editAndSelectProducersButton_clicked()
{
    int curProdId = producer->itemData(producer->currentIndex()).toInt();
    EditAndSelectProducer *easp = new EditAndSelectProducer(this);
    connect(easp,SIGNAL(producerSelected(int)),this,SLOT(setCurentProd(int)));
    if(!easp->exec()){
        populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
        producer->setCurrentIndex(producer->findData(curProdId));
    }
}
void Po::setCurentProd(int idProd)
{
    populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
    producer->setCurrentIndex(producer->findData(idProd));
}
void Po::on_editIcoButton_clicked()
{
    SelectIcons *si = new SelectIcons(this,"/ico/typepoico/");
    connect(si,SIGNAL(iconSelected(QString)),this,SLOT(setCurentIcon(QString)));
    si->exec();
}
void Po::setCurentIcon(const QString &icoFile)
{
    iconFile = icoFile;
    viewIco->setPixmap(QPixmap(QDir::currentPath()+"/ico/typepoico/"+iconFile).scaled(64,64));
    if(dataIsChanged())
        saveButton->setEnabled(true);
    else
        saveButton->setEnabled(false);
}
void Po::setAccessToActions()
{
    if(poModel->rowCount()>0){
        if(!readOnly){
            actionAddSubGroup->setEnabled(true);
            actionAddPo->setEnabled(true);
            actionDel->setEnabled(true);
            actionMove->setEnabled(true);
        }else{
            actionAddGroup->setEnabled(false);
            actionAddSubGroup->setEnabled(false);
            actionAddPo->setEnabled(false);
            actionDel->setEnabled(false);
            actionMove->setEnabled(false);
            namePo->setReadOnly(true);
            producer->setEnabled(false);
            editAndSelectProducersButton->setEnabled(false);
            note->setReadOnly(true);
            editIcoButton->setEnabled(false);
        }
    }else{
        actionAddSubGroup->setEnabled(false);
        actionAddPo->setEnabled(false);
        actionDel->setEnabled(false);
        actionMove->setEnabled(false);
        if(readOnly){
            actionAddGroup->setEnabled(false);
            namePo->setReadOnly(true);
            producer->setEnabled(false);
            editAndSelectProducersButton->setEnabled(false);
            note->setReadOnly(true);
            editIcoButton->setEnabled(false);
        }
    }
}
bool Po::dataIsChanged()
{
    if(poModel->data(poModel->index(viewPo->currentIndex().row(),0,viewPo->currentIndex().parent())).toString() != namePo->text() ||
            poModel->data(poModel->index(viewPo->currentIndex().row(),4,viewPo->currentIndex().parent())).toInt() != producer->itemData(producer->currentIndex()).toInt() ||
            poModel->data(poModel->index(viewPo->currentIndex().row(),5,viewPo->currentIndex().parent())).toString() != note->text() ||
            poModel->data(poModel->index(viewPo->currentIndex().row(),6,viewPo->currentIndex().parent())).toString() != iconFile)
        return true;
    else
        return false;
}
void Po::on_namePo_textEdited()
{
    if(dataIsChanged())
        saveButton->setEnabled(true);
    else
        saveButton->setEnabled(false);
}
void Po::on_producer_currentIndexChanged(int)
{
    if(dataIsChanged())
        saveButton->setEnabled(true);
    else
        saveButton->setEnabled(false);
}
void Po::on_note_textEdited()
{
    if(dataIsChanged())
        saveButton->setEnabled(true);
    else
        saveButton->setEnabled(false);
}
void Po::showCloseButton(bool show)
{
    closeButton->setVisible(show);
}
void Po::updateLockReferenceBook()
{
    if(!readOnly){
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->lockReferenceBook("po")){
            timer->stop();
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось продлить блокировку выбраного справочника:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
    }
}
void Po::changeEvent(QEvent *e)
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
void Po::closeEvent(QCloseEvent *event)
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!readOnly){
        if(!lockedControl->unlockReferenceBook("po"))
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать справочник:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
    }
    event->accept();
}
