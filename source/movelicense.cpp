#include <QtGui>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include "headers/movelicense.h"
#include "headers/selectworkplace.h"
#include "headers/devicemodelcontrol.h"
#include "headers/devicemodel.h"
#include "headers/filterdevice.h"
#include "headers/selectlicense.h"
#include "headers/licensemodel.h"
#include "headers/edittable.h"
#include "headers/addexistinguser.h"
#include "headers/lockdatabase.h"

MoveLicense::MoveLicense(QWidget *parent, bool singlMode, const QList<QVariant> &curWPData,
                         const QList<QVariant> &licData, bool addWPMode, bool moveFromWp) :
    QDialog(parent),
    m_singlMode(singlMode),
    m_addWPMode(addWPMode),
    m_moveFromWp(moveFromWp)
{
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    curOrgId = 0; curFilPredId = 0; curDepId = 0; curWPWHId = 0;
    newOrgId = 0; newFilPredId = 0; newDepId = 0; newWPWHId = 0;
    orgTexFilter = "";
    curFilter = "";

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
    devModel = new DeviceModelControl(devView,devView,"movelicense",db,"",false);
    connect(devModel,SIGNAL(dataIsPopulated()),this,SLOT(deviceModelIsPopulated()));
    devModel->setHeaderData();

    licModel = new LicenseModel(this);

    moveLicModel = new QStandardItemModel(0,licModel->columnCount()-1,this);
    moveLicModel->setHeaderData(licModel->cIndex.namePO, Qt::Horizontal,tr("Наименование"));
    moveLicModel->setHeaderData(licModel->cIndex.nameProd, Qt::Horizontal,tr("Производитель"));
    moveLicModel->setHeaderData(licModel->cIndex.invN, Qt::Horizontal,tr("Инвентарный №"));
    moveLicModel->setHeaderData(licModel->cIndex.versionN, Qt::Horizontal,tr("№ версии"));
    moveLicModel->setHeaderData(licModel->cIndex.nameLic, Qt::Horizontal,tr("Тип лицензии"));
    moveLicModel->setHeaderData(licModel->cIndex.nameState, Qt::Horizontal,tr("Состояние"));
    moveLicView->setModel(moveLicModel);
    for(int i = 1; i <= licModel->columnCount(); i++)
        if(i != licModel->cIndex.nameProd && i != licModel->cIndex.invN && i != licModel->cIndex.versionN &&
                i != licModel->cIndex.nameLic && i != licModel->cIndex.nameState)
            moveLicView->setColumnHidden(i,true);

    performer->setVisibleClearButtron(false);
    performer->setEnabledRunButtron(false);

    if(singlMode){
        fillCurPlace(curWPData);
        addLicForMove(licData);
    }
    if(addWPMode){
        fillNewPlace(curWPData);
        selectNewButton->setEnabled(false);
        setWindowTitle(tr("Добавление лицензии"));
        groupBox->setTitle(tr("ИЗ:"));
        groupBox_2->setTitle(tr("В:"));
        moveButton->setText(tr("Добавить"));
        moveButton->setIcon(QIcon(":/16x16/apply/ico/apply_16x16.png"));
    }
    if(moveFromWp){
        fillCurPlace(curWPData);
        selectCurButton->setEnabled(false);
    }
    dateMoved->setDateTime(QDateTime(QDate::currentDate(),QTime::currentTime()));
    populateCBox("CodCause","cause","",tr("<Выберите причину>"),cause);
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

void MoveLicense::on_selectCurButton_clicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this, newOrgId, true, true, newWPWHId);
    connect(swp,SIGNAL(addWorkPlace(QList<QVariant>)),this,SLOT(fillCurPlace(QList<QVariant>)));
    swp->setAttribute(Qt::WA_DeleteOnClose);
    swp->exec();
}

void MoveLicense::fillCurPlace(QList<QVariant> curPlaceData)
{
    curOrgId = curPlaceData.value(curPlaceData.indexOf("org") + 2).toInt();
    curFirm->setText(curPlaceData.value(curPlaceData.indexOf("org") + 1).toString());
    curFilPredId = curPlaceData.value(curPlaceData.indexOf("fp") + 2).toInt();
    curFP->setText(curPlaceData.value(curPlaceData.indexOf("fp") + 1).toString());
    curDepId = curPlaceData.value(curPlaceData.indexOf("dep") + 2).toInt();
    curDep->setText(curPlaceData.value(curPlaceData.indexOf("dep") + 1).toString());
    curWPWHId = curPlaceData.value(curPlaceData.indexOf("wpwh") + 2).toInt();
    curWpWh->setText(curPlaceData.value(curPlaceData.indexOf("wpwh") + 1).toString());
    addLicButton->setEnabled(true);
    moveButton->setEnabled(accessForMove());
    performer->setEnabledRunButtron(true);
}

