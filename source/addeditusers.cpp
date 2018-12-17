#include <QtWidgets>
#include <QtSql>
#include <QMessageBox>
#include "headers/addeditusers.h"
#include "headers/edittable.h"
#include "headers/lockdatabase.h"

AddEditUsers::AddEditUsers(QWidget *parent, bool wpMode, const QMap<QString,QVariant> &datas, bool editMode) :
    QDialog(parent), m_wpMode(wpMode), m_data(datas), m_editMode(editMode)
{
    setupUi(this);
    populateCBox("CodPost","post","","<Выберите должность>",codPost);
    populateCBox("id","departments","firm = 1","<Выберите фирму>",codFirm);
    dateInWp->setDate(QDate::currentDate());
    if(m_editMode){
        setDefaultData();
        timer = new QTimer(this);
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->lockRecord(m_data.value("codUser").toInt(),"CodUser","users")){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось заблокировать запись:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }else{
            connect(timer,SIGNAL(timeout()),this,SLOT(updateLockRecord()));
            timer->start(30000);
        }
    }
    if(wpMode){
        codFirm->setCurrentIndex(codFirm->findData(m_data.value("firmId").toInt()));
        codFirm->setEnabled(false);
    }else{
        dateInWp->setVisible(false);
        label_10->setVisible(false);
    }
}

void AddEditUsers::changeEvent(QEvent *e)
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

void AddEditUsers::closeEvent(QCloseEvent *event){
    if(m_editMode){
        timer->stop();
        delete timer;
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->unlockRecord(m_data.value("codUser").toInt(),"CodUser","users")){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать запись:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
    }
    event->accept();
}

void AddEditUsers::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_data.value("codUser").toInt(),"CodUser","users")){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}

