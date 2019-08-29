#include <QtWidgets>
#include <QMessageBox>
#include "headers/addeditdepartments.h"
#include "headers/lockdatabase.h"

AddEditDepartments::AddEditDepartments(QWidget *parent, bool editMode, const QString &name, const QString &tabName, int editId, bool readOnly) :
    QDialog(parent), m_editMode(editMode), m_name(name), m_tabName(tabName), m_editId(editId), m_readOnly(readOnly)
{
    setupUi(this);
    if(editMode){
        if(!readOnly){
            lockedControl = new LockDataBase(this);
            lockedControl->lockRecordThread(editId,"id",tabName);
        }else
            newDepartment->setReadOnly(true);
        groupBox->setVisible(false);
        newDepartment->setText(name);
        label->setText(tr("Подразделение"));
        adjustSize();
    }
}
void AddEditDepartments::on_newDepartment_textChanged(QString text)
{
    if (!text.isEmpty() && !text.isNull()){
        if(text != m_name)
            addButton->setEnabled(true);
        else
            addButton->setEnabled(false);
    }else
        addButton->setEnabled(false);
}
void AddEditDepartments::on_addButton_clicked()
{
    if(!m_editMode){
        if(oneDepartment->isChecked()){
            if(!newDepartment->text().isNull() && !newDepartment->text().isEmpty())
                emit newDepartmentAdd(newDepartment->text(),false);
            AddEditDepartments::accept();
        }else{
            if(!newDepartment->text().isNull() && !newDepartment->text().isEmpty()){
                emit newDepartmentAdd(newDepartment->text(),true);
                newDepartment->setText("");
            }
        }
    }else{
        if(m_name != newDepartment->text())
            emit newDepartmentAdd(newDepartment->text(),false);
        if(!m_readOnly){
            lockedControl->stopLockRecordThread(m_editId);
            if(!lockedControl->unlockRecord(m_editId,"id",m_tabName)){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
        AddEditDepartments::accept();
    }
}
void AddEditDepartments::on_cancelButton_clicked()
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
void AddEditDepartments::changeEvent(QEvent *e)
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
void AddEditDepartments::closeEvent(QCloseEvent *event){
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
