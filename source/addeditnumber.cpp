#include <QtWidgets>
#include <QtSql>
#include <QMessageBox>
#include "headers/addeditnumber.h"
#include "headers/edittable.h"
#include "headers/lockdatabase.h"

AddEditNumber::AddEditNumber(QWidget *parent, const QMap<QString, QVariant> &datas, bool editMode, bool readOnly) :
    QDialog(parent), m_data(datas), m_editMode(editMode), m_readOnly(readOnly)
{
    setupUi(this);
    user->setText(m_data.value("userName").toString());
    populateCBox("CodTypeNumber","typenumber","","<Выберите тип номера>",codTypeNumber);
    if(m_editMode){
        setDefaultData();
        if(m_readOnly){
            codTypeNumber->setEnabled(false);
            editTypeNumber->setEnabled(false);
            number->setReadOnly(true);
            note->setReadOnly(true);
        }else{
            lockedControl = new LockDataBase(this);
            lockedControl->lockRecordThread(m_data.value("userId").toInt(),"CodUser","users");
        }
    }
}

void AddEditNumber::changeEvent(QEvent *e)
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

void AddEditNumber::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            int button;
            if(dataChanged()){
                button = QMessageBox::question(this, tr("Внимание"),
                                               tr("Есть не сохранённые данные.\nХотите сохранить их?"),
                                               tr("Да"),tr("Нет"),"",0,1);
                if(button == 0){
                    if(!dataEntered()){
                        button = QMessageBox::question(this, tr("Внимание"),
                                                 tr("Отсутствуют данные обязательные для заполнения.\nЗакрыть без сохранения?"),
                                                 tr("Да"),tr("Нет"),"",1,1);
                        if(button == 1)
                            event->ignore();
                        else{
                            lockedControl->stopLockRecordThread(m_data.value("userId").toInt());
                            if(!lockedControl->unlockRecord(m_data.value("userId").toInt(),"CodUser","users")){
                                QMessageBox::warning(this,tr("Ошибка!!!"),
                                                     tr("Не удалось разблокировать запись:\n %1\n")
                                                     .arg(lockedControl->lastError().text()),
                                                     tr("Закрыть"));
                            }
                            event->accept();
                        }
                    }else{
                        on_buttonSave_clicked();
                        lockedControl->stopLockRecordThread(m_data.value("userId").toInt());
                        if(!lockedControl->unlockRecord(m_data.value("userId").toInt(),"CodUser","users")){
                            QMessageBox::warning(this,tr("Ошибка!!!"),
                                                 tr("Не удалось разблокировать запись:\n %1\n")
                                                 .arg(lockedControl->lastError().text()),
                                                 tr("Закрыть"));
                        }
                        event->accept();
                    }
                }else{
                    lockedControl->stopLockRecordThread(m_data.value("userId").toInt());
                    if(!lockedControl->unlockRecord(m_data.value("userId").toInt(),"CodUser","users")){
                        QMessageBox::warning(this,tr("Ошибка!!!"),
                                             tr("Не удалось разблокировать запись:\n %1\n")
                                             .arg(lockedControl->lastError().text()),
                                             tr("Закрыть"));
                    }
                    event->accept();
                }
            }else{
                lockedControl->stopLockRecordThread(m_data.value("userId").toInt());
                if(!lockedControl->unlockRecord(m_data.value("userId").toInt(),"CodUser","users")){
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось разблокировать запись:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                }
                event->accept();
            }
        }else{
            event->accept();
        }
    }else{
        event->accept();
    }
}
void AddEditNumber::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    cBox->addItem(nullStr);
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
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

void AddEditNumber::setDefaultData()
{
    number->setText(m_data.value("number").toString());
    note->setText(m_data.value("note").toString());
    codTypeNumber->setCurrentIndex(codTypeNumber->findData(m_data.value("codTypeNumber").toInt()));
}

