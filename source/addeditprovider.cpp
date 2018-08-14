#include <QtWidgets>
#include <QtSql>
#include <QMessageBox>
#include "headers/addeditprovider.h"
#include "headers/lockdatabase.h"

AddEditProvider::AddEditProvider(QWidget *parent, bool editMode, QSqlRecord rec, bool readOnly) :
    QDialog(parent), m_editMode(editMode), m_rec(rec), m_readOnly(readOnly){
    setupUi(this);
    if(editMode){
        name->setText(rec.value(1).toString());
        adress->setText(rec.value(2).toString());
        fax->setText(rec.value(3).toString());
        phone->setText(rec.value(4).toString());
        email->setText(rec.value(5).toString());
        www->setText(rec.value(6).toString());
        if(readOnly){
            name->setReadOnly(true);
            adress->setReadOnly(true);
            fax->setReadOnly(true);
            phone->setReadOnly(true);
            email->setReadOnly(true);
            www->setReadOnly(true);
        }else{
            timer = new QTimer(this);
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->lockRecord(m_rec.value(0).toInt(),"CodProvider","provider")){
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
void AddEditProvider::on_name_textEdited(QString text)
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
bool AddEditProvider::formIsEmpty()
{
    if(name->text().isEmpty() && adress->text().isEmpty() && fax->text().isEmpty() &&
       phone->text().isEmpty() && email->text().isEmpty() && www->text().isEmpty())
        return true;
    else
        return false;
}
bool AddEditProvider::dataChanged()
{
    if(name->text() != m_rec.value(1).toString() || adress->text() != m_rec.value(2).toString() ||
       fax->text() != m_rec.value(3).toString() || phone->text() != m_rec.value(4).toString() ||
       email->text() != m_rec.value(5).toString() || www->text() != m_rec.value(6).toString())
        return true;
    else
        return false;
}

void AddEditProvider::on_adress_textEdited()
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
void AddEditProvider::on_fax_textEdited()
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
void AddEditProvider::on_phone_textEdited()
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
void AddEditProvider::on_email_textEdited()
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
void AddEditProvider::on_www_textEdited()
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
void AddEditProvider::clearForm()
{
    name->setText("");
    adress->setText("");
    fax->setText("");
    phone->setText("");
    email->setText("");
    www->setText("");
    revertButton->setEnabled(false);
    saveButton->setEnabled(false);
}
void AddEditProvider::on_revertButton_clicked()
{
    if(m_editMode){
        name->setText(m_rec.value(1).toString());
        adress->setText(m_rec.value(2).toString());
        fax->setText(m_rec.value(3).toString());
        phone->setText(m_rec.value(4).toString());
        email->setText(m_rec.value(5).toString());
        www->setText(m_rec.value(6).toString());
        revertButton->setEnabled(false);
        saveButton->setEnabled(false);
    }else
        clearForm();
}
void AddEditProvider::on_saveButton_clicked()
{
    QSqlQuery addquery;
    QQueue<QVariant> bindval;
    QString field, val, queryStr;
    if(!m_editMode){
        field = "("; val = "(";
        field += "Name"; val += "?";
        bindval.enqueue(name->text());
        if(!adress->text().isNull() && !adress->text().isEmpty()){
            field += ",Adress"; val += ",?";
            bindval.enqueue(adress->text());}
        if(!fax->text().isNull() && !fax->text().isEmpty()){
            field += ",Fax"; val += ",?";
            bindval.enqueue(fax->text());}
        if(!phone->text().isNull() && !phone->text().isEmpty()){
            field += ",Phone"; val += ",?";
            bindval.enqueue(phone->text());}
        if(!email->text().isNull() && !email->text().isEmpty()){
            field += ",email"; val += ",?";
            bindval.enqueue(email->text());}
        if(!www->text().isNull() && !www->text().isEmpty()){
            field += ",www"; val += ",?";
            bindval.enqueue(www->text());}
        field += ")"; val += ")";
        queryStr = "INSERT INTO provider "+field+" VALUES "+val;
        addquery.prepare(queryStr);
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить поставщика:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        QMessageBox::information(this, tr("Добавление поставщика"),
                                 tr("Новый поставщик успешно добавлен!"),
                                 tr("Закрыть"));
        clearForm();
    }else{
        addquery.exec(QString("UPDATE provider SET Name = '%1', Adress = '%2', Fax = '%3', Phone = '%4', "
                              "email = '%5', www = '%6' WHERE CodProvider = %7")
                      .arg(name->text()).arg(adress->text()).arg(fax->text())
                      .arg(phone->text()).arg(email->text()).arg(www->text())
                      .arg(m_rec.value(0).toInt()));
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные поставщика:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        revertButton->setEnabled(false);
        saveButton->setEnabled(false);
        m_rec.setValue(1,name->text());
        m_rec.setValue(2,adress->text());
        m_rec.setValue(3,fax->text());
        m_rec.setValue(4,phone->text());
        m_rec.setValue(5,email->text());
        m_rec.setValue(6,www->text());
        QMessageBox::information(this, tr("Обновление данных"),
                                 tr("Данные успешно обновленны!"),
                                 tr("Закрыть"));
    }
    emit providerAdded();
}
void AddEditProvider::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_rec.value(0).toInt(),"CodProvider","provider")){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditProvider::changeEvent(QEvent *e)
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
void AddEditProvider::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_rec.value(0).toInt(),"CodProvider","provider")){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    event->accept();
}