void AddEditUsers::populateCBox(const QString &idName, const QString &tableName,
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

void AddEditUsers::clearForm()
{
    lastName->setText("");
    name->setText("");
    middleName->setText("");
    fioSummary->setText("");
    codAD->setText("");
    codPost->setCurrentIndex(0);
    if(!m_wpMode)
        codFirm->setCurrentIndex(0);
    email->setText("");
    additionalemail->setText("");
    note->setText("");
    buttonSave->setEnabled(false);
    buttonRevert->setEnabled(false);
}

bool AddEditUsers::dataEntered()
{
    if(!fioSummary->text().isNull() && !fioSummary->text().isEmpty() && codPost->currentIndex() != 0 &&
       codFirm->currentIndex() != 0)
        return true;
    else
        return false;
}

bool AddEditUsers::formIsEmpty()
{
    if((lastName->text().isEmpty() || lastName->text().isNull()) &&
            (name->text().isEmpty() || name->text().isNull()) &&
            (middleName->text().isEmpty() || middleName->text().isNull()) &&
            (fioSummary->text().isEmpty() || fioSummary->text().isNull()) &&
            codPost->currentIndex() == 0 &&
            (codAD->text().isEmpty() || codAD->text().isNull()) &&
            (email->text().isEmpty() || email->text().isNull()) &&
            (additionalemail->text().isEmpty() || additionalemail->text().isNull()) &&
            (note->text().isEmpty() || note->text().isNull()))
    {
        if(!m_wpMode && codFirm->currentIndex() == 0)
            return true;
        if(m_wpMode)
            return true;
    }
    return false;
}

bool AddEditUsers::dataChanged()
{
    if(m_data.value("lastName").toString() != lastName->text() || m_data.value("name").toString() != name->text() ||
       m_data.value("middleName").toString() != middleName->text() || m_data.value("fioSummary").toString() != fioSummary->text() ||
       m_data.value("firmId").toInt() != codFirm->itemData(codFirm->currentIndex()).toInt() ||
       m_data.value("codAD").toString() != codAD->text() || m_data.value("codPost").toInt() != codPost->itemData(codPost->currentIndex()).toInt() ||
       m_data.value("email").toString() != email->text() || m_data.value("additionalemail").toString() != additionalemail->text() ||
            m_data.value("note").toString() != note->text())
        return true;
    else
        return false;
}

void AddEditUsers::setDefaultData()
{
    lastName->setText(m_data.value("lastName").toString());
    name->setText(m_data.value("name").toString());
    middleName->setText(m_data.value("middleName").toString());
    fioSummary->setText(m_data.value("fioSummary").toString());
    codFirm->setCurrentIndex(codFirm->findData(m_data.value("firmId").toInt()));
    codAD->setText(m_data.value("codAD").toString());
    codPost->setCurrentIndex(codPost->findData(m_data.value("codPost").toInt()));
    email->setText(m_data.value("email").toString());
    additionalemail->setText(m_data.value("additionalemail").toString());
    note->setText(m_data.value("note").toString());
}

void AddEditUsers::on_buttonSave_clicked()
{
    QSqlQuery addquery, query;
    if(!m_editMode){
        if (!codAD->text().isEmpty()){
            query.exec(QString("SELECT CodAD FROM users WHERE CodAD=%1;").arg(codAD->text()));
            if (query.next())
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Пользователь с кодом '%1' уже существует.\nДобавление пользователя не выполненно!!!")
                                         .arg(codAD->text()),
                                         tr("Закрыть"));
                return;
            }
            addquery.prepare("INSERT INTO users (CodAD,LastName,Name,MiddleName,FIOSummary,"
                             "CodPost,email,additionalemail,CodOrganization,Note) VALUES (?,?,?,?,?,?,?,?,?)");
            addquery.addBindValue(codAD->text());
        }else{
            addquery.prepare("INSERT INTO users (LastName,Name,MiddleName,FIOSummary,"
                             "CodPost,email,additionalemail,CodOrganization,Note) VALUES (?,?,?,?,?,?,?,?)");
        }
        addquery.addBindValue(lastName->text());
        addquery.addBindValue(name->text());
        addquery.addBindValue(middleName->text());
        addquery.addBindValue(fioSummary->text());
        addquery.addBindValue(codPost->itemData(codPost->currentIndex()).toInt());
        addquery.addBindValue(email->text());
        addquery.addBindValue(additionalemail->text());
        addquery.addBindValue(codFirm->itemData(codFirm->currentIndex()).toInt());
        addquery.addBindValue(note->text());
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить пользователя:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        if(m_wpMode){
            query.exec(QString("INSERT INTO workplaceandusers SET CodWorkerPlace = %1, CodUser = %2;")
                       .arg(m_data.value("wpId").toInt())
                       .arg(addquery.lastInsertId().toInt()));
            if (query.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось присвоить рабочее место:\n %1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
            }
            query.prepare(QString("INSERT INTO historyuseronwp SET CodUser = ?, CodWorkerPlace = ?, DateIn = ?"));
            query.addBindValue(addquery.lastInsertId().toInt());
            query.addBindValue(m_data.value("wpId").toInt());
            query.addBindValue(dateInWp->date());
            query.exec();
            if (query.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось записать историю прихода на рабочее место:\n %1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
            }
        }
        emit userAdded();
        clearForm();
    }else{
        QQueue<QVariant> bindval;
        QString field;
        int button = 3;
        if(codFirm->itemData(codFirm->currentIndex()).toInt() != m_data.value("firmId").toInt())
            button = QMessageBox::warning(this,tr("ВНИМАНИЕ!!!"),
                                          tr("Вы изменили Фирму пользователя, все связи с рабочими местами\n"
                                             "и подразделениями будут удалены!!!\n"
                                             "Вы действительно хотите изменить фирму пользователя?"),
                                          tr("Да"),tr("Нет"),tr("Отмена"),2,2);
        switch (button){
        case 0:
            query.exec(QString("DELETE FROM workplaceandusers WHERE CODUSER = %1")
                       .arg(m_data.value("codUser").toInt()));
            if(query.lastError().type() != QSqlError::NoError){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось удалить связи с рабочими местами:\n %1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
            query.exec(QString("UPDATE tounits SET CodDepartment = NULL, CodFilPred = NULL WHERE CodUser = %1;")
                       .arg(m_data.value("codUser").toInt()));
            if(query.lastError().type() != QSqlError::NoError){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось удалить связи с подразделениями:\n %1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
            break;
        case 1:
            codFirm->setCurrentIndex(codFirm->findData(m_data.value("firmId").toInt()));
            break;
        case 2:return;
        }
        if (!codAD->text().isEmpty() && !codAD->text().isNull()){
            query.exec(QString("SELECT CodAD FROM users WHERE CodAD=%1;").arg(codAD->text()));
            if (query.next())
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Пользователь с кодом '%1' уже существует.\nИзмените код и повторите попытку сохранения.")
                                         .arg(codAD->text()),
                                         tr("Закрыть"));
                return;
            }
            field = "CodAD = ?,";
            bindval.enqueue(codAD->text());
        }else{
            field = "CodAD = NULL,";
        }
        field += "FIOSummary = ?,";
        field += "CodPost = ?,";
        field += "CodOrganization = ?,";
        field += "LastName = ?,";
        field += "Name = ?,";
        field += "MiddleName = ?,";
        field += "email = ?,";
        field += "additionalemail = ?,";
        field += "Note = ?,";
        bindval.enqueue(fioSummary->text());
        bindval.enqueue(codPost->itemData(codPost->currentIndex()).toInt());
        bindval.enqueue(codFirm->itemData(codFirm->currentIndex()).toInt());
        bindval.enqueue(lastName->text());
        bindval.enqueue(name->text());
        bindval.enqueue(middleName->text());
        bindval.enqueue(email->text());
        bindval.enqueue(additionalemail->text());
        bindval.enqueue(note->text());
        field += "RV = ? WHERE CodUser = ?";
        if(m_data.value("rv").toInt() == 254)
            bindval.enqueue(0);
        else
            bindval.enqueue(m_data.value("rv").toInt()+1);
        bindval.enqueue(m_data.value("codUser").toInt());
        addquery.prepare(QString("UPDATE users SET %2").arg(field));
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные пользователя:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        buttonSave->setEnabled(false);
        buttonRevert->setEnabled(false);
        m_data["firmId"] = codFirm->itemData(codFirm->currentIndex()).toInt();
        m_data["lastName"] = lastName->text();
        m_data["name"] = name->text();
        m_data["middleName"] = middleName->text();
        m_data["fioSummary"] = fioSummary->text();
        m_data["codAD"] = codAD->text();
        m_data["codPost"] = codPost->itemData(codPost->currentIndex()).toInt();
        m_data["email"] = email->text();
        m_data["additionalemail"] = additionalemail->text();
        m_data["note"] = note->text();
        if(m_data.value("rv").toInt() == 254)
            m_data["rv"] = 0;
        else
            m_data["rv"] = m_data.value("rv").toInt()+1;
        emit userDataChanged();
        QMessageBox::information(this, tr("Сохранение"),
                                 tr("Данные успешно сохранены!"),
                                 tr("Закрыть"));
    }
}