void MoveLicense::on_selectNewButton_clicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this, curOrgId, true, true, curWPWHId);
    connect(swp,SIGNAL(addWorkPlace(QList<QVariant>)),this,SLOT(fillNewPlace(QList<QVariant>)));
    swp->setAttribute(Qt::WA_DeleteOnClose);
    swp->exec();
}

void MoveLicense::fillNewPlace(QList<QVariant> newPlaceData)
{
    newOrgId = newPlaceData.value(newPlaceData.indexOf("org") + 2).toInt();
    newFirm->setText(newPlaceData.value(newPlaceData.indexOf("org") + 1).toString());
    newFilPredId = newPlaceData.value(newPlaceData.indexOf("fp") + 2).toInt();
    newFP->setText(newPlaceData.value(newPlaceData.indexOf("fp") + 1).toString());
    newDepId = newPlaceData.value(newPlaceData.indexOf("dep") + 2).toInt();
    newDep->setText(newPlaceData.value(newPlaceData.indexOf("dep") + 1).toString());
    newWPWHId = newPlaceData.value(newPlaceData.indexOf("wpwh") + 2).toInt();
    newWpWh->setText(newPlaceData.value(newPlaceData.indexOf("wpwh") + 1).toString());

    curFilter = QString("%2.id IN (SELECT id FROM %3 WHERE CodWorkerPlace = %1)")
            .arg(newWPWHId)
            .arg(devModel->model()->aliasModelTable())
            .arg(devModel->model()->nameModelTable());
    orgTexFilter = "";
    filterIsSet = false;
    clearFilterButton->setEnabled(false);
    devModel->setDevFilter(curFilter);
    moveButton->setEnabled(accessForMove());
}

void MoveLicense::on_groupBoxDev_clicked(bool checked)
{
    if(!checked){
        devView->setCurrentIndex(QModelIndex());
    }else
        devModel->setCurrentIndexFirstRow();
    devView->setEnabled(checked);
}

void MoveLicense::on_addFiltrButton_clicked()
{
    FilterDevice *f = new FilterDevice(this,true);
    connect(f,SIGNAL(setFilter(QString)),this,SLOT(setOrgTexFilter(QString)));
    f->exec();
}

void MoveLicense::setOrgTexFilter(const QString &filters)
{
    orgTexFilter = filters;
    if(!curFilter.isEmpty() && !orgTexFilter.isEmpty())
        devModel->setDevFilter(curFilter+" AND "+orgTexFilter);
    if(!curFilter.isEmpty() && orgTexFilter.isEmpty())
        devModel->setDevFilter(curFilter);
    if(curFilter.isEmpty() && !orgTexFilter.isEmpty())
        devModel->setDevFilter(orgTexFilter);
    if(filters.isNull() || filters.isEmpty())
        filterIsSet = false;
    else
        filterIsSet = true;
    if(devModel->model()->rowCount() > 0){
        devModel->setCurrentIndexFirstRow();
        moveButton->setEnabled(true);
    }else
        moveButton->setEnabled(false);
    clearFilterButton->setEnabled(filterIsSet);
}

void MoveLicense::on_clearFilterButton_clicked()
{
    setOrgTexFilter("");
}

bool MoveLicense::accessForMove()
{
    if(curOrgId > 0 && newOrgId > 0 && moveLicModel->rowCount() > 0 && cause->currentIndex() > 0 && !performer->data().isNull() && performer->data().toInt() > 0)
        if(groupBoxDev->isChecked()){
            if(devModel->model()->rowCount() > 0)
                return true;
            else
                return false;
        }else
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
    filter = QString("l.CodWorkerPlace = %1 %2").arg(curWPWHId).arg(dontShowLic);
    SelectLicense *sl = new SelectLicense(this,filter,true,true,curWPWHId);
    connect(sl,SIGNAL(selectedLicense(QList<QVariant>)),this,SLOT(addLicForMove(QList<QVariant>)));
    connect(this,SIGNAL(lockedError(QString,bool)),sl,SLOT(lockedErrorMessage(QString,bool)));
    sl->exec();
}

