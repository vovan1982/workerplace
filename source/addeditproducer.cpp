#include <QtWidgets>
#include <QtSql>
#include <QMessageBox>
#include "headers/addeditproducer.h"
#include "headers/lockdatabase.h"

AddEditProducer::AddEditProducer(QWidget *parent, bool editMode, QSqlRecord rec, bool readOnly) :
    QDialog(parent), m_editMode(editMode), m_rec(rec), m_readOnly(readOnly){
    setupUi(this);
    if(editMode){
        name->setText(rec.value(1).toString());
        www->setText(rec.value(2).toString());
        note->setText(rec.value(3).toString());
        if(readOnly){
            name->setReadOnly(true);
            www->setReadOnly(true);
            note->setReadOnly(true);
        }else{
            timer = new QTimer(this);
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->lockRecord(m_rec.value(0).toInt(),"CodProducer","producer")){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось заблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }else{
                connect(timer,SIGNAL(timeout()),this,SLOT(updateLockRecord()));
                timer->start(30000);
            }
        }
    }
}
void AddEditProducer::on_name_textEdited(QString text)
{
    if(!m_editMode){
        if(!text.isNull() && !text.isEmpty()){
            saveButton->setEnabled(true);
            revertButton->setEnabled(true);
        }else{
            saveButton->setEnabled(false);
            if(formIsEmpty())
                revertButton->setEnabled(false);
        }
    }else{
        if(!text.isNull() && !text.isEmpty()){
            if(dataChanged()){
                revertButton->setEnabled(true);
                saveButton->setEnabled(true);
            }else{
                revertButton->setEnabled(false);
                saveButton->setEnabled(false);
            }
        }else{
            saveButton->setEnabled(false);
            revertButton->setEnabled(true);
        }
    }
}
bool AddEditProducer::formIsEmpty()
{
    if(name->text().isEmpty() && www->text().isEmpty() && note->text().isEmpty())
        return true;
    else
        return false;
}
bool AddEditProducer::dataChanged()
{
    if(name->text() != m_rec.value(1).toString() || www->text() != m_rec.value(2).toString() ||
       note->text() != m_rec.value(3).toString())
        return true;
    else
        return false;
}
void AddEditProducer::on_www_textEdited()
{
    if(!m_editMode){
        if(formIsEmpty())
            revertButton->setEnabled(false);
        else
            revertButton->setEnabled(true);
    }else{
        if(dataChanged()){
            revertButton->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                saveButton->setEnabled(true);
        }else{
            revertButton->setEnabled(false);
            saveButton->setEnabled(false);
        }
    }
}
void AddEditProducer::on_note_textEdited()
{
    if(!m_editMode){
        if(formIsEmpty())
            revertButton->setEnabled(false);
        else
            revertButton->setEnabled(true);
    }else{
        if(dataChanged()){
            revertButton->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                saveButton->setEnabled(true);
        }else{
            revertButton->setEnabled(false);
            saveButton->setEnabled(false);
        }
    }
}
void AddEditProducer::clearForm()
{
    name->setText("");
    www->setText("");
    note->setText("");
    revertButton->setEnabled(false);
    saveButton->setEnabled(false);
}
void AddEditProducer::on_revertButton_clicked()
{
    if(m_editMode){
        name->setText(m_rec.value(1).toString());
        www->setText(m_rec.value(2).toString());
        note->setText(m_rec.value(3).toString());
        revertButton->setEnabled(false);
        saveButton->setEnabled(false);
    }else
        clearForm();
}
void AddEditProducer::on_saveButton_clicked()
{
    QSqlQuery addquery;
    QQueue<QVariant> bindval;
    QString field, val, queryStr;
    if(!m_editMode){
        field = "("; val = "(";
        field += "Name"; val += "?";
        bindval.enqueue(name->text());
        if(!www->text().isNull() && !www->text().isEmpty()){
            field += ",www"; val += ",?";
            bindval.enqueue(www->text());}
        if(!note->text().isNull() && !note->text().isEmpty()){
            field += ",Note"; val += ",?";
            bindval.enqueue(note->text());}
        field += ")"; val += ")";
        queryStr = "INSERT INTO producer "+field+" VALUES "+val;
        addquery.prepare(queryStr);
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить производителя:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        QMessageBox::information(this, tr("Добавление производителя"),
                                 tr("Новый производитель успешно добавлен!"),
                                 tr("Закрыть"));
        clearForm();
    }else{
        addquery.exec(QString("UPDATE producer SET Name = '%1', www = '%2', Note = '%3' WHERE CodProducer = %4")
                      .arg(name->text()).arg(www->text()).arg(note->text()).arg(m_rec.value(0).toInt()));
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные производителя:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        revertButton->setEnabled(false);
        saveButton->setEnabled(false);
        m_rec.setValue(1,name->text());
        m_rec.setValue(2,www->text());
        m_rec.setValue(3,note->text());
        QMessageBox::information(this, tr("Обновление данных"),
                                 tr("Данные успешно обновленны!"),
                                 tr("Закрыть"));
    }
    emit producerAdded();
}
void AddEditProducer::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_rec.value(0).toInt(),"CodProducer","producer")){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditProducer::changeEvent(QEvent *e)
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
void AddEditProducer::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_rec.value(0).toInt(),"CodProducer","producer")){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    event->accept();
}
