#include <QtGui>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include "headers/edittable.h"
#include "headers/movelicense.h"
#include "headers/devicemodel.h"
#include "headers/licensemodel.h"
#include "headers/lockdatabase.h"
#include "headers/selectdevice.h"
#include "headers/selectlicense.h"
#include "headers/addexistinguser.h"
#include "headers/devicemodelcontrol.h"

MoveLicense::MoveLicense(QWidget *parent, bool singlMode,const QList<QVariant> &licData) :
    QDialog(parent),
    m_singlMode(singlMode)
{
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    lockedControl = new LockDataBase(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateLockRecord()));
    timer->start(30000);

    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());

    devModel = new DeviceModelControl(devView,devView,db,"movelicense");

    licModel = new LicenseModel(this);
    moveLicModel = new QStandardItemModel(0,licModel->columnCount()-1,this);
    moveLicModel->setHeaderData(licModel->cIndex.namePO, Qt::Horizontal,tr("Наименование"));
    moveLicModel->setHeaderData(licModel->cIndex.nameProd, Qt::Horizontal,tr("Производитель"));
    moveLicModel->setHeaderData(licModel->cIndex.regKey, Qt::Horizontal,tr("Ключ регистрации"));
    moveLicModel->setHeaderData(licModel->cIndex.invN, Qt::Horizontal,tr("Инвентарный №"));
    moveLicModel->setHeaderData(licModel->cIndex.versionN, Qt::Horizontal,tr("№ версии"));
    moveLicModel->setHeaderData(licModel->cIndex.nameLic, Qt::Horizontal,tr("Тип лицензии"));
    moveLicModel->setHeaderData(licModel->cIndex.nameState, Qt::Horizontal,tr("Состояние"));
    moveLicModel->setHeaderData(licModel->cIndex.kolLicense, Qt::Horizontal,tr("Кол-во лицензий"));
    moveLicView->setModel(moveLicModel);
    for(int i = 1; i <= licModel->columnCount(); i++)
        if(i != licModel->cIndex.nameProd && i != licModel->cIndex.invN && i != licModel->cIndex.versionN &&
                i != licModel->cIndex.nameLic && i != licModel->cIndex.nameState && i != licModel->cIndex.regKey &&
                i != licModel->cIndex.kolLicense)
            moveLicView->setColumnHidden(i,true);

    performer->setVisibleClearButtron(false);
    performer->setEnabledRunButtron(false);

    dateMoved->setDateTime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    populateCBox("CodCause","cause","",tr("<Выберите причину>"),cause);
    populateCBox("id","departments","firm = 1",tr("<Выберите организацию>"),curOrganization);
    populateCBox("id","departments","firm = 1",tr("<Выберите организацию>"),newOrganization);
    if(singlMode){
        curOrganization->setCurrentIndex(curOrganization->findData(licData.value(licModel->cIndex.codOrganization)));
        curOrganization->setEnabled(false);
        addLicForMove(licData);
    }
}

void MoveLicense::changeEvent(QEvent *e)
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

void MoveLicense::closeEvent(QCloseEvent *event){
    on_clearLicButton_clicked();
    event->accept();
}

