#include <QtSql>
#include <QtWidgets>
#include <QMessageBox>
#include "headers/addeditlicense.h"
#include "headers/selectworkplace.h"
#include "headers/selectpo.h"
#include "headers/edittable.h"
#include "headers/editprovider.h"
#include "headers/devicemodelcontrol.h"
#include "headers/devicemodel.h"
#include "headers/licensemodel.h"
#include "headers/lockdatabase.h"
#include "headers/selectdevice.h"

AddEditLicense::AddEditLicense(QWidget *parent, Enums::Modes mode, bool editMode, const QList<QVariant> &rec, bool readOnly) :
    QDialog(parent), m_mode(mode), m_editMode(editMode), m_rec(rec), m_readOnly(readOnly)
{
    setupUi(this);
    poId = 0;
    licModel = new LicenseModel(this);
    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());

    if (mode == Enums::Device){
        orgGroupBox->setVisible(false);
        dataTab->removeTab(3);
    }else{
        devModel = new DeviceModelControl(deviceView,deviceView,"addeditlicense",db,"",false);
        connect(devModel,SIGNAL(dataIsPopulated()),this,SLOT(updateDevice()));
    }
    populateCBox("CodTypeLicense","typelicense","",tr("<Выберите тип лицензии>"),typeLic);
    populateCBox("CodStatePO","statepo","",tr("<Выберите состояние>"),stateLic);
    populateCBox("CodProvider","provider","",tr("<Выберите поставщика>"),prov);
    populateCBox("id","departments","firm = 1",tr("<Выберите организацию>"),organization);

    if(!editMode){
        datePurchase->setDate(QDate::currentDate());
        dateEndLic->setDate(QDate::currentDate());
        if (mode != Enums::Device){
            devModel->setDevFilter(QString("%1.%2 IN (SELECT CodDevice FROM licenseanddevice WHERE CodLicense = -1)")
                                   .arg(devModel->model()->aliasModelTable())
                                   .arg(devModel->model()->colTabName.id));
        }
    }else{
        setDefaultEditData();
        organization->setEnabled(false);
        if(readOnly){
            buttonSelectPo->setEnabled(false);
            versionN->setReadOnly(true);
            invN->setReadOnly(true);
            typeLic->setEnabled(false);
            buttonEditTypeLic->setEnabled(false);
            stateLic->setEnabled(false);
            buttonEditStateLic->setEnabled(false);
            note->setEnabled(false);
            regName->setReadOnly(true);
            regKey->setReadOnly(true);
            regEmail->setReadOnly(true);
            kolLic->setEnabled(false);
            checkDateEndLic->setEnabled(false);
            dateEndLic->setEnabled(false);
            prov->setEnabled(false);
            buttonEditProv->setEnabled(false);
            checkDatePurchase->setEnabled(false);
            datePurchase->setEnabled(false);
            price->setEnabled(false);
        }else{
            if(m_rec.value(licModel->cIndex.key).toInt() != 0){
                timer = new QTimer(this);
                LockDataBase *lockedControl = new LockDataBase(this);
                if(!lockedControl->lockRecord(m_rec.value(licModel->cIndex.key).toInt(),"`"+licModel->colTabName.key+"`",licModel->nameModelTable())){
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
    connect(organization,SIGNAL(currentIndexChanged(int)),this,SLOT(organizationIndexChanged()));
}
void AddEditLicense::populateCBox(const QString &idName, const QString &tableName,
                  const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    if(!nullStr.isNull() && !nullStr.isEmpty())
        cBox->addItem(nullStr,0);
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
void AddEditLicense::setPo(const QList<QVariant> &poData)
{
    QSqlQuery query;
    bool ok;
    poId = poData.value(1).toInt();
    poIcon = poData.value(6).toString();
    namePO->setText(poData.value(0).toString());
    ok = query.exec(QString("SELECT Name FROM producer WHERE CodProducer = %1").arg(poData.value(4).toInt()));
    if(ok && query.size() > 0){
        query.next();
        nameProd->setText(query.value(0).toString());
    }
    query.clear();
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
bool AddEditLicense::dataIsEntered()
{
    if(m_mode == Enums::Device){
        if(poId != 0 && typeLic->currentIndex() > 0 && stateLic->currentIndex() > 0)
            return true;
    }else{
        if(organization->currentIndex() > 0 && poId != 0 && typeLic->currentIndex() > 0 && stateLic->currentIndex() > 0)
            return true;
    }
    return false;
}
bool AddEditLicense::formIsEmpty()
{
    if(m_mode == Enums::Standart){
        if(organization->currentIndex() <= 0 && poId == 0 && typeLic->currentIndex() <= 0 && stateLic->currentIndex() <= 0 &&
                (versionN->text().isNull() || versionN->text().isEmpty()) && (invN->text().isNull() || invN->text().isEmpty()) &&
                (note->toPlainText().isNull() || note->toPlainText().isEmpty()) && (regName->text().isNull() || regName->text().isEmpty()) &&
                (regKey->text().isNull() || regKey->text().isEmpty()) && (regEmail->text().isNull() || regEmail->text().isEmpty()) &&
                kolLic->value() == 0 && !checkDateEndLic->isChecked() && prov->currentIndex() <= 0 && !checkDatePurchase->isChecked() &&
                price->value() == 0.00 && devModel->model()->rowCount() == 0)
            return true;
    }else if(m_mode == Enums::WorkPlace){
        if(poId == 0 && typeLic->currentIndex() <= 0 && stateLic->currentIndex() <= 0 &&
                (versionN->text().isNull() || versionN->text().isEmpty()) && (invN->text().isNull() || invN->text().isEmpty()) &&
                (note->toPlainText().isNull() || note->toPlainText().isEmpty()) && (regName->text().isNull() || regName->text().isEmpty()) &&
                (regKey->text().isNull() || regKey->text().isEmpty()) && (regEmail->text().isNull() || regEmail->text().isEmpty()) &&
                kolLic->value() == 0 && !checkDateEndLic->isChecked() && prov->currentIndex() <= 0 && !checkDatePurchase->isChecked() &&
                price->value() == 0.00 && devModel->model()->rowCount() == 0)
            return true;
    }else if(m_mode == Enums::Device){
        if(poId == 0 && typeLic->currentIndex() <= 0 && stateLic->currentIndex() <= 0 &&
                (versionN->text().isNull() || versionN->text().isEmpty()) && (invN->text().isNull() || invN->text().isEmpty()) &&
                (note->toPlainText().isNull() || note->toPlainText().isEmpty()) && (regName->text().isNull() || regName->text().isEmpty()) &&
                (regKey->text().isNull() || regKey->text().isEmpty()) && (regEmail->text().isNull() || regEmail->text().isEmpty()) &&
                kolLic->value() == 0 && !checkDateEndLic->isChecked() && prov->currentIndex() <= 0 && !checkDatePurchase->isChecked() &&
                price->value() == 0.00)
            return true;
    }
    return false;
}
void AddEditLicense::clearForm()
{
    organization->setCurrentIndex(0);
    poId = 0;
    namePO->setText("");
    nameProd->setText("");
    typeLic->setCurrentIndex(0);
    stateLic->setCurrentIndex(0);
    versionN->setText("");
    invN->setText("");
    note->setText("");
    regName->setText("");
    regKey->setText("");
    regEmail->setText("");
    kolLic->setValue(0);
    checkDateEndLic->setChecked(false);
    on_checkDateEndLic_clicked(false);
    dateEndLic->setDate(QDate::currentDate());
    prov->setCurrentIndex(0);
    checkDatePurchase->setChecked(false);
    on_checkDatePurchase_clicked(false);
    datePurchase->setDate(QDate::currentDate());
    price->setValue(0.00);
    if (m_mode != Enums::Device){
        devModel->updateDevModel();
        deviceBindingChangesCache.clear();
    }
}
void AddEditLicense::setDefaultEditData()
{
    if(!m_rec.isEmpty()){
        organization->setCurrentIndex(organization->findData(m_rec.value(licModel->cIndex.codOrganization).toInt()));
        poId = m_rec.value(licModel->cIndex.codPO).toInt();
        namePO->setText(m_rec.value(licModel->cIndex.namePO).toString());
        nameProd->setText(m_rec.value(licModel->cIndex.nameProd).toString());
        typeLic->setCurrentIndex(typeLic->findData(m_rec.value(licModel->cIndex.codTypeLicense).toInt()));
        stateLic->setCurrentIndex(stateLic->findData(m_rec.value(licModel->cIndex.codStatePO).toInt()));
        if(!m_rec.value(licModel->cIndex.invN).isNull()) invN->setText(m_rec.value(licModel->cIndex.invN).toString()); else invN->setText("");
        if(!m_rec.value(licModel->cIndex.versionN).isNull()) versionN->setText(m_rec.value(licModel->cIndex.versionN).toString()); else versionN->setText("");
        if(!m_rec.value(licModel->cIndex.note).isNull()) note->setText(m_rec.value(licModel->cIndex.note).toString()); else note->setText("");
        if(!m_rec.value(licModel->cIndex.regName).isNull()) regName->setText(m_rec.value(licModel->cIndex.regName).toString()); else regName->setText("");
        if(!m_rec.value(licModel->cIndex.regKey).isNull()) regKey->setText(m_rec.value(licModel->cIndex.regKey).toString()); else regKey->setText("");
        if(!m_rec.value(licModel->cIndex.regMail).isNull()) regEmail->setText(m_rec.value(licModel->cIndex.regMail).toString()); else regEmail->setText("");
        kolLic->setValue(m_rec.value(licModel->cIndex.kolLicense).toInt());
        if(!m_rec.value(licModel->cIndex.codProvider).isNull() && m_rec.value(licModel->cIndex.codProvider).toInt() > 0)
            prov->setCurrentIndex(prov->findData(m_rec.value(licModel->cIndex.codProvider).toInt()));
        else
            prov->setCurrentIndex(0);
        if(!m_rec.value(licModel->cIndex.datePurchase).isNull()){
            checkDatePurchase->setChecked(true);
            datePurchase->setEnabled(true);
            datePurchase->setDate(m_rec.value(licModel->cIndex.datePurchase).toDate());
        }else{
            checkDatePurchase->setChecked(false);
            datePurchase->setEnabled(false);
            datePurchase->setDate(QDate::currentDate());
        }
        if(!m_rec.value(licModel->cIndex.dateEndLicense).isNull()){
            checkDateEndLic->setChecked(true);
            dateEndLic->setEnabled(true);
            dateEndLic->setDate(m_rec.value(licModel->cIndex.dateEndLicense).toDate());
        }else{
            checkDateEndLic->setChecked(false);
            dateEndLic->setEnabled(false);
            dateEndLic->setDate(QDate::currentDate());
        }
        if(!m_rec.value(licModel->cIndex.price).isNull()) price->setValue(m_rec.value(licModel->cIndex.price).toDouble()); else price->setValue(0.00);
        if (m_mode != Enums::Device){
            devModel->setDevFilter(QString("%1.%2 IN (SELECT CodDevice FROM licenseanddevice WHERE CodLicense = %3)")
                                   .arg(devModel->model()->aliasModelTable())
                                   .arg(devModel->model()->colTabName.id)
                                   .arg(m_rec.value(licModel->cIndex.key).toInt()));
            deviceBindingChangesCache.clear();
        }
        buttonSave->setEnabled(false);
        buttonRevert->setEnabled(false);
    }
}
bool AddEditLicense::dataChanged()
{
    if(m_rec.value(licModel->cIndex.codPO).toInt() != poId ||
            m_rec.value(licModel->cIndex.codOrganization) != organization->itemData(organization->currentIndex()) ||
            m_rec.value(licModel->cIndex.codTypeLicense) != typeLic->itemData(typeLic->currentIndex()) ||
            m_rec.value(licModel->cIndex.codStatePO) != stateLic->itemData(stateLic->currentIndex()) ||
            m_rec.value(licModel->cIndex.invN).toString() != invN->text() ||
            m_rec.value(licModel->cIndex.versionN).toString() != versionN->text() ||
            m_rec.value(licModel->cIndex.note).toString() != note->toPlainText() ||
            m_rec.value(licModel->cIndex.regName).toString() != regName->text() ||
            m_rec.value(licModel->cIndex.regKey).toString() != regKey->text() ||
            m_rec.value(licModel->cIndex.regMail).toString() != regEmail->text() ||
            m_rec.value(licModel->cIndex.kolLicense).toInt() != kolLic->value() ||
            m_rec.value(licModel->cIndex.codProvider) != prov->itemData(prov->currentIndex()).toInt() ||
            !compareDate(checkDatePurchase,datePurchase,m_rec.value(licModel->cIndex.datePurchase).toDate()) ||
            !compareDate(checkDateEndLic,dateEndLic,m_rec.value(licModel->cIndex.dateEndLicense).toDate()) ||
            m_rec.value(licModel->cIndex.price).toDouble() != price->value() ||
            deviceBindingChangesCache.size() > 0) //|| !compareParentDevId(m_rec.value(licModel->cIndex.codDevice))
        return true;
    else
        return false;
}
bool AddEditLicense::compareDate(QCheckBox *dateCheck, QDateEdit *dateEdit, QDate defDate)
{
    if(dateCheck->isChecked()){
        if(dateEdit->date() == defDate)
            return true;
        else
            return false;
    }else{
        if(defDate.isNull())
            return true;
        else
            return false;
    }
}
void AddEditLicense::updateDevice()
{
    if(devModel->model()->rowCount() > 0){
        devModel->setCurrentIndexFirstRow();
        delDeviceButton->setEnabled(true);
    }else{
        delDeviceButton->setEnabled(false);
    }
}
void AddEditLicense::on_buttonSelectPo_clicked()
{
    SelectPo *sp = new SelectPo(this,"",true,true);
    connect(sp,SIGNAL(itemSelected(QList<QVariant>)),this,SLOT(setPo(QList<QVariant>)));
    sp->exec();
}
void AddEditLicense::on_buttonEditTypeLic_clicked()
{
    CeditTable edittable(this,QString("typelicense"));
    edittable.setWindowTitle(tr("Редактирование типов лицензии"));
    edittable.exec();
    typeLic->clear();
    populateCBox("CodTypeLicense","typelicense","",tr("<Выберите тип лицензии>"),typeLic);
}
void AddEditLicense::on_buttonEditStateLic_clicked()
{
    CeditTable edittable(this,QString("statepo"));
    edittable.setWindowTitle(tr("Редактирование состояний лицензии"));
    edittable.exec();
    stateLic->clear();
    populateCBox("CodStatePO","statepo","",tr("<Выберите состояние>"),stateLic);
}
void AddEditLicense::on_buttonEditProv_clicked()
{
    editProvider provedit(this);
    provedit.exec();
    prov->clear();
    populateCBox("CodProvider","provider","","<Выберите поставщика>",prov);
}
void AddEditLicense::on_note_textChanged()
{
    if(note->toPlainText().count() > 255){
        QString str;
        str = note->toPlainText().mid(0,255);
        note->setText(str);
        QTextCursor cursor = note->textCursor();
        cursor.movePosition(QTextCursor::End);
        note->setTextCursor(cursor);
    }
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_typeLic_currentIndexChanged(int)
{
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_stateLic_currentIndexChanged(int)
{
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::organizationIndexChanged()
{
    devModel->updateDevModel();
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_versionN_textEdited()
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_invN_textEdited()
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_regName_textEdited()
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_regKey_textEdited()
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_regEmail_textEdited()
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_kolLic_valueChanged(int)
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_checkDateEndLic_clicked(bool checked)
{
    dateEndLic->setEnabled(checked);
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_prov_currentIndexChanged(int)
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_checkDatePurchase_clicked(bool checked)
{
    datePurchase->setEnabled(checked);
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_price_valueChanged(double)
{
    if(!m_editMode)
        buttonRevert->setEnabled(!formIsEmpty());
    else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_buttonRevert_clicked()
{
    if(!m_editMode)
        clearForm();
    else
        setDefaultEditData();
}
void AddEditLicense::on_selectDeviceButton_clicked()
{
    QString filter = "";
    QString selectedDevice;
    // Если не включен режим редактирования и не указана организация, выводим сообщение о необходимости указать оршганизацию
    if(!m_editMode && organization->currentIndex() <= 0){
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Для прикрепления устройства к лицензии, необходимо указать организацию!"),
                                 tr("Закрыть"));
        return;
    }
    // Если есть привязанные устройства, тогда необходимо исключить их из списка устройств доступных для выбора
    if(devModel->model()->rowCount() > 0){
        selectedDevice = "(";
        for(int i = 0; i<devModel->model()->rowCount();i++){
            selectedDevice += QString("%1,").arg(devModel->model()->index(i,devModel->model()->cIndex.id).data().toInt());
        }
        selectedDevice = selectedDevice.left(selectedDevice.length()-1) + ")";
        filter = QString("%1.CodOrganization = %2 AND typedevice.Type = 1 AND %1.id NOT IN %3")
                .arg(devModel->model()->aliasModelTable())
                .arg(organization->itemData(organization->currentIndex()).toInt())
                .arg(selectedDevice);
    }else{
        filter = QString("%1.CodOrganization = %2 AND typedevice.Type = 1")
                .arg(devModel->model()->aliasModelTable())
                .arg(organization->itemData(organization->currentIndex()).toInt());
    }
    // Открываем окно выбора устройств
    SelectDevice *sd = new SelectDevice(this,filter,true,false,true,true);
    connect(sd,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(setOrgTex(QList<QVariant>)));
    sd->setViewRootIsDecorated(false);
    sd->setAttribute(Qt::WA_DeleteOnClose);
    sd->exec();
}
void AddEditLicense::setOrgTex(const QList<QVariant> &dev)
{
    bool setFirstRow = devModel->model()->rowCount() <= 0;
    devModel->model()->insertRow(devModel->model()->rowCount());
    deviceView->setCurrentIndex(devModel->model()->index(devModel->model()->rowCount()-1,0));
    for(int i = 0; i<dev.count();i++){
        devModel->model()->setDataWithOutSQL(devModel->model()->index(devModel->model()->rowCount()-1,i),dev.value(i));
    }
    if(deviceBindingChangesCache.contains(dev.value(devModel->model()->cIndex.id).toInt())){
        deviceBindingChangesCache.remove(dev.value(devModel->model()->cIndex.id).toInt());
    }else{
        deviceBindingChangesCache[dev.value(devModel->model()->cIndex.id).toInt()] = "ins";
    }
    delDeviceButton->setEnabled(true);
    if(setFirstRow) devModel->setCurrentIndexFirstRow();
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_delDeviceButton_clicked()
{
    QModelIndex curDevModelIndex = devModel->realModelIndex(deviceView->currentIndex());
    int curDevId = devModel->model()->index(curDevModelIndex.row(),
                                            devModel->model()->cIndex.id,
                                            curDevModelIndex.parent()).data().toInt();
    if(deviceBindingChangesCache.contains(curDevId)){
        deviceBindingChangesCache.remove(curDevId);
    }else{
        deviceBindingChangesCache[curDevId] = "rem";
    }
    devModel->model()->removeRow(curDevModelIndex.row(),curDevModelIndex.parent());
    if(devModel->model()->rowCount() <= 0)
        delDeviceButton->setEnabled(false);
    if(!m_editMode){
        buttonSave->setEnabled(dataIsEntered());
        buttonRevert->setEnabled(!formIsEmpty());
    }else{
        if(dataChanged()){
            buttonRevert->setEnabled(true);
            buttonSave->setEnabled(dataIsEntered());
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditLicense::on_buttonSave_clicked()
{
    QSqlQuery addquery;
    QString queryStr;
    QQueue<QVariant> bindval;
    QString field;
    if(!m_editMode){
        if(m_mode == Enums::Device){
            QList<QVariant> licenseTableModelRowData = QList<QVariant>();
            licenseTableModelRowData.append(namePO->text());
            licenseTableModelRowData.append(QVariant()); //key
            licenseTableModelRowData.append(poId);
            licenseTableModelRowData.append(organization->itemData(organization->currentIndex()).toInt());
            licenseTableModelRowData.append(typeLic->itemData(typeLic->currentIndex()).toInt());
            licenseTableModelRowData.append(stateLic->itemData(stateLic->currentIndex()).toInt());
            licenseTableModelRowData.append(organization->itemText(organization->currentIndex()));
            licenseTableModelRowData.append(nameProd->text());
            licenseTableModelRowData.append(regName->text());
            licenseTableModelRowData.append(regKey->text());
            licenseTableModelRowData.append(regEmail->text());
            licenseTableModelRowData.append(kolLic->value());
            licenseTableModelRowData.append(invN->text());
            licenseTableModelRowData.append(versionN->text());
            licenseTableModelRowData.append(prov->itemData(prov->currentIndex()).toInt());
            if(checkDatePurchase->isChecked()){
                licenseTableModelRowData.append(datePurchase->date().toString(Qt::ISODate));
            }else{
                licenseTableModelRowData.append(QVariant());
            }
            if(checkDateEndLic->isChecked()){
                licenseTableModelRowData.append(dateEndLic->date().toString(Qt::ISODate));
            }else{
                licenseTableModelRowData.append(QVariant());
            }
            licenseTableModelRowData.append(price->value());
            licenseTableModelRowData.append(typeLic->itemText(typeLic->currentIndex()));
            licenseTableModelRowData.append(stateLic->itemText(stateLic->currentIndex()));
            licenseTableModelRowData.append(note->toPlainText());
            licenseTableModelRowData.append(poIcon);
            licenseTableModelRowData.append(0); // RV
            licenseTableModelRowData.append("ins"); // StatusRow
            emit licenseAdded(licenseTableModelRowData);
            clearForm();
        }else{
            QString val;
            int lastId;
            field = "("; val = "(";

            field += "CodOrganization,CodPO,CodTypeLicense,CodStatePO"; val += "?,?,?,?";
            bindval.enqueue(organization->itemData(organization->currentIndex()).toInt());
            bindval.enqueue(poId);
            bindval.enqueue(typeLic->itemData(typeLic->currentIndex()).toInt());
            bindval.enqueue(stateLic->itemData(stateLic->currentIndex()).toInt());

            if(!versionN->text().isNull() && !versionN->text().isEmpty()){
                field += ",VersionN"; val += ",?";
                bindval.enqueue(versionN->text());}
            if(!invN->text().isNull() && !invN->text().isEmpty()){
                field += ",InvN"; val += ",?";
                bindval.enqueue(invN->text());}
            if(prov->currentIndex() != 0){
                field += ",CodProvider"; val += ",?";
                bindval.enqueue(prov->itemData(prov->currentIndex()).toInt());}
            if(!regName->text().isNull() && !regName->text().isEmpty()){
                field += ",RegName"; val += ",?";
                bindval.enqueue(regName->text());}
            if(!regKey->text().isNull() && !regKey->text().isEmpty()){
                field += ",RegKey"; val += ",?";
                bindval.enqueue(regKey->text());}
            if(!regEmail->text().isNull() && !regEmail->text().isEmpty()){
                field += ",RegMail"; val += ",?";
                bindval.enqueue(regEmail->text());}
            if(kolLic->value() > 0){
                field += ",KolLicense"; val += ",?";
                bindval.enqueue(kolLic->value());}
            if(checkDatePurchase->isChecked()){
                field += ",DatePurchase"; val += ",?";
                bindval.enqueue(datePurchase->date());}
            if(checkDateEndLic->isChecked()){
                field += ",DateEndLicense"; val += ",?";
                bindval.enqueue(dateEndLic->date());}

            field += ",Price"; val += ",?";
            bindval.enqueue(price->value());

            if(!note->toPlainText().isNull() && !note->toPlainText().isEmpty()){
                field += ",Note"; val += ",?";
                bindval.enqueue(note->toPlainText());}

            field += ")"; val += ")";
            queryStr = "INSERT INTO licenses "+field+" VALUES "+val;
            addquery.prepare(queryStr);
            while(!bindval.empty()){
                addquery.addBindValue(bindval.dequeue());
            }
            addquery.exec();
            if (addquery.lastError().type() != QSqlError::NoError){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось добавить лицензию:\n %1")
                                         .arg(addquery.lastError().text()),
                                         tr("Закрыть"));
                return;
            }

            // Получаем ID новой лицензии
            lastId = addquery.lastInsertId().toInt();

            // Если выбраны устройства для привязки, сохраняем связь лицензии и устройств
            if(deviceBindingChangesCache.size() > 0){
                bool ok;
                QMapIterator<int,QString> i(deviceBindingChangesCache);
                while (i.hasNext()) {
                    i.next();
                    if(i.value().compare("ins") == 0){
                        ok = addquery.exec(QString("INSERT INTO licenseanddevice (CodDevice,CodLicense) VALUES (%1,%2)")
                                           .arg(i.key()).arg(lastId));
                        if(!ok){
                            QMessageBox::critical(this, tr("Ошибка"),
                                                     tr("Не удалось обновить привязки устройств:\n %1")
                                                     .arg(addquery.lastError().text()),
                                                     tr("Закрыть"));
                        }
                    }
                }
            }
            emit licenseAdded();
            QMessageBox::information(this, tr("Сохранение"),
                                     tr("Лицензия успешно добавленна"),
                                     tr("Закрыть"));
            clearForm();
        }
    }else{
        if(m_rec.value(licModel->cIndex.key).toInt() != 0){
            if(m_rec.value(licModel->cIndex.codPO).toInt() != poId){
                field += "CodPO = ?,";
                bindval.enqueue(poId);
            }
            if(m_rec.value(licModel->cIndex.codTypeLicense) != typeLic->itemData(typeLic->currentIndex())){
                field += "CodTypeLicense = ?,";
                bindval.enqueue(typeLic->itemData(typeLic->currentIndex()).toInt());
            }
            if(m_rec.value(licModel->cIndex.codStatePO) != stateLic->itemData(stateLic->currentIndex())){
                field += "CodStatePO = ?,";
                bindval.enqueue(stateLic->itemData(stateLic->currentIndex()).toInt());
            }
            if(m_rec.value(licModel->cIndex.invN).toString() != invN->text()){
                if(invN->text().isEmpty() || invN->text().isNull())
                    field += "InvN = NULL,";
                else{
                    field += "InvN = ?,";
                    bindval.enqueue(invN->text());
                }
            }
            if(m_rec.value(licModel->cIndex.versionN).toString() != versionN->text()){
                if(versionN->text().isEmpty() || versionN->text().isNull())
                    field += "VersionN = NULL,";
                else{
                    field += "VersionN = ?,";
                    bindval.enqueue(versionN->text());
                }
            }
            if(m_rec.value(licModel->cIndex.regName).toString() != regName->text()){
                if(regName->text().isEmpty() || regName->text().isNull())
                    field += "RegName = NULL,";
                else{
                    field += "RegName = ?,";
                    bindval.enqueue(regName->text());
                }
            }
            if(m_rec.value(licModel->cIndex.regKey).toString() != regKey->text()){
                if(regKey->text().isEmpty() || regKey->text().isNull())
                    field += "RegKey = NULL,";
                else{
                    field += "RegKey = ?,";
                    bindval.enqueue(regKey->text());
                }
            }
            if(m_rec.value(licModel->cIndex.regMail).toString() != regEmail->text()){
                if(regEmail->text().isEmpty() || regEmail->text().isNull())
                    field += "RegMail = NULL,";
                else{
                    field += "RegMail = ?,";
                    bindval.enqueue(regEmail->text());
                }
            }
            if(m_rec.value(licModel->cIndex.kolLicense).toInt() != kolLic->value()){
                field += "KolLicense = ?,";
                bindval.enqueue(kolLic->value());
            }
            if(m_rec.value(licModel->cIndex.codProvider) != prov->itemData(prov->currentIndex()).toInt()){
                if(prov->itemData(prov->currentIndex()).toInt() <= 0)
                    field += "CodProvider = NULL,";
                else{
                    field += "CodProvider = ?,";
                    bindval.enqueue(prov->itemData(prov->currentIndex()).toInt());
                }
            }
            if(!compareDate(checkDatePurchase,datePurchase,m_rec.value(licModel->cIndex.datePurchase).toDate())){
                if(checkDatePurchase->isChecked()){
                    field += "DatePurchase = ?,";
                    bindval.enqueue(datePurchase->date().toString(Qt::ISODate));
                }else
                    field += "DatePurchase = NULL,";
            }
            if(!compareDate(checkDateEndLic,dateEndLic,m_rec.value(licModel->cIndex.dateEndLicense).toDate())){
                if(checkDateEndLic->isChecked()){
                    field += "DateEndLicense = ?,";
                    bindval.enqueue(dateEndLic->date().toString(Qt::ISODate));
                }else
                    field += "DateEndLicense = NULL,";
            }
            if(m_rec.value(licModel->cIndex.price).toDouble() != price->value()){
                field += "Price = ?,";
                bindval.enqueue(price->value());
            }
            if(m_rec.value(licModel->cIndex.note).toString() != note->toPlainText()){
                if(note->toPlainText().isEmpty() || note->toPlainText().isNull())
                    field += "Note = NULL,";
                else{
                    field += "Note = ?,";
                    bindval.enqueue(note->toPlainText());
                }
            }
            field += "RV = ? WHERE `key` = ?";
            if(m_rec.value(licModel->cIndex.rv).toInt() == 254)
                bindval.enqueue(0);
            else
                bindval.enqueue(m_rec.value(licModel->cIndex.rv).toInt()+1);
            bindval.enqueue(m_rec.value(licModel->cIndex.key).toInt());
            queryStr = QString("UPDATE %1 SET %2")
                    .arg(licModel->nameModelTable())
                    .arg(field);

            addquery.prepare(queryStr);
            while(!bindval.empty()){
                addquery.addBindValue(bindval.dequeue());
            }
            addquery.exec();
            if (addquery.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось обновить данные лицензии:\n %1")
                                         .arg(addquery.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
        if(deviceBindingChangesCache.size() > 0){
            bool ok;
            QMapIterator<int,QString> i(deviceBindingChangesCache);
            while (i.hasNext()) {
                i.next();
                if(i.value().compare("ins") == 0){
                    ok = addquery.exec(QString("INSERT INTO licenseanddevice (CodDevice,CodLicense) VALUES (%1,%2)")
                                       .arg(i.key()).arg(m_rec.value(licModel->cIndex.key).toInt()));
                    if(!ok){
                        QMessageBox::critical(this, tr("Ошибка"),
                                              tr("Не удалось обновить привязки устройств:\n %1")
                                              .arg(addquery.lastError().text()),
                                              tr("Закрыть"));
                    }
                }
                if(i.value().compare("rem") == 0){
                    ok = addquery.exec(QString("DELETE FROM licenseanddevice WHERE CodDevice = %1 AND CodLicense = %2")
                                       .arg(i.key()).arg(m_rec.value(licModel->cIndex.key).toInt()));
                    if(!ok){
                        QMessageBox::critical(this, tr("Ошибка"),
                                              tr("Не удалось обновить привязки устройств:\n %1")
                                              .arg(addquery.lastError().text()),
                                              tr("Закрыть"));
                    }
                }
            }
            deviceBindingChangesCache.clear();
        }

        buttonRevert->setEnabled(false);
        buttonSave->setEnabled(false);
        m_rec.replace(licModel->cIndex.codPO,poId);
        m_rec.replace(licModel->cIndex.codTypeLicense,typeLic->itemData(typeLic->currentIndex()).toInt());
        m_rec.replace(licModel->cIndex.codStatePO,stateLic->itemData(stateLic->currentIndex()).toInt());
        m_rec.replace(licModel->cIndex.invN,invN->text());
        m_rec.replace(licModel->cIndex.versionN,versionN->text());
        m_rec.replace(licModel->cIndex.regName,regName->text());
        m_rec.replace(licModel->cIndex.regMail,regEmail->text());
        m_rec.replace(licModel->cIndex.regKey,regKey->text());
        m_rec.replace(licModel->cIndex.kolLicense,kolLic->value());
        if(prov->itemData(prov->currentIndex()).isNull() || prov->itemData(prov->currentIndex()).toInt() == 0)
            m_rec.replace(licModel->cIndex.codProvider,QVariant(QVariant::Int));
        else
            m_rec.replace(licModel->cIndex.codProvider,prov->itemData(prov->currentIndex()));
        if(checkDatePurchase->isChecked())
            m_rec.replace(licModel->cIndex.datePurchase,datePurchase->date());
        else
            m_rec.replace(licModel->cIndex.datePurchase,QVariant(QVariant::Date));
        if(checkDateEndLic->isChecked())
            m_rec.replace(licModel->cIndex.dateEndLicense,dateEndLic->date());
        else
            m_rec.replace(licModel->cIndex.dateEndLicense,QVariant(QVariant::Date));
        m_rec.replace(licModel->cIndex.price,price->value());
        m_rec.replace(licModel->cIndex.note,note->toPlainText());
        if(m_rec.value(licModel->cIndex.rv).toInt() == 254)
            m_rec.replace(licModel->cIndex.rv,0);
        else
            m_rec.replace(licModel->cIndex.rv,m_rec.value(licModel->cIndex.rv).toInt()+1);
        if(m_mode == Enums::Device){
            emit licenseDataChanged(m_rec);
        }else{
            emit licenseDataChanged();
        }
        QMessageBox::information(this, tr("Сохранение"),
                                 tr("Данные успешно сохранены!"),
                                 tr("Закрыть"));
    }
}
void AddEditLicense::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_rec.value(licModel->cIndex.key).toInt(),"`"+licModel->colTabName.key+"`",licModel->nameModelTable())){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditLicense::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            if(m_rec.value(licModel->cIndex.key).toInt() != 0){
                timer->stop();
                delete timer;
                LockDataBase *lockedControl = new LockDataBase(this);
                if(!lockedControl->unlockRecord(m_rec.value(licModel->cIndex.key).toInt(),"`"+licModel->colTabName.key+"`",licModel->nameModelTable())){
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось разблокировать запись:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                }
            }
        }
    }
    event->accept();
}
void AddEditLicense::changeEvent(QEvent *e)
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