void AddEditUsers::on_buttonPopulateFio_clicked()
{
    if ((!lastName->text().isNull() && !lastName->text().isEmpty()) &&
            (!name->text().isNull() && !name->text().isEmpty()) &&
            (!middleName->text().isNull() && !middleName->text().isEmpty())){
        QString ln, n, str;
        ln = name->text().simplified();
        n = middleName->text().simplified();
        str = lastName->text().simplified()+" ";
        str += ln.data()[0];
        str += ".";
        str += n.data()[0];
        str += ".";
        fioSummary->setText(str);
    }

    if ((lastName->text().isNull() || lastName->text().isEmpty()) &&
            (!name->text().isNull() && !name->text().isEmpty()) &&
            (!middleName->text().isNull() && !middleName->text().isEmpty()))
        fioSummary->setText(name->text().simplified()+" "+middleName->text().simplified());

    if ((!lastName->text().isNull() && !lastName->text().isEmpty()) &&
            (name->text().isNull() || name->text().isEmpty()) &&
            (!middleName->text().isNull() && !middleName->text().isEmpty()))
        fioSummary->setText(lastName->text().simplified());

    if ((!lastName->text().isNull() && !lastName->text().isEmpty()) &&
            (!name->text().isNull() && !name->text().isEmpty()) &&
            (middleName->text().isNull() || middleName->text().isEmpty())){
        QString ln, str;
        ln = name->text().simplified();
        str = lastName->text().simplified()+" ";
        str += ln.data()[0];
        str += ".";
        fioSummary->setText(str);
    }

    if ((lastName->text().isNull() || lastName->text().isEmpty()) &&
            (name->text().isNull() || name->text().isEmpty()) &&
            (!middleName->text().isNull() && !middleName->text().isEmpty()))
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Автолаполнение не возможно!!!"),
                                 tr("Закрыть"));

    if ((!lastName->text().isNull() && !lastName->text().isEmpty()) &&
            (name->text().isNull() || name->text().isEmpty()) &&
            (middleName->text().isNull() || middleName->text().isEmpty()))
        fioSummary->setText(lastName->text().simplified());

    if ((lastName->text().isNull() || lastName->text().isEmpty()) &&
            (!name->text().isNull() && !name->text().isEmpty()) &&
            (middleName->text().isNull() || middleName->text().isEmpty()))
        fioSummary->setText(name->text().simplified());

    if ((lastName->text().isNull() || lastName->text().isEmpty()) &&
            (name->text().isNull() || name->text().isEmpty()) &&
            (middleName->text().isNull() || middleName->text().isEmpty()))
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Автолаполнение не возможно!!!"),
                                 tr("Закрыть"));
}

void AddEditUsers::on_buttonEditPost_clicked()
{

    CeditTable edittable(this,QString("post"));
    edittable.setWindowTitle(tr("Редактирование должностей"));
    edittable.exec();
    codPost->clear();
    populateCBox("CodPost","post","","<Выберите должность>",codPost);
}

void AddEditUsers::checkData(bool checkDataEntered)
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

void AddEditUsers::on_lastName_textChanged()
{
    checkData(true);
}

void AddEditUsers::on_name_textChanged()
{
    checkData(true);
}

void AddEditUsers::on_middleName_textChanged()
{
    checkData(true);
}

void AddEditUsers::on_fioSummary_textChanged()
{
    checkData(true);
}

void AddEditUsers::on_codPost_currentIndexChanged(int)
{
    checkData(true);
}

void AddEditUsers::on_codFirm_currentIndexChanged(int)
{
    checkData(true);
}

void AddEditUsers::on_email_textChanged()
{
    checkData();
}

void AddEditUsers::on_additionalemail_textChanged()
{
    checkData();
}

void AddEditUsers::on_codAD_textChanged(const QString &)
{
    checkData();
}

void AddEditUsers::on_note_textChanged(const QString &)
{
    checkData();
}

void AddEditUsers::on_buttonRevert_clicked()
{
    if(m_editMode)
        setDefaultData();
    else
        clearForm();
}