void MoveLicense::addLicForMove(const QList<QVariant> &lic)
{
    if(!lockedControl->recordIsLosked(lic.value(licModel->cIndex.key).toInt(),
                                      "`"+licModel->colTabName.key+"`",
                                      licModel->nameModelTable())){
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
                            .arg(licModel->colTabName.codWorkerPlace)
                            .arg(licModel->nameModelTable())
                            .arg(licModel->colTabName.key)
                            .arg(lic.value(licModel->cIndex.key).toInt()));
            if(ok){
                query.next();
                if(lic.value(licModel->cIndex.rv).toInt() != query.value(0).toInt()){
                    if(query.value(1).toInt() != lic.value(licModel->cIndex.codWorkerPlace).toInt()){
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
                }else
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось получить информацию о версии записи:\n %1\n")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
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
        selectCurButton->setEnabled(false);
    }else{
        addLicButton->setEnabled(false);
        moveButton->setEnabled(accessForMove());
        selectCurButton->setEnabled(false);
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
    if(!m_moveFromWp)
        selectCurButton->setEnabled(true);
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
        if(!m_moveFromWp)
            selectCurButton->setEnabled(true);
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
    AddExistingUser *aeu = new AddExistingUser(this,curOrgId);
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

    ok = query.exec(QString("UPDATE licenses SET CodWorkerPlace = %1 WHERE `key` IN %2")
                    .arg(newWPWHId).arg(licForMove));
    if(!ok){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не удалось выполнить перемещение:\n %1").arg(query.lastError().text()),
                              tr("Закрыть"));
        return;
    }
    if(groupBoxDev->isChecked()){
        ok = query.exec(QString("UPDATE licenses SET CodDevice = %1 WHERE `key` IN %2")
                        .arg(devModel->model()->index(devView->currentIndex().row(),
                                                         devModel->model()->cIndex.id,
                                                         devView->currentIndex().parent()).data().toInt())
                        .arg(licForMove));
        if(!ok){
            QMessageBox::critical(this, tr("Ошибка"),
                                  tr("Не удалось установить привязку к устройству:\n %1").arg(query.lastError().text()),
                                  tr("Закрыть"));
            return;
        }
    }else{
        ok = query.exec(QString("UPDATE licenses SET CodDevice = NULL WHERE `key` IN %1").arg(licForMove));
        if(!ok){
            QMessageBox::critical(this, tr("Ошибка"),
                                  tr("Не удалось удалить привязку к устройству:\n %1").arg(query.lastError().text()),
                                  tr("Закрыть"));
            return;
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

    QString oldPlace, newPlace;
    oldPlace = curFirm->text();
    newPlace = newFirm->text();
    if (!curFP->text().isNull() && !curFP->text().isEmpty())
        oldPlace += "/"+curFP->text();
    if (!newFP->text().isNull() && !newFP->text().isEmpty())
        newPlace += "/"+newFP->text();
    if (!curDep->text().isNull() && !curDep->text().isEmpty())
        oldPlace += "/"+curDep->text();
    if (!newDep->text().isNull() && !newDep->text().isEmpty())
        newPlace += "/"+newDep->text();
    oldPlace += "/"+curWpWh->text();
    newPlace += "/"+newWpWh->text();

    for(int i = 0; i < moveLicModel->rowCount(); i++){
        addHistoryQuery.prepare("INSERT INTO historymoved (CodMovedLicense,DateMoved,OldPlace,NewPlace,"
                                "CodCause,CodPerformer,Note,TypeHistory,CodOldPlace,CodNewPlace) VALUES (?,?,?,?,?,?,?,?,?,?)");
        addHistoryQuery.addBindValue(moveLicModel->data(moveLicModel->index(i,licModel->cIndex.key)).toInt());
        addHistoryQuery.addBindValue(dateMoved->dateTime());
        addHistoryQuery.addBindValue(oldPlace);
        addHistoryQuery.addBindValue(newPlace);
        addHistoryQuery.addBindValue(cause->itemData(cause->currentIndex()).toInt());
        addHistoryQuery.addBindValue(performer->data().toInt());
        addHistoryQuery.addBindValue(note->toPlainText());
        addHistoryQuery.addBindValue(1);
        addHistoryQuery.addBindValue(curWPWHId);
        addHistoryQuery.addBindValue(newWPWHId);
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

    if(!m_singlMode && !m_addWPMode && !m_moveFromWp){
        emit licIsMoved();
        on_clearLicButton_clicked();
        QMessageBox::information(this," ",tr("Перемещение выполненно успешно."),tr("Закрыть"));
    }else{
        emit licIsMoved();
        accept();
    }
}

void MoveLicense::deviceModelIsPopulated()
{
    if(devModel->model()->rowCount()>0){
        groupBoxDev->setChecked(false);
        groupBoxDev->setEnabled(true);
        on_groupBoxDev_clicked(false);
    }else{
        groupBoxDev->setChecked(false);
        groupBoxDev->setEnabled(false);
        on_groupBoxDev_clicked(false);
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
