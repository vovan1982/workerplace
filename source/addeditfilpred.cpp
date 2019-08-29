#include <QtWidgets>
#include <QMessageBox>
#include "headers/addeditfilpred.h"
#include "headers/lockdatabase.h"

AddEditFilPred::AddEditFilPred(QWidget *parent, const QString &tabName, int parentId, bool editMode, int editId, bool readOnly) :
    QDialog(parent), m_tabName(tabName), m_parentId(parentId), m_editMode(editMode), m_editId(editId), m_readOnly(readOnly)
{
    QSqlQuery query;
    setupUi(this);
    if(editMode){
        if(!query.exec(QString("SELECT name FROM %1 WHERE id = %2").arg(tabName).arg(editId))){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось получить данные о Филиале/Представительстве:\n %1\n")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(!readOnly){
            lockedControl = new LockDataBase(this);
            lockedControl->lockRecordThread(editId,"id",tabName);
        }else
            filpredName->setReadOnly(true);
        query.next();
        filpredName->setText(query.value(0).toString());
        curName = query.value(0).toString();
    }
}
void AddEditFilPred::on_filpredName_textEdited(QString str)
{
    if (!str.isEmpty() && !str.isNull()){
        if(str != curName)
            saveButton->setEnabled(true);
        else
            saveButton->setEnabled(false);
    }else
        saveButton->setEnabled(false);
}
void AddEditFilPred::on_saveButton_clicked()
{
    bool ok;
    QSqlQuery query;
    if(!m_editMode){
        ok = query.exec(QString("INSERT INTO %1 (parent_id, name, fp) VALUES (%3, '%2', 1)")
                        .arg(m_tabName)
                        .arg(filpredName->text())
                        .arg(m_parentId));
        if(ok){
            emit newFilPredAdd(filpredName->text(), query.lastInsertId().toInt());
            AddEditFilPred::accept();
        }else{
            qDebug()<<query.lastError().text();
            AddEditFilPred::reject();
        }
    }else{
        if(curName != filpredName->text())
            emit editFilPred(filpredName->text());
        if(!m_readOnly){
            lockedControl->stopLockRecordThread(m_editId);
            if(!lockedControl->unlockRecord(m_editId,"id",m_tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
        AddEditFilPred::accept();
    }
}
void AddEditFilPred::on_cancelButton_clicked()
{
    if(m_editMode){
        if(!m_readOnly){
            lockedControl->stopLockRecordThread(m_editId);
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
void AddEditFilPred::changeEvent(QEvent *e)
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
void AddEditFilPred::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            lockedControl->stopLockRecordThread(m_editId);
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
