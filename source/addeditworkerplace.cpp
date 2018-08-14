#include <QtWidgets>
#include "headers/addeditworkerplace.h"
#include "headers/users.h"
#include "headers/device.h"
#include "headers/licenses.h"
#include "headers/addeditnetworkdata.h"
#include "headers/lockdatabase.h"
#include "headers/journalhistoryusersonwp.h"
#include "headers/journalhistorymoved.h"

AddEditWorkerPlace::AddEditWorkerPlace(QWidget *parent, int wpFirmId, const QMap<int, QVariant> record, bool readOnly, const QString &tableName) :
    QDialog(parent), m_readOnly(readOnly), m_tableName(tableName)
{
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    m_wpId = record.value(0).toInt();
    m_wpDepId = record.value(1).toInt();
    m_wpName = record.value(3).toString();
    m_wpPUserId = record.value(4).toInt();
    m_wpPUser = record.value(5).toString();
    m_wpIntNum = record.value(6).toString();
    m_wpLocation = record.value(7).toString();
    m_wpWareHouse = record.value(10).toInt();
    m_wpRV = record.value(11).toInt();
    name->setText(m_wpName);
    internalNumber->setText(m_wpIntNum);
    primaryUser->setText(m_wpPUser);
    location->setText(m_wpLocation);
    tempWpPUserId = m_wpPUserId;
    name->setReadOnly(readOnly);
    internalNumber->setReadOnly(readOnly);
    location->setReadOnly(readOnly);
    delPUserButton->setDisabled(readOnly);
    if(!readOnly){
        timer = new QTimer(this);
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->lockRecord(m_wpId,"CodWorkerPlace",tableName)){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось заблокировать запись:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }else{
            connect(timer,SIGNAL(timeout()),this,SLOT(updateLockRecord()));
            timer->start(30000);
        }
    }
    wUsers = new Cusers(users,true,wpFirmId,m_wpId,readOnly);
    connect(wUsers,SIGNAL(primaryUserSet(QString,int)),this,SLOT(setPrimaryUser(QString,int)));
    connect(wUsers,SIGNAL(updatePrimaryUser()),this,SLOT(updatePrimaryUser()));
    if(m_wpWareHouse != 0){
        this->setWindowTitle(tr("Склад"));
        this->label_3->setText(tr("Зав. склада:"));
        wDevice = new Device(orgtex,true,m_wpId,wpFirmId,true,readOnly);
        wLicenses = new Licenses(licenses,true,m_wpId,true,readOnly);
        tabWidget->removeTab(3);
    }else{
        wDevice = new Device(orgtex,true,m_wpId,wpFirmId,false,readOnly);
        wLicenses = new Licenses(licenses,true,m_wpId,readOnly);
        wAddEditNetworkData = new AddEditNetworkData(network,m_wpId,readOnly);
        verticalLayout_9->addWidget(wAddEditNetworkData);
    }
    verticalLayout_2->addWidget(wUsers);
    verticalLayout_5->addWidget(wDevice);
    verticalLayout_7->addWidget(wLicenses);
    connect(wDevice,SIGNAL(devModelUpdated()),wLicenses,SLOT(updateLicenseModel()));
}
void AddEditWorkerPlace::setPrimaryUser(const QString &fio, int id)
{
    tempWpPUserId = id;
    primaryUser->setText(fio);
}
void AddEditWorkerPlace::updatePrimaryUser()
{
    QSqlQuery query;
    QString queryText = "";
    if(query.exec(QString("SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1").arg(m_wpId))){
        if(query.size() > 0){
            query.next();
            if(query.value(0).isNull()){
                m_wpPUserId = tempWpPUserId = 0;
                m_wpPUser = "";
                primaryUser->setText("");
                if(m_wpRV == 254)
                    queryText += QString("UPDATE workerplace SET RV = 0 WHERE CodWorkerPlace = %1").arg(m_wpId);
                else
                    queryText += QString("UPDATE workerplace SET RV = %1 WHERE CodWorkerPlace = %2").arg(m_wpRV+1).arg(m_wpId);
                if(!query.exec(queryText))
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось обновить версию строки:\n %1\n")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                emit wpDataChange();
            }
        }
    }else{
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось обновить данные основного пользователя:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    }
}
void AddEditWorkerPlace::on_name_textEdited(QString text)
{
    if(text != m_wpName){
        saveButton->setEnabled(true);
        cancelButton->setEnabled(true);
    }else{
        if(!formIsEdited()){
            saveButton->setEnabled(false);
            cancelButton->setEnabled(false);
        }
    }
}
void AddEditWorkerPlace::on_primaryUser_textChanged(QString text)
{
    if(text != m_wpPUser){
        saveButton->setEnabled(true);
        cancelButton->setEnabled(true);
    }else{
        if(!formIsEdited()){
            saveButton->setEnabled(false);
            cancelButton->setEnabled(false);
        }
    }
}
void AddEditWorkerPlace::on_internalNumber_textEdited(QString text)
{
    if(text != m_wpIntNum){
        saveButton->setEnabled(true);
        cancelButton->setEnabled(true);
    }else{
        if(!formIsEdited()){
            saveButton->setEnabled(false);
            cancelButton->setEnabled(false);
        }
    }
}
void AddEditWorkerPlace::on_location_textEdited(QString text)
{
    if(text != m_wpLocation){
        saveButton->setEnabled(true);
        cancelButton->setEnabled(true);
    }else{
        if(!formIsEdited()){
            saveButton->setEnabled(false);
            cancelButton->setEnabled(false);
        }
    }
}

