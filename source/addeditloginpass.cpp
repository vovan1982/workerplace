#include <QtWidgets>
#include <QtSql>
#include <QMessageBox>
#include "headers/addeditloginpass.h"
#include "headers/edittable.h"
#include "headers/lockdatabase.h"

AddEditLoginPass::AddEditLoginPass(QWidget *parent, const QMap<QString, QVariant> &datas, bool editMode, bool readOnly) :
    QDialog(parent), m_data(datas), m_editMode(editMode), m_readOnly(readOnly)
{
    setupUi(this);
    user->setText(m_data.value("userName").toString());
    populateCBox("CodLoginType","logintype","","<Выберите тип логина>",codLoginType);
    if(m_editMode){
        setDefaultData();
        if(m_readOnly){
            codLoginType->setEnabled(false);
            editLoginType->setEnabled(false);
            login->setReadOnly(true);
            pass->setReadOnly(true);
            note->setReadOnly(true);
        }else{
            timer = new QTimer(this);
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->lockRecord(m_data.value("userId").toInt(),"CodUser","users")){
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

void AddEditLoginPass::changeEvent(QEvent *e)
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

void AddEditLoginPass::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_data.value("userId").toInt(),"CodUser","users")){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    event->accept();
}

void AddEditLoginPass::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_data.value("userId").toInt(),"CodUser","users")){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}

void AddEditLoginPass::populateCBox(const QString &idName, const QString &tableName,
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

void AddEditLoginPass::setDefaultData()
{
    login->setText(m_data.value("login").toString());
    pass->setText(m_data.value("pass").toString());
    note->setText(m_data.value("note").toString());
    codLoginType->setCurrentIndex(codLoginType->findData(m_data.value("codLoginType").toInt()));
}

void AddEditLoginPass::on_editLoginType_clicked()
{
    CeditTable edittable(this,QString("logintype"));
    edittable.setWindowTitle(tr("Редактирование типов лонина"));
    edittable.exec();
    codLoginType->clear();
    populateCBox("CodLoginType","logintype","","<Выберите тип логина>",codLoginType);
}
void AddEditLoginPass::on_buttonSave_clicked()
{
    QSqlQuery addquery;
    if(!m_editMode){
        addquery.prepare("INSERT INTO loginpass (CodLoginPass,CodLoginType,Login,Pass,Note) VALUES (?,?,?,?,?)");
        addquery.addBindValue(m_data.value("userId").toInt());
        addquery.addBindValue(codLoginType->itemData(codLoginType->currentIndex()).toInt());
        addquery.addBindValue(login->text());
        addquery.addBindValue(pass->text());
        addquery.addBindValue(note->text());
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить логин и пароль:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        clearForm();
        emit loginAdded();
    }else{
        QQueue<QVariant> bindval;
        QString field;
        field += "CodLoginType = ?,";
        field += "Login = ?,";
        field += "Pass = ?,";
        field += "Note = ?";
        field += " WHERE CodLoginPass = ?";
        bindval.enqueue(codLoginType->itemData(codLoginType->currentIndex()).toInt());
        bindval.enqueue(login->text());
        bindval.enqueue(pass->text());
        bindval.enqueue(note->text());
        bindval.enqueue(m_data.value("userId").toInt());
        addquery.prepare(QString("UPDATE loginpass SET %2").arg(field));
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные логинов и паролей:\n %1")
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
        m_data["codLoginType"] = codLoginType->itemData(codLoginType->currentIndex()).toInt();
        m_data["login"] = login->text();
        m_data["pass"] = pass->text();
        m_data["note"] = note->text();
        if(m_data.value("rv").toInt() == 254)
            m_data["rv"] = 0;
        else
            m_data["rv"] = m_data.value("rv").toInt()+1;
        emit loginDataChanged();
        QMessageBox::information(this, tr("Сохранение"),
                                 tr("Данные успешно сохранены!"),
                                 tr("Закрыть"));
    }
}
void AddEditLoginPass::on_buttonRevert_clicked()
{
    if(m_editMode)
        setDefaultData();
    else
        clearForm();
}
void AddEditLoginPass::checkData(bool checkDataEntered)
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
void AddEditLoginPass::on_codLoginType_currentIndexChanged(int)
{
    checkData(true);
}
void AddEditLoginPass::on_login_textChanged()
{
    checkData(true);
}
void AddEditLoginPass::on_pass_textChanged()
{
    checkData();
}
void AddEditLoginPass::on_note_textChanged()
{
    checkData();
}
bool AddEditLoginPass::dataEntered()
{
    if(login->text() != NULL  && codLoginType->currentIndex() != 0)
        return true;
    else
        return false;
}

bool AddEditLoginPass::formIsEmpty()
{
    if(login->text() == NULL && pass->text() == NULL && note->text() == NULL &&
       codLoginType->currentIndex() == 0)
        return true;
    else
        return false;
}
bool AddEditLoginPass::dataChanged()
{
    if(m_data.value("login").toString() != login->text() || m_data.value("pass").toString() != pass->text() ||
            m_data.value("note").toString() != note->text() ||
            m_data.value("codLoginType").toInt() != codLoginType->itemData(codLoginType->currentIndex()).toInt())
        return true;
    else
        return false;
}
void AddEditLoginPass::clearForm()
{
    codLoginType->setCurrentIndex(0);
    login->setText("");
    pass->setText("");
    note->setText("");
}
