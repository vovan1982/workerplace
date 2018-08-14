#include <QtSql>
#include <QtWidgets>
#include <QMessageBox>
#include "headers/edittable.h"
#include "headers/selectworkplace.h"
#include "headers/addeditdevice.h"
#include "headers/addedittypedevice.h"
#include "headers/editprovider.h"
#include "headers/editandselectproducer.h"
#include "headers/selectdevice.h"
#include "headers/devicemodel.h"
#include "headers/lockdatabase.h"

AddEditDevice::AddEditDevice(QWidget *parent, int type, int parent_id, int workPlaceId, const QString &workPlaceName,
                             bool compositionMode, bool editMode, const QList<QVariant> &rec, bool wpMode, int firmId, bool readOnly) :
    QDialog(parent), m_type(type), m_parent_id(parent_id), m_workPlaceId(workPlaceId), m_workPlaceName(workPlaceName),
    m_compositionMode(compositionMode), m_editMode(editMode), m_rec(rec), m_wpMode(wpMode), m_firmId(firmId), m_readOnly(readOnly)
{
    setupUi(this);
    if(type == 1){
        label_12->setVisible(false);
        orgTex->setVisible(false);
    }

    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());

    dm = new DeviceModel("addEditDevice",db,this);
    populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
    populateCBox("CodState","statedev","",tr("<Выберите состояние>"),state);
    populateCBox("id","departments","firm = 1","<Выберите фирму>",organization);
    populateCBox("CodProvider","provider","","<Выберите поставщика>",provider);
    populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
    datePurchase->setDate(QDate::currentDate());
    datePosting->setDate(QDate::currentDate());
    endGuarantee->setDate(QDate::currentDate());
    workPlace->setVisibleClearButtron(false);

    if(type == 0 && compositionMode){
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("SELECT CodOrganization, Name, LockedUp, LockedBy FROM device WHERE id = %1").arg(parent_id));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка"),
                                     tr("Не удалось получить информацию о оргтехнике:\n %1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
        }else{
            if(query.size()>0){
                query.next();
                organization->setCurrentIndex(organization->findData(query.value(0)));
                orgTex->setText(query.value(1).toString());
                m_parent_id = parent_id;
            }
        }
        workPlace->setText(workPlaceName);
        organization->setEnabled(false);
        workPlace->setEnabled(false);
        workPlace->setEnabledRunButtron(false);
        orgTex->setEnabled(false);
        orgTex->setEnabledClearButtron(false);
        orgTex->setEnabledRunButtron(false);
        groupBox_2->setVisible(false);
        m_workPlaceId = workPlaceId;
    }
    if(editMode){
        label->setVisible(false);
        workPlace->setVisible(false);
        label_8->setVisible(false);
        organization->setVisible(false);
        label_12->setVisible(false);
        orgTex->setVisible(false);
        groupBox_2->setVisible(false);
        setDefaultEditData();
        if(readOnly){
            typeDevice->setEnabled(false);
            buttonEditTypeDevice->setEnabled(false);
            name->setReadOnly(true);
            inventoryN->setReadOnly(true);
            serialN->setReadOnly(true);
            provider->setEnabled(false);
            buttonEditProvider->setEnabled(false);
            producer->setEnabled(false);
            buttonEditProducer->setEnabled(false);
            checkBoxDate1->setEnabled(false);
            datePurchase->setEnabled(false);
            checkBoxDate3->setEnabled(false);
            datePosting->setEnabled(false);
            checkBoxDate2->setEnabled(false);
            endGuarantee->setEnabled(false);
            price->setEnabled(false);
            state->setEnabled(false);
            buttonEditState->setEnabled(false);
            note->setEnabled(false);
        }else{
            timer = new QTimer(this);
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->lockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
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
    if(wpMode){
        organization->setCurrentIndex(organization->findData(firmId));
        workPlace->setText(workPlaceName);
        organization->setEnabled(false);
        workPlace->setEnabled(false);
        workPlace->setEnabledRunButtron(false);
        m_workPlaceId = workPlaceId;
        orgTex->setEnabledRunButtron(true);
    }
    AddEditDevice::adjustSize();
}
void AddEditDevice::clearForm()
{
    if((m_type == 0 && m_compositionMode) || m_wpMode){
        typeDevice->setCurrentIndex(0);
        name->setText("");
        inventoryN->setText("");
        serialN->setText("");
        provider->setCurrentIndex(0);
        producer->setCurrentIndex(0);
        datePurchase->setDate(QDate::currentDate());
        checkBoxDate1->setChecked(false);
        datePurchase->setEnabled(false);
        datePosting->setDate(QDate::currentDate());
        checkBoxDate3->setChecked(false);
        datePosting->setEnabled(false);
        endGuarantee->setDate(QDate::currentDate());
        checkBoxDate2->setChecked(false);
        endGuarantee->setEnabled(false);
        price->setValue(0.00);
        state->setCurrentIndex(0);
        note->setText("");
        buttonSave->setEnabled(false);
        buttonCancel->setEnabled(false);
        if(m_wpMode){
            m_parent_id = 0;
            orgTex->clear();
        }
    }else{
        organization->setCurrentIndex(0);
        typeDevice->setCurrentIndex(0);
        name->setText("");
        inventoryN->setText("");
        serialN->setText("");
        provider->setCurrentIndex(0);
        producer->setCurrentIndex(0);
        datePurchase->setDate(QDate::currentDate());
        checkBoxDate1->setChecked(false);
        datePurchase->setEnabled(false);
        datePosting->setDate(QDate::currentDate());
        checkBoxDate3->setChecked(false);
        datePosting->setEnabled(false);
        endGuarantee->setDate(QDate::currentDate());
        checkBoxDate2->setChecked(false);
        endGuarantee->setEnabled(false);
        price->setValue(0.00);
        state->setCurrentIndex(0);
        note->setText("");
        buttonSave->setEnabled(false);
        buttonCancel->setEnabled(false);
    }
}
bool AddEditDevice::dataEntered()
{
    if(organization->currentIndex() != 0 && !name->text().isNull() && !name->text().isEmpty() &&
       typeDevice->currentIndex() != 0 && state->currentIndex() != 0 && m_workPlaceId != 0)
        return true;
    else
        return false;
}
bool AddEditDevice::formIsEmpty()
{
    if(m_type == 0 && m_compositionMode){
        if(typeDevice->currentIndex() == 0 &&
           (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
           (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
           checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
           (note->text().isNull() || note->text().isEmpty()) && state->currentIndex() == 0)
            return true;
        else
            return false;
    }else if(m_wpMode){
        if(m_parent_id == 0 && typeDevice->currentIndex() == 0 &&
           (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
           (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
           checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
           (note->text().isNull() || note->text().isEmpty()) && state->currentIndex() == 0)
            return true;
        else
            return false;
    }else{
        if(organization->currentIndex() == 0 && typeDevice->currentIndex() == 0 &&
           (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
           (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
           checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
           (note->text().isNull() || note->text().isEmpty()) && state->currentIndex() == 0 && m_workPlaceId == 0 &&
           m_parent_id == 0)
            return true;
        else
            return false;
    }
}
bool AddEditDevice::dataChanged()
{
    if(m_rec.value(dm->cIndex.codTypeDevice) != typeDevice->itemData(typeDevice->currentIndex()) ||
       m_rec.value(dm->cIndex.name).toString() != name->text() || m_rec.value(dm->cIndex.inventoryN).toString() != inventoryN->text() ||
       m_rec.value(dm->cIndex.serialN).toString() != serialN->text() || providerIsChanged() || producerIsChanged() ||
       m_rec.value(dm->cIndex.price).toDouble() != price->value() || m_rec.value(dm->cIndex.note).toString() != note->text() ||
       m_rec.value(dm->cIndex.codState) != state->itemData(state->currentIndex()) ||
       !compareDate(checkBoxDate1,datePurchase,m_rec.value(dm->cIndex.datePurchase).toDate()) ||
       !compareDate(checkBoxDate2,endGuarantee,m_rec.value(dm->cIndex.endGuarantee).toDate()) ||
            !compareDate(checkBoxDate3,datePosting,m_rec.value(dm->cIndex.datePosting).toDate()))
        return true;
    else
        return false;
}
bool AddEditDevice::compareDate(QCheckBox *dateCheck, QDateEdit *dateEdit, QDate defDate)
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
bool AddEditDevice::providerIsChanged()
{
    if(!m_rec.value(dm->cIndex.codProvider).isNull()){
        if(m_rec.value(dm->cIndex.codProvider) != provider->itemData(provider->currentIndex()))
            return true;
        else
            return false;
    }else{
        if(provider->currentIndex() > 0)
            return true;
        else
            return false;
    }
}
bool AddEditDevice::producerIsChanged()
{
    if(!m_rec.value(dm->cIndex.codProducer).isNull()){
        if(m_rec.value(dm->cIndex.codProducer) != producer->itemData(producer->currentIndex()))
            return true;
        else
            return false;
    }else{
        if(producer->currentIndex() > 0)
            return true;
        else
            return false;
    }
}
void AddEditDevice::populateCBox(const QString &idName, const QString &tableName,
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
void AddEditDevice::setDefaultEditData()
{
    organization->setCurrentIndex(organization->findData(m_rec.value(dm->cIndex.codOrganization)));
    typeDevice->setCurrentIndex(typeDevice->findData(m_rec.value(dm->cIndex.codTypeDevice)));
    name->setText(m_rec.value(dm->cIndex.name).toString());
    inventoryN->setText(m_rec.value(dm->cIndex.inventoryN).toString());
    serialN->setText(m_rec.value(dm->cIndex.serialN).toString());
    if(!m_rec.value(dm->cIndex.codProvider).isNull())
        provider->setCurrentIndex(provider->findData(m_rec.value(dm->cIndex.codProvider)));
    else
        provider->setCurrentIndex(0);
    if(!m_rec.value(dm->cIndex.codProducer).isNull())
        producer->setCurrentIndex(producer->findData(m_rec.value(dm->cIndex.codProducer)));
    else
        producer->setCurrentIndex(0);
    if(!m_rec.value(dm->cIndex.datePurchase).isNull()){
        checkBoxDate1->setChecked(true);
        datePurchase->setEnabled(true);
        datePurchase->setDate(m_rec.value(dm->cIndex.datePurchase).toDate());
    }else{
        checkBoxDate1->setChecked(false);
        datePurchase->setEnabled(false);
    }
    if(!m_rec.value(dm->cIndex.datePosting).isNull()){
        checkBoxDate3->setChecked(true);
        datePosting->setEnabled(true);
        datePosting->setDate(m_rec.value(dm->cIndex.datePosting).toDate());
    }else{
        checkBoxDate3->setChecked(false);
        datePosting->setEnabled(false);
    }
    if(!m_rec.value(dm->cIndex.endGuarantee).isNull()){
        checkBoxDate2->setChecked(true);
        endGuarantee->setEnabled(true);
        endGuarantee->setDate(m_rec.value(dm->cIndex.endGuarantee).toDate());
    }else{
        checkBoxDate2->setChecked(false);
        endGuarantee->setEnabled(false);
    }
    price->setValue(m_rec.value(dm->cIndex.price).toDouble());
    state->setCurrentIndex(state->findData(m_rec.value(dm->cIndex.codState)));
    note->setText(m_rec.value(dm->cIndex.note).toString());
    buttonSave->setEnabled(false);
    buttonCancel->setEnabled(false);
}
void AddEditDevice::on_buttonCancel_clicked()
{
    if(!m_editMode)
        clearForm();
    else
        setDefaultEditData();
}
void AddEditDevice::on_organization_currentIndexChanged(int index)
{
    if(dataEntered())
        buttonSave->setEnabled(true);
    else
        buttonSave->setEnabled(false);

    if(!m_editMode){
        workPlace->clear();
        m_workPlaceId = 0;
        m_parent_id = 0;
        orgTex->clear();
    }
    if(formIsEmpty())
        buttonCancel->setEnabled(false);
    else
        buttonCancel->setEnabled(true);
    if(!m_wpMode)
        orgTex->setEnabledRunButtron(false);
    if(index <= 0)
        workPlace->setEnabledRunButtron(false);
    else
        workPlace->setEnabledRunButtron(true);
}
void AddEditDevice::on_workPlace_textChanged()
{
    if(dataEntered())
        buttonSave->setEnabled(true);
    else
        buttonSave->setEnabled(false);

    if(formIsEmpty())
        buttonCancel->setEnabled(false);
    else
        buttonCancel->setEnabled(true);
}
void AddEditDevice::on_name_textChanged()
{
    if(!m_editMode){
        if(dataEntered())
            buttonSave->setEnabled(true);
        else
            buttonSave->setEnabled(false);

        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_typeDevice_currentIndexChanged(int)
{
    if(!m_editMode){
        if(dataEntered())
            buttonSave->setEnabled(true);
        else
            buttonSave->setEnabled(false);

        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_inventoryN_textChanged()
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_serialN_textChanged()
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_price_valueChanged(double)
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_state_currentIndexChanged(int)
{
    if(!m_editMode){
        if(dataEntered())
            buttonSave->setEnabled(true);
        else
            buttonSave->setEnabled(false);

        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_note_textChanged()
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_provider_currentIndexChanged(int /*index*/)
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditDevice::on_producer_currentIndexChanged(int)
{
    if(!m_editMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditDevice::on_checkBoxDate1_clicked(bool check)
{
    if(!m_editMode){
        if(!check)
            datePurchase->setDate(QDate::currentDate());
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_checkBoxDate2_clicked(bool check)
{
    if(!m_editMode){
        if(!check)
            endGuarantee->setDate(QDate::currentDate());
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }

}

void AddEditDevice::on_checkBoxDate3_clicked(bool check)
{
    if(!m_editMode){
        if(!check)
            datePosting->setDate(QDate::currentDate());
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }else{
        if(dataChanged()){
            buttonCancel->setEnabled(true);
            if(dataEntered())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonCancel->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}
void AddEditDevice::on_buttonEditTypeDevice_clicked()
{
    AddEditTypeDevice aetd(this,"typedevice",m_type);
    aetd.exec();
    typeDevice->clear();
    populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
}
void AddEditDevice::on_buttonEditState_clicked()
{
    CeditTable edittable(this,QString("statedev"));
    edittable.setWindowTitle(tr("Редактирование состояний"));
    edittable.exec();
    state->clear();
    populateCBox("CodState","statedev","",tr("<Выберите состояние>"),state);
}
void AddEditDevice::on_buttonEditProvider_clicked()
{
    editProvider prov(this);
    prov.exec();
    provider->clear();
    populateCBox("CodProvider","provider","","<Выберите поставщика>",provider);
}
void AddEditDevice::on_buttonEditProducer_clicked()
{
    EditAndSelectProducer easp(this);
    easp.setVisibleSelectButton(false);
    easp.exec();
    producer->clear();
    populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
}
void AddEditDevice::on_orgTex_clearButtonClicked()
{
    m_parent_id = 0;
    if(formIsEmpty())
        buttonCancel->setEnabled(false);
    else
        buttonCancel->setEnabled(true);
}
void AddEditDevice::setWorkPlase(int wpId, const QString &wpName)
{
    if(m_workPlaceId != wpId){
        m_parent_id = 0;
        orgTex->clear();
    }
    orgTex->setEnabledRunButtron(true);
    m_workPlaceId = wpId;
    workPlace->setText(wpName);
}
void AddEditDevice::setOrgTex(const QList<QVariant> &orgTexData)
{
    m_parent_id = orgTexData.value(dm->cIndex.id).toInt();
    orgTex->setText(orgTexData.value(dm->cIndex.name).toString());
    if(m_wpMode){
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }
}
void AddEditDevice::on_workPlace_runButtonClicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this, organization->itemData(organization->currentIndex()).toInt());
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(setWorkPlase(int,QString)));
    swp->exec();
}
void AddEditDevice::on_orgTex_runButtonClicked()
{
    QString filter = QString("%2.%3 IN (SELECT %3 FROM %4 WHERE %5 = %1)"
                         " AND %2.%6 = 1").arg(m_workPlaceId).arg(dm->aliasModelTable()).arg(dm->colTabName.id).arg(dm->nameModelTable())
            .arg(dm->colTabName.codWorkerPlace).arg(dm->colTabName.type);
    SelectDevice *sd = new SelectDevice(this,filter,false,false,true,true);
    connect(sd,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(setOrgTex(QList<QVariant>)));
    sd->exec();
}
void AddEditDevice::on_buttonSave_clicked()
{
    QSqlQuery addquery;
    QQueue<QVariant> bindval;
    QString field, val, queryStr;
    int lastId;
    if(!m_editMode){
        field = "("; val = "(";

        field += "parent_id,CodOrganization,CodWorkerPlace,CodTypeDevice,Name,CodState,Type"; val += "?,?,?,?,?,?,?";
        bindval.enqueue(m_parent_id);
        bindval.enqueue(organization->itemData(organization->currentIndex()).toInt());
        bindval.enqueue(m_workPlaceId);
        bindval.enqueue(typeDevice->itemData(typeDevice->currentIndex()).toInt());
        bindval.enqueue(name->text());
        bindval.enqueue(state->itemData(state->currentIndex()).toInt());
        bindval.enqueue(m_type);

        if(!inventoryN->text().isNull() && !inventoryN->text().isEmpty()){
            field += ",InventoryN"; val += ",?";
            bindval.enqueue(inventoryN->text());}
        if(!serialN->text().isNull() && !serialN->text().isEmpty()){
            field += ",SerialN"; val += ",?";
            bindval.enqueue(serialN->text());}
        if(provider->currentIndex() != 0){
            field += ",CodProvider"; val += ",?";
            bindval.enqueue(provider->itemData(provider->currentIndex()).toInt());}
        if(producer->currentIndex() != 0){
            field += ",CodProducer"; val += ",?";
            bindval.enqueue(producer->itemData(producer->currentIndex()).toInt());}
        if(checkBoxDate1->isChecked()){
            field += ",DatePurchase"; val += ",?";
            bindval.enqueue(datePurchase->date());}
        if(checkBoxDate3->isChecked()){
            field += ",DatePosting"; val += ",?";
            bindval.enqueue(datePosting->date());}
        if(checkBoxDate2->isChecked()){
            field += ",EndGuarantee"; val += ",?";
            bindval.enqueue(endGuarantee->date());}
        if(!note->text().isNull() && !note->text().isEmpty()){
            field += ",Note"; val += ",?";
            bindval.enqueue(note->text());}
        field += ",Price"; val += ",?";
        bindval.enqueue(price->value());

        field += ")"; val += ")";
        queryStr = "INSERT INTO "+dm->nameModelTable()+" "+field+" VALUES "+val;
        addquery.prepare(queryStr);
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось добавить устройство:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }

        lastId = addquery.lastInsertId().toInt();

        if(m_type == 0 && m_compositionMode)
            emit deviceAdded(lastId);
        else if(m_wpMode)
            emit deviceAdded(lastId,m_parent_id);
        else
            emit deviceAdded();
        clearForm();
    }else{
        field = "CodTypeDevice = ?,";
        field += "Name = ?,";
        bindval.enqueue(typeDevice->itemData(typeDevice->currentIndex()).toInt());
        bindval.enqueue(name->text());
        if(!inventoryN->text().isNull() && !inventoryN->text().isEmpty()){
            field += "InventoryN = ?, ";
            bindval.enqueue(inventoryN->text());
        }else
            field += "InventoryN = NULL, ";
        if(!serialN->text().isNull() && !serialN->text().isEmpty()){
            field += "SerialN = ?, ";
            bindval.enqueue(serialN->text());
        }else
            field += "SerialN = NULL, ";
        if(provider->currentIndex() > 0){
            field += "CodProvider = ?, ";
            bindval.enqueue(provider->itemData(provider->currentIndex()).toInt());
        }else
            field += "CodProvider = NULL, ";
        if(producer->currentIndex() > 0){
            field += "CodProducer = ?, ";
            bindval.enqueue(producer->itemData(producer->currentIndex()).toInt());
        }else
            field += "CodProducer = NULL, ";
        if(checkBoxDate1->isChecked() && checkBoxDate2->isChecked() && checkBoxDate3->isChecked()){
            field += "DatePurchase = ?, DatePosting = ?, EndGuarantee = ?, ";
            bindval.enqueue(datePurchase->date().toString(Qt::ISODate));
            bindval.enqueue(datePosting->date().toString(Qt::ISODate));
            bindval.enqueue(endGuarantee->date().toString(Qt::ISODate));
        }else if (!checkBoxDate1->isChecked() && checkBoxDate2->isChecked() && checkBoxDate3->isChecked()){
            field += "DatePurchase = NULL, DatePosting = ?, EndGuarantee = ?, ";
            bindval.enqueue(datePosting->date().toString(Qt::ISODate));
            bindval.enqueue(endGuarantee->date().toString(Qt::ISODate));
        }else if (checkBoxDate1->isChecked() && !checkBoxDate2->isChecked() && checkBoxDate3->isChecked()){
            field += "DatePurchase = ?, DatePosting = ?, EndGuarantee = NULL, ";
            bindval.enqueue(datePurchase->date().toString(Qt::ISODate));
            bindval.enqueue(datePosting->date().toString(Qt::ISODate));
        }else if (checkBoxDate1->isChecked() && checkBoxDate2->isChecked() && !checkBoxDate3->isChecked()){
            field += "DatePurchase = ?, DatePosting = NULL, EndGuarantee = ?, ";
            bindval.enqueue(datePurchase->date().toString(Qt::ISODate));
            bindval.enqueue(endGuarantee->date().toString(Qt::ISODate));
        }else if (!checkBoxDate1->isChecked() && !checkBoxDate2->isChecked() && checkBoxDate3->isChecked()){
            field += "DatePurchase = NULL, DatePosting = ?, EndGuarantee = NULL, ";
            bindval.enqueue(datePosting->date().toString(Qt::ISODate));
        }else if (checkBoxDate1->isChecked() && !checkBoxDate2->isChecked() && !checkBoxDate3->isChecked()){
            field += "DatePurchase = ?, DatePosting = NULL, EndGuarantee = NULL, ";
            bindval.enqueue(datePurchase->date().toString(Qt::ISODate));
        }else if (!checkBoxDate1->isChecked() && checkBoxDate2->isChecked() && !checkBoxDate3->isChecked()){
            field += "DatePurchase = NULL, DatePosting = NULL, EndGuarantee = '%1', ";
            bindval.enqueue(endGuarantee->date().toString(Qt::ISODate));
        }else if (!checkBoxDate1->isChecked() && !checkBoxDate2->isChecked() && !checkBoxDate3->isChecked())
            field += "DatePurchase = NULL, DatePosting = NULL, EndGuarantee = NULL, ";
        field += "Price = ?, CodState = ?, Note = ?, RV = ? WHERE id = ?";
        bindval.enqueue(price->value());
        bindval.enqueue(state->itemData(state->currentIndex()).toInt());
        bindval.enqueue(note->text());
        if(m_rec.value(dm->cIndex.rv).toInt() == 254)
            bindval.enqueue(0);
        else
            bindval.enqueue(m_rec.value(dm->cIndex.rv).toInt()+1);
        bindval.enqueue(m_rec.value(dm->cIndex.id).toInt());
        queryStr = QString("UPDATE %1 SET %2")
                .arg(dm->nameModelTable())
                .arg(field);
        addquery.prepare(queryStr);
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось обновить данные устройства:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }
        buttonSave->setEnabled(false);
        buttonCancel->setEnabled(false);
        m_rec.replace(dm->cIndex.codTypeDevice,typeDevice->itemData(typeDevice->currentIndex()));
        m_rec.replace(dm->cIndex.name,name->text());
        m_rec.replace(dm->cIndex.inventoryN,inventoryN->text());
        m_rec.replace(dm->cIndex.serialN,serialN->text());
        m_rec.replace(dm->cIndex.codProvider,provider->itemData(provider->currentIndex()));
        m_rec.replace(dm->cIndex.codProducer,producer->itemData(producer->currentIndex()));
        m_rec.replace(dm->cIndex.datePurchase,datePurchase->date());
        m_rec.replace(dm->cIndex.datePosting,datePosting->date());
        m_rec.replace(dm->cIndex.endGuarantee,endGuarantee->date());
        m_rec.replace(dm->cIndex.price,price->value());
        m_rec.replace(dm->cIndex.codState,state->itemData(state->currentIndex()));
        m_rec.replace(dm->cIndex.note,note->text());
        if(m_rec.value(dm->cIndex.rv).toInt() == 254)
            m_rec.replace(dm->cIndex.rv,0);
        else
            m_rec.replace(dm->cIndex.rv,m_rec.value(dm->cIndex.rv).toInt()+1);
        emit deviceDataChanged();
        QMessageBox::information(this, tr("Сохранение"),
                                 tr("Данные успешно сохранены!"),
                                 tr("Закрыть"));
    }
}
void AddEditDevice::on_typeOrgTex_clicked(bool checked)
{
    if(checked){
        label_12->setVisible(false);
        orgTex->setVisible(false);
        clearForm();
        m_type = 1;
        populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
        AddEditDevice::adjustSize();
    }
}
void AddEditDevice::on_typeHardware_clicked(bool checked)
{
    if(checked){
        label_12->setVisible(true);
        orgTex->setVisible(true);
        clearForm();
        m_type = 0;
        populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
        AddEditDevice::adjustSize();
    }
}
void AddEditDevice::on_buttonClose_clicked()
{
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    delete dm;
    reject();
}
void AddEditDevice::updateLockRecord()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку записи:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
        timer->stop();
    }
}
void AddEditDevice::changeEvent(QEvent *e)
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
void AddEditDevice::closeEvent(QCloseEvent *event){
    if(m_editMode){
        if(!m_readOnly){
            timer->stop();
            delete timer;
            LockDataBase *lockedControl = new LockDataBase(this);
            if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
        }
    }
    event->accept();
}