void MoveLicense::populateCBox(const QString &idName, const QString &tableName,
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
    cBox->setMaxCount(query.size()+1);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

bool MoveLicense::accessForMove()
{
    if(curOrganization->currentIndex() > 0 && newOrganization->currentIndex() > 0 &&
            moveLicModel->rowCount() > 0 && cause->currentIndex() > 0 &&
            !performer->data().isNull() && performer->data().toInt() > 0)
        return true;
    else
        return false;
}

void MoveLicense::on_addLicButton_clicked()
{
    QString filter, dontShowLic;
    dontShowLic = "";
    if(moveLicModel->rowCount()>0){
        dontShowLic = "AND l.key NOT IN ( ";
        if(moveLicModel->rowCount() == 1){
            dontShowLic += moveLicModel->data(moveLicModel->index(0,1)).toString();
        }else{
            dontShowLic += moveLicModel->data(moveLicModel->index(0,1)).toString();
            for(int i = 1; i<moveLicModel->rowCount();i++){
                dontShowLic += ","+moveLicModel->data(moveLicModel->index(i,1)).toString();
            }
        }
        dontShowLic += " )";
    }
    filter = QString("l.CodOrganization = %1 %2").arg(curOrganization->currentData().toInt()).arg(dontShowLic);
    QMap<int,QString> organizations;
    for(int i = 1; i<curOrganization->count();i++)
        organizations[curOrganization->itemData(i).toInt()] = curOrganization->itemText(i);
    SelectLicense *sl = new SelectLicense(this,filter,true,organizations,curOrganization->currentData().toInt());
    connect(sl,SIGNAL(selectedLicense(QList<QVariant>)),this,SLOT(addLicForMove(QList<QVariant>)));
    connect(this,SIGNAL(lockedError(QString,bool)),sl,SLOT(lockedErrorMessage(QString,bool)));
    sl->exec();
}

void MoveLicense::addLicForMove(const QList<QVariant> &lic)
{
    if(!lockedControl->recordIsLosked(lic.value(licModel->cIndex.key).toInt(),
                                      "`"+licModel->colTabName.key+"`",
                                      licModel->nameModelTable()))
    {
        if(lockedControl->lastError().type() != QSqlError::NoError){
            timer->stop();
            if(!m_singlMode)
                emit lockedError(tr("Не удалось получить информацию о блокировке:\n %1\n").arg(lockedControl->lastError().text()),false);
            else
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n").arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            return;
        }else{
            QSqlQuery query;
            bool ok;
            ok = query.exec(QString("SELECT %1, %2 FROM %3 WHERE `%4` = %5")
                            .arg(licModel->colTabName.rv)
                            .arg(licModel->colTabName.codOrganization)
                            .arg(licModel->nameModelTable())
                            .arg(licModel->colTabName.key)
                            .arg(lic.value(licModel->cIndex.key).toInt()));
            if(ok){
                query.next();
                if(lic.value(licModel->cIndex.rv).toInt() != query.value(0).toInt()){
                    if(query.value(1).toInt() != lic.value(licModel->cIndex.codOrganization).toInt()){
                        if(!m_singlMode)
                            emit lockedError(tr("Лицензия была перемещена,\n"
                                                "выберите лицензию с нового места положения."),true);
                        else
                            QMessageBox::warning(this,tr("Ошибка!!!"),
                                                 tr("Лицензия была перемещена,\n"
                                                    "обновите список лицензий и\n"
                                                    "выберите лицензию с нового места положения."),
                                                 tr("Закрыть"));
                        return;
                    }else{
                        if(!m_singlMode)
                            emit lockedError(tr("Запись была изменена, будут загружены новые данные записи.\n"
                                                "Повторите выбор записи."),true);
                        else
                            QMessageBox::warning(this,tr("Ошибка!!!"),
                                                 tr("Лицензия была изменена,\n"
                                                    "обновите список лицензий и\n"
                                                    "повторите выбор лицензии."),
                                                 tr("Закрыть"));
                        return;
                    }
                }
            }else{
                if(!m_singlMode){
                    emit lockedError(tr("Не удалось получить информацию о версии записи:\n %1\n")
                                     .arg(query.lastError().text()),true);
                    return;
                }else{
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось получить информацию о версии записи:\n %1\n")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                    return;
                }
            }

            if(!lockedControl->lockRecord(lic.value(licModel->cIndex.key).toInt(),
                                          "`"+licModel->colTabName.key+"`",
                                          licModel->nameModelTable())){
                timer->stop();
                if(!m_singlMode)
                    emit lockedError(tr("Не удалось заблокировать выбранную лицензию:\n %1\n").arg(lockedControl->lastError().text()),false);
                else
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось заблокировать выбранную лицензию:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
    }else{
        if(!m_singlMode)
            emit lockedError(tr("Лицензия заблокирована.\n"
                                "Пользователь выполнивший блокировку: %1")
                             .arg(lockedControl->recordBlockingUser()),false);
        else
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Лицензия заблокирована.\n"
                                    "Пользователь выполнивший блокировку: %1")
                                 .arg(lockedControl->recordBlockingUser(),
                                      tr("Закрыть")));
        return;
    }
    moveLicModel->insertRow(moveLicModel->rowCount());
    moveLicView->setCurrentIndex(moveLicModel->index(moveLicModel->rowCount()-1,0));
    for(int i = 0; i<lic.count();i++){
        moveLicModel->setData(moveLicModel->index(moveLicModel->rowCount()-1,i),lic.value(i));
    }
    if(moveLicModel->data(moveLicModel->index(moveLicModel->rowCount()-1,licModel->cIndex.ico)).toString().isEmpty() ||
            moveLicModel->data(moveLicModel->index(moveLicModel->rowCount()-1,licModel->cIndex.ico)).toString().isNull()){
        moveLicModel->item(moveLicModel->rowCount()-1)->setIcon(
                    QIcon(QDir::currentPath()+"/ico/typepoico/Default.png"));
    }else{
        moveLicModel->item(moveLicModel->rowCount()-1)->setIcon(
                    QIcon(QDir::currentPath()+"/ico/typepoico/"+
                         moveLicModel->data(moveLicModel->index(moveLicModel->rowCount()-1,licModel->cIndex.ico)).toString()));
    }

    if(!m_singlMode){
        delLicButton->setEnabled(true);
        clearLicButton->setEnabled(true);
        moveButton->setEnabled(accessForMove());
    }else{
        addLicButton->setEnabled(false);
        moveButton->setEnabled(accessForMove());
    }

    moveLicView->resizeColumnToContents(licModel->cIndex.namePO);
    moveLicView->resizeColumnToContents(licModel->cIndex.nameProd);
    moveLicView->resizeColumnToContents(licModel->cIndex.invN);
    moveLicView->resizeColumnToContents(licModel->cIndex.versionN);
    moveLicView->resizeColumnToContents(licModel->cIndex.nameLic);
    moveLicView->resizeColumnToContents(licModel->cIndex.nameState);
}

void MoveLicense::on_clearLicButton_clicked()
{
    if(moveLicModel->rowCount() <= 0)
        return;
    QList<int> lockedId;
    for(int i = 0;i<moveLicModel->rowCount();i++)
        lockedId<<moveLicModel->index(i,licModel->cIndex.key).data().toInt();
    moveLicModel->removeRows(0,moveLicModel->rowCount());
    delLicButton->setEnabled(false);
    clearLicButton->setEnabled(false);
    moveButton->setEnabled(accessForMove());
    if(!lockedControl->unlockListRecord(lockedId,
                                      "`"+licModel->colTabName.key+"`",
                                      licModel->nameModelTable())){
        timer->stop();
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось снять блокировку:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    }
}

void MoveLicense::on_delLicButton_clicked()
{
    int lockedId = moveLicModel->index(moveLicView->currentIndex().row(),licModel->cIndex.key).data().toInt();
    moveLicModel->removeRow(moveLicView->currentIndex().row());
    if(moveLicModel->rowCount()<=0){
        delLicButton->setEnabled(false);
        clearLicButton->setEnabled(false);
        moveButton->setEnabled(accessForMove());
    }
    if(!lockedControl->unlockRecord(lockedId,
                                    "`"+licModel->colTabName.key+"`",
                                    licModel->nameModelTable())){
        timer->stop();
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось снять блокировку:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    }
}

void MoveLicense::on_buttonEditCause_clicked()
{
    CeditTable edittable(this,QString("cause"));
    edittable.setWindowTitle(tr("Редактирование причин"));
    edittable.exec();
    cause->clear();
    populateCBox("CodCause","cause","",tr("<Выберите причину>"),cause);
}

void MoveLicense::on_performer_runButtonClicked()
{
    AddExistingUser *aeu = new AddExistingUser(this,curOrganization->currentData().toInt());
    aeu->setWindowTitle(tr("Выбор исполнителя"));
    aeu->setAddButtonName(tr("Выбрать"));
    connect(aeu,SIGNAL(userAdded(QString,int)),this,SLOT(setPerformer(QString,int)));
    aeu->exec();
}

void MoveLicense::setPerformer(const QString &performerName, int performerId)
{
    performer->setText(performerName);
    performer->setData(performerId);
    moveButton->setEnabled(accessForMove());
}

void MoveLicense::on_cause_currentIndexChanged(int)
{
    moveButton->setEnabled(accessForMove());
}

void MoveLicense::on_note_textChanged()
{
    if(note->toPlainText().count() > 255){
        QString str;
        str = note->toPlainText().mid(0,255);
        note->setText(str);
        QTextCursor cursor = note->textCursor();
        cursor.movePosition(QTextCursor::End);
        note->setTextCursor(cursor);
    }
}

void MoveLicense::on_moveButton_clicked()
{
    QSqlQuery query, addHistoryQuery;
    QString licForMove = "(";
    bool ok;
    if(moveLicModel->rowCount() == 1){
        licForMove += moveLicModel->data(moveLicModel->index(0,1)).toString();
    }else{
        licForMove += moveLicModel->data(moveLicModel->index(0,1)).toString();
        for(int i = 1; i<moveLicModel->rowCount();i++){
            licForMove += ","+moveLicModel->data(moveLicModel->index(i,1)).toString();
        }
    }
    licForMove += ")";

    ok = query.exec(QString("UPDATE licenses SET CodOrganization = %1 WHERE `key` IN %2")
                    .arg(newOrganization->currentData().toInt()).arg(licForMove));
    if(!ok){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось выполнить перемещение:\n %1").arg(query.lastError().text()),
                              tr("Закрыть"));
        return;
    }

    ok = query.exec(QString("DELETE FROM licenseanddevice WHERE CodLicense IN %1").arg(licForMove));
    if(!ok){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось очистить связи с устройствами предыдущей организации.\n"
                                 "Связи с устройствами новой организации не будут установлены.\n %1").arg(query.lastError().text()),
                              tr("Закрыть"));
    }else{
        if(devModel->model()->rowCount() > 0){
            int licId = 0;
            int devId = 0;
            for(int i = 0; i<moveLicModel->rowCount();i++){
                licId = moveLicModel->data(moveLicModel->index(i,licModel->cIndex.key)).toInt();
                for(int j = 0; j<devModel->model()->rowCount();j++){
                    devId = devModel->model()->index(j,devModel->model()->cIndex.id).data().toInt();
                    ok = query.exec(QString("INSERT INTO licenseanddevice (CodDevice,CodLicense) VALUES (%1,%2)").arg(devId).arg(licId));
                    if(!ok)
                        QMessageBox::critical(this, tr("Ошибка"),
                                              tr("Не удалось установить связь с устройством %2:\n%1")
                                              .arg(query.lastError().text())
                                              .arg(devModel->model()->index(i,devModel->model()->cIndex.networkName).data().toString() + " / " +
                                                   devModel->model()->index(i,devModel->model()->cIndex.name).data().toString()),
                                              tr("Закрыть"));
                }
            }
        }
    }
    ok = query.exec(QString("SELECT `key`, rv FROM %2 WHERE `key` IN %1")
                    .arg(licForMove).arg(licModel->nameModelTable()));
    if(!ok)
        QMessageBox::warning(this, tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версиях записей:\n%1")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    else{
        if(query.size() > 0){
            QMap<int,int> rowVersion;
            while(query.next()){
                if(query.value(1).toInt() == 254)
                    rowVersion[query.value(0).toInt()] = 0;
                else
                    rowVersion[query.value(0).toInt()] = query.value(1).toInt()+1;
            }
            QMap<int,int>::const_iterator i;
            for (i = rowVersion.constBegin(); i != rowVersion.constEnd(); ++i){
                ok = query.exec(QString("UPDATE %3 SET RV = %1 WHERE `key` = %2")
                                .arg(i.value()).arg(i.key()).arg(licModel->nameModelTable()));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка!!!"),
                                         tr("Не удалось обновить версии строк:\n%1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                }
            }
        }
    }
    for(int i = 0; i < moveLicModel->rowCount(); i++){
        addHistoryQuery.prepare("INSERT INTO historymoved (CodMovedLicense,DateMoved,OldPlace,NewPlace,"
                                "CodCause,CodPerformer,Note,TypeHistory) VALUES (?,?,?,?,?,?,?,?)");
        addHistoryQuery.addBindValue(moveLicModel->data(moveLicModel->index(i,licModel->cIndex.key)).toInt());
        addHistoryQuery.addBindValue(dateMoved->dateTime());
        addHistoryQuery.addBindValue(curOrganization->currentText());
        addHistoryQuery.addBindValue(newOrganization->currentText());
        addHistoryQuery.addBindValue(cause->itemData(cause->currentIndex()).toInt());
        addHistoryQuery.addBindValue(performer->data().toInt());
        addHistoryQuery.addBindValue(note->toPlainText());
        addHistoryQuery.addBindValue(1);
        addHistoryQuery.exec();
        if (addHistoryQuery.lastError().type() != QSqlError::NoError)
        {
            qDebug()<<addHistoryQuery.lastQuery();
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить историю перемещения:\n %1")
                                     .arg(addHistoryQuery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
    }

    if(!m_singlMode)
    {
        emit licIsMoved();
        on_clearLicButton_clicked();
        QMessageBox::information(this," ",tr("Перемещение выполненно успешно."),tr("Закрыть"));
    }else{
        emit licIsMoved();
        QList<int> lockedId;
        for(int i = 0;i<moveLicModel->rowCount();i++)
            lockedId<<moveLicModel->index(i,licModel->cIndex.key).data().toInt();
        if(!lockedControl->unlockListRecord(lockedId,
                                          "`"+licModel->colTabName.key+"`",
                                          licModel->nameModelTable())){
            timer->stop();
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось снять блокировку:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
        accept();
    }
}

void MoveLicense::updateLockRecord()
{
    if(moveLicModel->rowCount() <= 0)
        return;
    QList<int> lockedId;
    for(int i = 0;i<moveLicModel->rowCount();i++)
        lockedId<<moveLicModel->index(i,licModel->cIndex.key).data().toInt();
    if(!lockedControl->lockListRecord(lockedId,
                                      "`"+licModel->colTabName.key+"`",
                                      licModel->nameModelTable())){
        timer->stop();
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    }
}

void MoveLicense::on_newOrganization_currentIndexChanged(int index)
{
    if(index > 0){
        if(index == curOrganization->currentIndex()){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Текущее место нахождения и новое место нахождения не должны совпадать!!!"),
                                 tr("Закрыть"));
            newOrganization->setCurrentIndex(0);
            return;
        }
        selectDeviceButton->setEnabled(true);
        clearDevModel();
        moveButton->setEnabled(accessForMove());
    }else{
        selectDeviceButton->setEnabled(false);
        clearDevModel();
        moveButton->setEnabled(accessForMove());
    }
}

void MoveLicense::on_curOrganization_currentIndexChanged(int index)
{
    if(index > 0){
        if(index == newOrganization->currentIndex()){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Текущее место нахождения и новое место нахождения не должны совпадать!!!"),
                                 tr("Закрыть"));
            curOrganization->setCurrentIndex(0);
            return;
        }
        addLicButton->setEnabled(true);
        moveButton->setEnabled(accessForMove());
        performer->setEnabledRunButtron(true);
        setPerformer("",0);
        on_clearLicButton_clicked();
    }else{
        addLicButton->setEnabled(false);
        moveButton->setEnabled(accessForMove());
        performer->setEnabledRunButtron(false);
        setPerformer("",0);
        on_clearLicButton_clicked();
    }

}

void MoveLicense::on_selectDeviceButton_clicked()
{
    QString filter = "";
    QString selectedDevice;
    // Если есть привязанные устройства, тогда необходимо исключить их из списка устройств доступных для выбора
    if(devModel->model()->rowCount() > 0){
        selectedDevice = "(";
        for(int i = 0; i<devModel->model()->rowCount();i++){
            selectedDevice += QString("%1,").arg(devModel->model()->index(i,devModel->model()->cIndex.id).data().toInt());
        }
        selectedDevice = selectedDevice.left(selectedDevice.length()-1) + ")";
        filter = QString("%1.CodOrganization = %2 AND typedevice.Type = 1 AND %1.id NOT IN %3")
                .arg(devModel->model()->aliasModelTable())
                .arg(newOrganization->itemData(newOrganization->currentIndex()).toInt())
                .arg(selectedDevice);
    }else{
        filter = QString("%1.CodOrganization = %2 AND typedevice.Type = 1")
                .arg(devModel->model()->aliasModelTable())
                .arg(newOrganization->itemData(newOrganization->currentIndex()).toInt());
    }
    // Открываем окно выбора устройств
    SelectDevice *sd = new SelectDevice(this,filter,true,false,true,true);
    connect(sd,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(setOrgTex(QList<QVariant>)));
    sd->setViewRootIsDecorated(false);
    sd->setAttribute(Qt::WA_DeleteOnClose);
    sd->exec();
}

void MoveLicense::setOrgTex(const QList<QVariant> &dev)
{
    devModel->model()->insertRow(devModel->model()->rowCount());
    devView->setCurrentIndex(devModel->realViewIndex(devModel->model()->index(devModel->model()->rowCount()-1,0)));
    for(int i = 0; i<dev.count();i++){
        devModel->model()->setDataWithOutSQL(devModel->model()->index(devModel->model()->rowCount()-1,i),dev.value(i));
    }
    removeDeviceButton->setEnabled(true);
}
void MoveLicense::clearDevModel()
{
    if(devModel->model()->rowCount() > 0)
        devModel->model()->removeRows(0,devModel->model()->rowCount());
    removeDeviceButton->setEnabled(false);
}
void MoveLicense::on_removeDeviceButton_clicked()
{
    QModelIndex curDevModelIndex = devModel->realModelIndex(devView->currentIndex());
    devModel->model()->removeRow(curDevModelIndex.row(),curDevModelIndex.parent());
    if(devModel->model()->rowCount() <= 0)
        removeDeviceButton->setEnabled(false);
}