bool AddEditWorkerPlace::formIsEdited()
{
    if(name->text() != m_wpName || tempWpPUserId != m_wpPUserId ||
       internalNumber->text() != m_wpIntNum || location->text() != m_wpLocation)
        return true;
    else
        return false;
}
void AddEditWorkerPlace::on_cancelButton_clicked()
{
    name->setText(m_wpName);
    primaryUser->setText(m_wpPUser);
    internalNumber->setText(m_wpIntNum);
    location->setText(m_wpLocation);
    saveButton->setEnabled(false);
    cancelButton->setEnabled(false);
}
void AddEditWorkerPlace::on_saveButton_clicked()
{
    QSqlQuery query;
    QString queryText;
    bool ok;
    QQueue<QVariant> bindval;
    queryText = "UPDATE workerplace SET ";
    if(name->text() != m_wpName){
        queryText += "workerplace.name = ?,";
        bindval.enqueue(name->text());
    }
    if(tempWpPUserId != m_wpPUserId){
        if(tempWpPUserId != 0){
            queryText += "PrimaryUser = ?,";
            bindval.enqueue(tempWpPUserId);
        }else
            queryText += "PrimaryUser = NULL,";
    }
    if(internalNumber->text() != m_wpIntNum){
        queryText += "InternalNumber = ?,";
        bindval.enqueue(internalNumber->text());
    }
    if(location->text() != m_wpLocation){
        queryText += "Location = ?,";
        bindval.enqueue(location->text());
    }
    if(m_wpRV == 254)
        queryText += QString("RV = 0");
    else
        queryText += QString("RV = %1").arg(m_wpRV+1);
    queryText += QString(" WHERE CodWorkerPlace = %1").arg(m_wpId);
    query.prepare(queryText);
    while(!bindval.empty()){
        query.addBindValue(bindval.dequeue());
    }
    ok = query.exec();
    if(ok){
        m_wpName = name->text();
        m_wpPUserId = tempWpPUserId;
        m_wpPUser = primaryUser->text();
        m_wpIntNum = internalNumber->text();
        m_wpLocation = location->text();
        if(m_wpRV == 254)
            m_wpRV = 0;
        else
            m_wpRV++;
        saveButton->setEnabled(false);
        cancelButton->setEnabled(false);
        emit wpDataChange();
    }else
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось обновить данные:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
}
void AddEditWorkerPlace::on_delPUserButton_clicked()
{
    tempWpPUserId = 0;
    primaryUser->clear();
}
void AddEditWorkerPlace::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_wpId,"CodWorkerPlace",m_tableName)){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditWorkerPlace::on_buttonHistoryUsers_clicked()
{
    QDialog *d = new QDialog(this);
    d->setWindowTitle(tr("История пользователя на рабочих местах"));
    d->setMinimumSize(200,200);
    QVBoxLayout *layout = new QVBoxLayout;
    JournalHistoryUsersOnWP *jhu = new JournalHistoryUsersOnWP(d,0,m_wpId);
    jhu->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhu,SIGNAL(closeJhuWin()),d,SLOT(reject()));
    layout->addWidget(jhu);
    d->setLayout(layout);
    d->layout()->setContentsMargins(0,0,0,0);
    jhu->show();
    d->setGeometry(QStyle::alignedRect(
                       Qt::LeftToRight,
                       Qt::AlignCenter,
                       QSize(600,300),
                       QRect(this->geometry().left(),this->geometry().top(),this->geometry().width(),this->geometry().height())
                       ));
    d->exec();
    d->adjustSize();
}
void AddEditWorkerPlace::on_buttonHistoryDevice_clicked()
{
    QDialog *d = new QDialog(this);
    d->setWindowTitle(tr("История перемещений"));
    d->setMinimumSize(200,200);
    QVBoxLayout *layout = new QVBoxLayout;
    JournalHistoryMoved *jhm = new JournalHistoryMoved(d,0,0,m_wpId);
    jhm->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhm,SIGNAL(closeJhmWin()),d,SLOT(reject()));
    layout->addWidget(jhm);
    d->setLayout(layout);
    d->layout()->setContentsMargins(0,0,0,0);
    jhm->show();
    d->setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    QSize(600,300),
                    QRect(this->geometry().left(),this->geometry().top(),this->geometry().width(),this->geometry().height())
                ));
    d->exec();
    d->adjustSize();
}
void AddEditWorkerPlace::on_closeButton_clicked()
{
    close();
}
void AddEditWorkerPlace::changeEvent(QEvent *e)
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
void AddEditWorkerPlace::closeEvent(QCloseEvent *event){
    if(!m_readOnly){
        timer->stop();
        delete timer;
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->unlockRecord(m_wpId,"CodWorkerPlace",m_tableName)){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать запись:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
    }
    event->accept();
}
