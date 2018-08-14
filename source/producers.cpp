#include <QtSql>
#include <QMenu>
#include <QMessageBox>
#include "headers/producers.h"
#include "headers/addeditproducer.h"
#include "headers/lockdatabase.h"

Producers::Producers(QWidget *parent, bool editMode, bool selectMode) :
    QWidget(parent){
    setupUi(this);
    if (editMode){
        closeButton->setVisible(false);
        selectButton->setVisible(false);
        horizontalLayout->removeItem(horizontalSpacer);
    }else{
        frame->setVisible(!selectMode);
        selectButton->setVisible(selectMode);
    }
    model = new QSqlTableModel(producersView);
    populateModel();
    producersView->setModel(model);
    producersView->setColumnHidden(0,true);
    producersView->setColumnHidden(4,true);
    producersView->setColumnHidden(5,true);
    producersView->setColumnHidden(6,true);
    producersView->resizeColumnToContents(1);
    producersView->setCurrentIndex(model->index(0,1));
    if(model->rowCount() <= 0){
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
    connect(producersView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_editButton_clicked()));
    connect(producersView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onProdMenu(const QPoint &)));
}
void Producers::onProdMenu(const QPoint &p){
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(actionAdd);
    ag->addAction(actionDel);
    ag->addAction(actionEdit);
    QMenu *menu = new QMenu(tr("Производитель"), this);
    menu->addActions(ag->actions());
    menu->exec(producersView->viewport()->mapToGlobal(p));
}
void Producers::populateModel()
{
    model->clear();
    model->setTable("producer");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal,tr("Наименование"));
    model->setHeaderData(2, Qt::Horizontal,tr("Сайт"));
    model->setHeaderData(3, Qt::Horizontal,tr("Примечание"));
}

void Producers::on_addButton_clicked()
{
    AddEditProducer *aep = new AddEditProducer(this);
    aep->setWindowTitle(tr("Добавление производителя"));
    connect(aep,SIGNAL(producerAdded()),this,SLOT(updateView()));
    aep->exec();
}
void Producers::on_editButton_clicked()
{
    QModelIndex index = producersView->currentIndex();
    bool readOnly = false;
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->recordIsLosked(model->data(model->index(index.row(),0)).toInt(),"CodProducer","producer")){
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
    ok = query.exec(QString("SELECT RV FROM producer WHERE CodProducer = %1")
                    .arg(model->data(model->index(index.row(),0)).toInt()));
    if(ok){
        query.next();
        if(model->data(model->index(index.row(),6)).toInt() != query.value(0).toInt()){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Запись была изменена, будут загружены новые данные записи"),
                                 tr("Закрыть"));
            int id = model->data(model->index(index.row(),0)).toInt();
            populateModel();
            for(int i = 0;i<model->rowCount();i++){
                if(model->index(i,0).data().toInt() == id){
                    producersView->setCurrentIndex(model->index(i,0));
                    break;
                }
            }
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версии записи:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    }
    AddEditProducer *aep = new AddEditProducer(this,true,model->record(producersView->currentIndex().row()),readOnly);
    if(readOnly)
        aep->setWindowTitle(tr("Редактирование производителя - [Только чтение]"));
    else
        aep->setWindowTitle(tr("Редактирование производителя"));
    aep->setAttribute(Qt::WA_DeleteOnClose);
    connect(aep,SIGNAL(producerAdded()),this,SLOT(updateView()));
    aep->exec();
}
void Producers::on_delButton_clicked()
{
    int button = QMessageBox::question(this,tr("Удаление производителя"),
                                       tr("Внимание!!!\n"
                                          "Вы уверены что хотите удалить данного производителя?"),
                                       tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    QModelIndex index = producersView->currentIndex();
    model->removeRow(index.row());
    model->submitAll();
    if (model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка!"),
                                 tr("Не удалось удалить производителя:\n %1")
                                 .arg(model->lastError().text()),
                                 tr("Закрыть"));
        return;
    }
    if(model->rowCount() > 0){
        if(model->rowCount()<=index.row())
            producersView->setCurrentIndex(model->index(model->rowCount()-1,index.column()));
        else
            producersView->setCurrentIndex(index);
        actionDel->setEnabled(true);
        actionEdit->setEnabled(true);
        delButton->setEnabled(true);
        editButton->setEnabled(true);
    }else{
        producersView->setCurrentIndex(QModelIndex());
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
}

void Producers::updateView()
{
    QModelIndex index = producersView->currentIndex();
    model->submitAll();
    if(model->rowCount() > 0){
        if(model->rowCount()<=index.row())
            producersView->setCurrentIndex(model->index(model->rowCount()-1,index.column()));
        else
            producersView->setCurrentIndex(index);
        actionDel->setEnabled(true);
        actionEdit->setEnabled(true);
        delButton->setEnabled(true);
        editButton->setEnabled(true);
    }else{
        producersView->setCurrentIndex(QModelIndex());
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
}

void Producers::on_closeButton_clicked()
{
    emit closeProducerWin();
}
int Producers::currentItemId()
{
    return model->data(model->index(producersView->currentIndex().row(),0)).toInt();
}
void Producers::on_selectButton_clicked()
{
    QList<QVariant> producer;
    for(int i = 0;i<model->columnCount();i++){
        producer<<model->data(model->index(producersView->currentIndex().row(),i));
    }
    emit selectedRowData(producer);
    emit closeProducerWin();
}
void Producers::on_actionAdd_triggered()
{
    on_addButton_clicked();
}
void Producers::on_actionDel_triggered()
{
    on_delButton_clicked();
}
void Producers::on_actionEdit_triggered()
{
    on_editButton_clicked();
}
void Producers::changeEvent(QEvent *e)
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
