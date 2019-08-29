#include <QtWidgets>
#include <QKeyEvent>
#include <QMessageBox>
#include "headers/addedittypedevice.h"
#include "headers/delegats.h"
#include "headers/selecticons.h"
#include "headers/lockdatabase.h"

AddEditTypeDevice::AddEditTypeDevice(QWidget *parent,const QString &tName, int type, bool allType) :
    QDialog(parent), m_tName(tName), m_type(type)
{
    setupUi(this);
    readOnly = false;
    model = new QSqlTableModel(this);
    model->setTable(tName);
    if(!allType)
        model->setFilter(QString("Type = %1").arg(type));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal,tr("Иконка"));
    model->setHeaderData(2, Qt::Horizontal,tr("Наименование"));
    model->setHeaderData(3, Qt::Horizontal,tr("Тип"));
    typeDeviceView->setModel(model);
    typeDeviceView->setColumnHidden(0,true);
    typeDeviceView->horizontalHeader()->setStretchLastSection(true);
    if(!allType)
        typeDeviceView->setColumnHidden(3,true);
    typeDeviceView->setItemDelegateForColumn(1,new TypeDevIconDelegat(typeDeviceView));
    typeDeviceView->setItemDelegateForColumn(3,new TypeDevDelegat(typeDeviceView));
    editMode = false;
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataModelEdit()));
    typeDeviceView->setCurrentIndex(model->index(0,1));
    typeDeviceView->resizeColumnsToContents();
    typeDeviceView->resizeRowsToContents();

    lockedControl = new LockDataBase(this);

    if(lockedControl->referenceBookIsLocked(tName)){
        readOnly = true;
        setWindowTitle(windowTitle()+tr(" - [Только чтение]"));
        addButton->setEnabled(false);
        delButton->setEnabled(false);
        buttonEditIcon->setEnabled(false);
        delIcoButton->setEnabled(false);
        typeDeviceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
        }else{
            lockedControl->lockReferenceBookThread(tName);
        }
    }
}
void AddEditTypeDevice::changeButton(bool ch)
{
    revertButton->setEnabled(ch);
    submitButton->setEnabled(ch);
    editMode = ch;
}
void AddEditTypeDevice::dataModelEdit()
{
    changeButton(true);
}
void AddEditTypeDevice::on_addButton_clicked()
{
    model->insertRow(model->rowCount());
    model->setData(model->index(model->rowCount()-1,3),m_type);
    typeDeviceView->selectRow(model->rowCount()-1);
    typeDeviceView->setFocus();
    changeButton(true);
}

void AddEditTypeDevice::on_delButton_clicked()
{
    QModelIndex index;
    index = typeDeviceView->currentIndex();
    model->removeRow(index.row());
    changeButton(true);
}

void AddEditTypeDevice::on_submitButton_clicked()
{
    QModelIndex index = typeDeviceView->currentIndex();
    model->submitAll();
    if (model->lastError().type() != QSqlError::NoError){
        model->revertAll();
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ни все изменения удалось применить:\n %1").arg(model->lastError().text()),
                                 "Закрыть");
        model->submitAll();
    }
    changeButton(false);
    if(model->rowCount() > index.row())
        typeDeviceView->setCurrentIndex(index);
    else
        typeDeviceView->setCurrentIndex(model->index(0,1));
}

void AddEditTypeDevice::on_revertButton_clicked()
{
    model->revertAll();
    model->submitAll();
    typeDeviceView->selectRow(0);
    typeDeviceView->setFocus();
    changeButton(false);
}

void AddEditTypeDevice::setCurentIcon(const QString &icoFile)
{
    QModelIndex index = typeDeviceView->currentIndex();
    model->setData(model->index(index.row(),1),icoFile);
}

void AddEditTypeDevice::on_buttonEditIcon_clicked()
{
    SelectIcons *si = new SelectIcons(this,"/ico/typedeviceico/");
    connect(si,SIGNAL(iconSelected(QString)),this,SLOT(setCurentIcon(QString)));
    si->exec();
}

void AddEditTypeDevice::on_delIcoButton_clicked()
{
    QModelIndex index = typeDeviceView->currentIndex();
    model->setData(model->index(index.row(),1),"");
}

void AddEditTypeDevice::keyPressEvent(QKeyEvent * e)
{
    if(!readOnly){
        if (e->key() == Qt::Key_Insert){
            typeDeviceView->model()->insertRow(typeDeviceView->model()->rowCount());
            typeDeviceView->selectRow(typeDeviceView->model()->rowCount()-1);
            typeDeviceView->setFocus();
            changeButton(true);
        }
        if((e->key() == Qt::Key_Escape) && (editMode)){
            model->revertAll();
            model->submitAll();
            typeDeviceView->selectRow(0);
            typeDeviceView->setFocus();
            changeButton(false);
        }
        if (e->key() == Qt::Key_Delete){
            QModelIndex index;
            index = typeDeviceView->currentIndex();
            model->removeRow(index.row());
            typeDeviceView->closePersistentEditor(index);
            typeDeviceView->selectRow(index.row());
            typeDeviceView->setFocus();
            changeButton(true);
        }
        if ((e->key() == Qt::Key_S) && (e->modifiers() == Qt::ControlModifier) && (editMode))
        {
            model->submitAll();
            if (model->lastError().type() != QSqlError::NoError){
                model->revertAll();
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Ни все изменения удалось применить:\n %1").arg(model->lastError().text()),
                                         tr("Закрыть"));
                model->submitAll();
            }
            changeButton(false);
        }
    }
}

void AddEditTypeDevice::on_closeButton_clicked()
{
    emit closeTypeDeviceWin();
}

void AddEditTypeDevice::changeEvent(QEvent *e)
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
void AddEditTypeDevice::closeEvent(QCloseEvent *event)
{
    if(!readOnly){
        lockedControl->stopLockReferenceBookThread(m_tName);
        if(!lockedControl->unlockReferenceBook(m_tName))
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать справочник:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
    }
    model->clear();
    event->accept();
}
