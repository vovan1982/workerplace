#include <QtSql>
#include <QMenu>
#include <QMessageBox>
#include "headers/providers.h"
#include "headers/addeditprovider.h"
#include "headers/lockdatabase.h"

Providers::Providers(QWidget *parent, bool editMode, bool selectMode) :
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
    model = new QSqlTableModel(providersView);
    populateModel();
    providersView->setModel(model);
    providersView->setColumnHidden(0,true);
    providersView->setColumnHidden(7,true);
    providersView->setColumnHidden(8,true);
    providersView->setColumnHidden(9,true);
    providersView->resizeColumnToContents(1);
    providersView->setCurrentIndex(model->index(0,1));
    if(model->rowCount() <= 0){
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
    connect(providersView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_editButton_clicked()));
    connect(providersView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onProvMenu(const QPoint &)));
}
void Providers::onProvMenu(const QPoint &p){
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(actionAdd);
    ag->addAction(actionDel);
    ag->addAction(actionEdit);
    QMenu *menu = new QMenu(tr("Поставщики"), this);
    menu->addActions(ag->actions());
    menu->exec(providersView->viewport()->mapToGlobal(p));
}
void Providers::populateModel()
{
    model->clear();
    model->setTable("provider");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal,tr("Наименование"));
    model->setHeaderData(2, Qt::Horizontal,tr("Адрес"));
    model->setHeaderData(3, Qt::Horizontal,tr("Факс"));
    model->setHeaderData(4, Qt::Horizontal,tr("Телефон"));
    model->setHeaderData(5, Qt::Horizontal,tr("Эл. Почта"));
    model->setHeaderData(6, Qt::Horizontal,tr("Сайт"));
}

void Providers::on_addButton_clicked()
{
    AddEditProvider *aep = new AddEditProvider(this);
    aep->setWindowTitle(tr("Добавление поставщика"));
    connect(aep,SIGNAL(providerAdded()),this,SLOT(updateView()));
    aep->exec();
}
void Providers::on_editButton_clicked()
{
    QModelIndex index = providersView->currentIndex();
    bool readOnly = false;
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->recordIsLosked(model->data(model->index(index.row(),0)).toInt(),"CodProvider","provider")){
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
    ok = query.exec(QString("SELECT RV FROM provider WHERE CodProvider = %1")
                    .arg(model->data(model->index(index.row(),0)).toInt()));
    if(ok){
        query.next();
        if(model->data(model->index(index.row(),9)).toInt() != query.value(0).toInt()){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                 tr("Запись была изменена, будут загружены новые данные записи"),
                                 tr("Закрыть"));
            int id = model->data(model->index(index.row(),0)).toInt();
            populateModel();
            for(int i = 0;i<model->rowCount();i++){
                if(model->index(i,0).data().toInt() == id){
                    providersView->setCurrentIndex(model->index(i,0));
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
    AddEditProvider *aep = new AddEditProvider(this,true,model->record(providersView->currentIndex().row()),readOnly);
    if(readOnly)
        aep->setWindowTitle(tr("Редактирование поставщика - [Только чтение]"));
    else
        aep->setWindowTitle(tr("Редактирование поставщика"));
    aep->setAttribute(Qt::WA_DeleteOnClose);
    connect(aep,SIGNAL(providerAdded()),this,SLOT(updateView()));
    aep->exec();
}
void Providers::on_delButton_clicked()
{
    int button = QMessageBox::question(this,tr("Удаление поставщика"),
                                       tr("Внимание!!!\n"
                                          "Вы уверены что хотите удалить данного поставщика?"),
                                       tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    QModelIndex index = providersView->currentIndex();
    model->removeRow(index.row());
    model->submitAll();
    if (model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка!"),
                                 tr("Не удалось удалить поставщика:\n %1")
                                 .arg(model->lastError().text()),
                                 tr("Закрыть"));
        return;
    }
    if(model->rowCount() > 0){
        if(model->rowCount()<=index.row())
            providersView->setCurrentIndex(model->index(model->rowCount()-1,index.column()));
        else
            providersView->setCurrentIndex(index);
        actionDel->setEnabled(true);
        actionEdit->setEnabled(true);
        delButton->setEnabled(true);
        editButton->setEnabled(true);
    }else{
        providersView->setCurrentIndex(QModelIndex());
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
}

void Providers::updateView()
{
    QModelIndex index = providersView->currentIndex();
    model->submitAll();
    if(model->rowCount() > 0){
        if(model->rowCount()<=index.row())
            providersView->setCurrentIndex(model->index(model->rowCount()-1,index.column()));
        else
            providersView->setCurrentIndex(index);
        actionDel->setEnabled(true);
        actionEdit->setEnabled(true);
        delButton->setEnabled(true);
        editButton->setEnabled(true);
    }else{
        providersView->setCurrentIndex(QModelIndex());
        actionDel->setEnabled(false);
        actionEdit->setEnabled(false);
        delButton->setEnabled(false);
        editButton->setEnabled(false);
    }
}

void Providers::on_closeButton_clicked()
{
    emit closeProviderWin();
}
void Providers::on_selectButton_clicked()
{
    QList<QVariant> provider;
    for(int i = 0;i<model->columnCount();i++){
        provider<<model->data(model->index(providersView->currentIndex().row(),i));
    }
    emit selectedRowData(provider);
    emit closeProviderWin();
}
void Providers::on_actionAdd_triggered()
{
    on_addButton_clicked();
}
void Providers::on_actionDel_triggered()
{
    on_delButton_clicked();
}
void Providers::on_actionEdit_triggered()
{
    on_editButton_clicked();
}
void Providers::changeEvent(QEvent *e)
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