void AddEditNumber::on_editTypeNumber_clicked()
{
    CeditTable edittable(this,QString("typenumber"));
    edittable.setWindowTitle(tr("Редактирование типов номера"));
    edittable.exec();
    codTypeNumber->clear();
    populateCBox("CodTypeNumber","typenumber","","<Выберите тип номера>",codTypeNumber);
}
void AddEditNumber::on_buttonSave_clicked()
{
    QSqlQuery addquery;
    if(!m_editMode){
        addquery.prepare("INSERT INTO listnumber (CodUser,CodTypeNumber,Number,Note) VALUES (?,?,?,?)");
        addquery.addBindValue(m_data.value("userId").toInt());
        addquery.addBindValue(codTypeNumber->itemData(codTypeNumber->currentIndex()).toInt());
        addquery.addBindValue(number->text());
        addquery.addBindValue(note->text());
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить номер:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        clearForm();
        emit numberAdded();
    }else{
        QQueue<QVariant> bindval;
        QString field;
        field += "CodTypeNumber = ?,";
        field += "Number = ?,";
        field += "Note = ?";
        field += " WHERE CodUser = ?";
        bindval.enqueue(codTypeNumber->itemData(codTypeNumber->currentIndex()).toInt());
        bindval.enqueue(number->text());
        bindval.enqueue(note->text());
        bindval.enqueue(m_data.value("userId").toInt());
        addquery.prepare(QString("UPDATE listnumber SET %2").arg(field));
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные номеров:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        addquery.prepare("UPDATE users SET RV = ? WHERE CodUser = ?");
        if(m_data.value("rv").toInt() == 254)
            addquery.addBindValue(0);
        else
            addquery.addBindValue(m_data.value("rv").toInt()+1);
        addquery.addBindValue(m_data.value("userId").toInt());
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить версию строки пользователя:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
        buttonSave->setEnabled(false);
        buttonRevert->setEnabled(false);
        m_data["codTypeNumber"] = codTypeNumber->itemData(codTypeNumber->currentIndex()).toInt();
        m_data["login"] = number->text();
        m_data["note"] = note->text();
        if(m_data.value("rv").toInt() == 254)
            m_data["rv"] = 0;
        else
            m_data["rv"] = m_data.value("rv").toInt()+1;
        emit numberDataChanged();
        QMessageBox::information(this, tr("Сохранение"),
                                 tr("Данные успешно сохранены!"),
                                 tr("Закрыть"));
    }
}
void AddEditNumber::on_buttonRevert_clicked()
{
    if(m_editMode)
        setDefaultData();
    else
        clearForm();
}
void AddEditNumber::checkData(bool checkDataEntered)
{
    if(!m_editMode){
        if(checkDataEntered)
            buttonSave->setEnabled(dataEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataEntered());
        }else{
            buttonSave->setEnabled(false);
            buttonRevert->setEnabled(false);
        }
    }
}
void AddEditNumber::on_codTypeNumber_currentIndexChanged(int)
{
    checkData(true);
}
void AddEditNumber::on_number_textChanged()
{
    checkData(true);
}
void AddEditNumber::on_note_textChanged()
{
    checkData();
}
bool AddEditNumber::dataEntered()
{
    if(number->text() != NULL  && codTypeNumber->currentIndex() != 0)
        return true;
    else
        return false;
}

bool AddEditNumber::formIsEmpty()
{
    if(number->text() == NULL && note->text() == NULL && codTypeNumber->currentIndex() == 0)
        return true;
    else
        return false;
}
bool AddEditNumber::dataChanged()
{
    if(m_data.value("number").toString() != number->text() || m_data.value("note").toString() != note->text() ||
            m_data.value("codTypeNumber").toInt() != codTypeNumber->itemData(codTypeNumber->currentIndex()).toInt())
        return true;
    else
        return false;
}
void AddEditNumber::clearForm()
{
    codTypeNumber->setCurrentIndex(0);
    number->setText("");
    note->setText("");
}
