#include <QMessageBox>
#include <QCloseEvent>
#include "headers/addeditfirm.h"
#include "headers/lockdatabase.h"

AddEditFirm::AddEditFirm(QWidget *parent, const QString &tabName, bool editMode, int editId, bool readOnly) :
    QDialog(parent), m_tabName(tabName), m_editMode(editMode), m_editId(editId), m_readOnly(readOnly)
{
    QSqlQuery query;
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    if(editMode){
        if(!query.exec(QString("SELECT name FROM %1 WHERE id = %2").arg(tabName).arg(editId))){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось получить данные о Фирме:\n %1\n")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(!readOnly){
            timer = new QTimer(this);
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->lockRecord(editId,"id",tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось заблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }else{
                connect(timer,SIGNAL(timeout()),this,SLOT(updateLockRecord()));
                timer->start(30000);
            }
        }else
            firmName->setReadOnly(true);
        query.next();
        firmName->setText(query.value(0).toString());
        curName = query.value(0).toString();
    }
}
void AddEditFirm::on_firmName_textEdited(QString str)
{
    if (!str.isEmpty() && !str.isNull()){
        if(str != curName)
            saveButton->setEnabled(true);
        else
            saveButton->setEnabled(false);
    }else
        saveButton->setEnabled(false);
}
void AddEditFirm::on_saveButton_clicked()
{
    bool ok;
    QSqlQuery query;
    if(!m_editMode){
        ok = query.exec(QString("SELECT * FROM %1 WHERE name = '%2' AND firm = 1")
                   .arg(m_tabName)
                   .arg(firmName->text()));
        if(ok){
            if(query.size() <= 0){
                ok = query.exec(QString("INSERT INTO %1 (parent_id, name, firm) VALUES (0, '%2', 1)")
                                .arg(m_tabName)
                                .arg(firmName->text()));
                if(ok){
                    emit newFirmAdd(firmName->text(), query.lastInsertId().toInt());
                    AddEditFirm::accept();
                }else{
                    qDebug()<<query.lastError().text();
                    AddEditFirm::reject();
                }
            }else{
                QMessageBox::information(this," ",tr("Фирма с таким названием уже существует!!!"),tr("Закрыть"));
                return;
            }
        }else{
            qDebug()<<query.lastError().text();
            AddEditFirm::reject();
        }
    }else{
        if(curName != firmName->text())
            emit editFirm(firmName->text());
        if(!m_readOnly){
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_editId,"id",m_tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
        AddEditFirm::accept();
    }
}
void AddEditFirm::on_cancelButton_clicked()
{
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_editId,"id",m_tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    reject();
}
void AddEditFirm::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_editId,"id",m_tabName)){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditFirm::changeEvent(QEvent *e)
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
void AddEditFirm::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_editId,"id",m_tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    event->accept();
}
