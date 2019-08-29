#include <QtSql>
#include <QtWidgets>
#include <QMessageBox>
#include "headers/edittable.h"
#include "headers/devicemodel.h"
#include "headers/editprovider.h"
#include "headers/selectdevice.h"
#include "headers/lockdatabase.h"
#include "headers/selectlicense.h"
#include "headers/addeditdevice.h"
#include "headers/addeditlicense.h"
#include "headers/selectworkplace.h"
#include "headers/licensetablemodel.h"
#include "headers/addedittypedevice.h"
#include "headers/editandselectproducer.h"
#include "headers/addeditnetworkinterface.h"

AddEditDevice::AddEditDevice(QWidget *parent, int type, int parent_id, Enums::FormModes formMode,
              const QList<QVariant> &rec, int workPlaceId, const QString &workPlaceName, Enums::Modes mode, int firmId) :
    QDialog(parent), m_type(type), m_parent_id(parent_id), m_formMode(formMode),m_rec(rec),
    m_workPlaceId(workPlaceId), m_workPlaceName(workPlaceName), m_mode(mode)
{
    setupUi(this);

    /*Установка значений поумолчанию, инициализация моделей, заполнение комбобоксов*/
    {
        interfaceIsChanged = false;
        licenseIsChanged = false;
        licenseIdToUntie = QList<QVariant>();
        datePurchase->setDate(QDate::currentDate());
        datePosting->setDate(QDate::currentDate());
        endGuarantee->setDate(QDate::currentDate());
        workPlace->setVisibleClearButtron(false);

        db = QSqlDatabase::database("qt_sql_default_connection");
        db.setHostName(QSqlDatabase::database().hostName());
        db.setDatabaseName(QSqlDatabase::database().databaseName());
        db.setPort(QSqlDatabase::database().port());
        db.setUserName(QSqlDatabase::database().userName());
        db.setPassword(QSqlDatabase::database().password());

        dm = new DeviceModel("addEditDevice",db,this);

        interfaceModel = new QSqlTableModel(this,db);
        interfaceModel->setTable("networkinterface");
        interfaceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        interfaceModel->setFilter(QString("CodDevice = -1"));

        lm = new LicenseTableModel(this,db);
        lm->setFilter(QString("`key` IN (SELECT CodLicense FROM licenseanddevice WHERE CodDevice = -1)"));

        populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
        populateCBox("CodState","statedev","",tr("<Выберите состояние>"),state);
        populateCBox("id","departments","firm = 1","<Выберите фирму>",organization);
        populateCBox("CodProvider","provider","","<Выберите поставщика>",provider);
        populateCBox("CodProducer","producer","","<Выберите производителя>",producer);
        populateCBox("CodDomainWg","domainwg","","<Выберите Домен/Рабочую группу>",domainWg);
    }
    // Если тип устройства оргтехника, скрыть поле выбора родительской оргтехники, иначе удалить вкладку сеть
    if(type == 1){
        label_12->setVisible(false);
        orgTex->setVisible(false);
    }else{
        typeHardware->setChecked(true);
        on_typeHardware_clicked(true);
    }
    // Если тип устройства комплектующее и нахождится в составе устройства
    if(type == 0 && parent_id > 0){
        QSqlQuery query;
        bool ok;
        if(formMode != Enums::FormModes::Copy){
            /*Получаем и устанавливаем код организации и наименование родительского устройства*/
            {
                ok = query.exec(QString("SELECT CodOrganization, Name FROM device WHERE id = %1").arg(parent_id));
                if(!ok){
                    QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось получить информацию о родительской оргтехнике:\n %1")
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
            }
            workPlace->setText(workPlaceName);
            m_workPlaceId = workPlaceId;
            organization->setEnabled(false);
            workPlace->setEnabled(false);
            workPlace->setEnabledRunButtron(false);
            orgTex->setEnabled(false);
            orgTex->setEnabledClearButtron(false);
            orgTex->setEnabledRunButtron(false);
            groupBox_2->setVisible(false);
        }
    }
    // Если активирован режим рабочего места
    if(mode == Enums::WorkPlace){
        organization->setCurrentIndex(organization->findData(firmId));
        workPlace->setText(workPlaceName);
        m_workPlaceId = workPlaceId;
        organization->setEnabled(false);
        workPlace->setEnabled(false);
        workPlace->setEnabledRunButtron(false);
        orgTex->setEnabledRunButtron(true);
    }
    // Если активирован режим редактирования
    if(formMode == Enums::FormModes::Edit){
        label->setVisible(false);
        workPlace->setVisible(false);
        label_8->setVisible(false);
        organization->setVisible(false);
        label_12->setVisible(false);
        orgTex->setVisible(false);
        groupBox_2->setVisible(false);
        doNotClearForm->setVisible(false);
        this->setWindowTitle(tr("Редактирование устроства"));
        setDefaultEditData();

        // Устанавливаем фильтры для моделей интерфейсвов и лицензий на основании выбранного устройства
        interfaceModel->setFilter(QString("CodDevice = %1").arg(m_rec.value(dm->cIndex.id).toInt()));
        lm->setFilter(QString("`key` IN (SELECT CodLicense FROM licenseanddevice WHERE CodDevice = %1)").arg(m_rec.value(dm->cIndex.id).toInt()));

        // Запускаем блокировку записи устройства в БД
        lockedControl = new LockDataBase(this);
        lockedControl->lockRecordThread(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable());
    }
    // Если активирован режим только чтение
    if(formMode == Enums::FormModes::Read){
        label->setVisible(false);
        workPlace->setVisible(false);
        label_8->setVisible(false);
        organization->setVisible(false);
        label_12->setVisible(false);
        orgTex->setVisible(false);
        groupBox_2->setVisible(false);
        doNotClearForm->setVisible(false);
        this->setWindowTitle(tr("Только чтение"));
        setDefaultEditData();

        // Устанавливаем фильтры для моделей интерфейсвов и лицензий на основании выбранного устройства
        interfaceModel->setFilter(QString("CodDevice = %1").arg(m_rec.value(dm->cIndex.id).toInt()));
        lm->setFilter(QString("`key` IN (SELECT CodLicense FROM licenseanddevice WHERE CodDevice = %1)").arg(m_rec.value(dm->cIndex.id).toInt()));

        // Переводим форму в режим чтения
        typeDevice->setEnabled(false);
        buttonEditTypeDevice->setEnabled(false);
        name->setReadOnly(true);
        networkName->setReadOnly(true);
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
        domainWg->setEnabled(false);
        buttonEditState->setEnabled(false);
        note->setEnabled(false);
        detailDescription->setReadOnly(true);
        buttonAddInterface->setEnabled(false);
        actionAddInterface->setEnabled(false);
        actionAddNewLicense->setEnabled(false);
        addNewLicenseButton->setEnabled(false);
        actionSelectlicense->setEnabled(false);
        selectLicenseButton->setEnabled(false);
        actionDelLicense->setEnabled(false);
        deleteLicenseButton->setEnabled(false);

    }
    // Если активирован режим создания копии устройства
    if(formMode == Enums::FormModes::Copy){
        setDefaultEditData();
        workPlace->setText(workPlaceName);
        m_workPlaceId = workPlaceId;
        if(type == 0) orgTex->setEnabledRunButtron(true);
        if(parent_id > 0){
            QSqlQuery query;
            bool ok;
            //Получаем и устанавливаем наименование родительского устройства
            ok = query.exec(QString("SELECT Name FROM device WHERE id = %1").arg(parent_id));
            if(!ok){
                QMessageBox::warning(this, tr("Ошибка"),
                                     tr("Не удалось получить информацию о родительской оргтехнике:\n %1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
            }else{
                if(query.size()>0){
                    query.next();
                    orgTex->setText(query.value(0).toString());
                    m_parent_id = parent_id;
                }
            }
        }
        if(dataEntered())
            buttonSave->setEnabled(true);
        else
            buttonSave->setEnabled(false);
        if(formIsEmpty())
            buttonCancel->setEnabled(false);
        else
            buttonCancel->setEnabled(true);
    }
    /*Заполнение и настройка модели Интерфейсы. Привязка мотели к вьюхе. Настройка режима редактирования*/
    {
        interfaceModel->select();
        interfaceModel->setHeaderData(2, Qt::Horizontal, tr("Название"));
        interfaceModel->setHeaderData(3, Qt::Horizontal, tr("IP адрес"));
        interfaceModel->setHeaderData(4, Qt::Horizontal, tr("Маска подсети"));
        interfaceModel->setHeaderData(5, Qt::Horizontal, tr("Шлюз"));
        interfaceModel->setHeaderData(6, Qt::Horizontal, tr("DNS сервера"));
        interfaceModel->setHeaderData(7, Qt::Horizontal, tr("WINS сервера"));
        interfaceModel->setHeaderData(8, Qt::Horizontal, tr("Примечание"));

        interfaceView->setModel(interfaceModel);
        interfaceView->setColumnHidden(0,true);
        interfaceView->setColumnHidden(1,true);
        interfaceView->setColumnHidden(9,true);
        interfaceView->setColumnHidden(10,true);
        interfaceView->setColumnHidden(11,true);

        if(interfaceModel->rowCount() > 0){
            interfaceView->setCurrentIndex(interfaceModel->index(0,0));
            if(formMode != Enums::FormModes::Read){
                buttonChangeInterface->setEnabled(true);
                buttonDelInterface->setEnabled(true);
                actionChangeInterface->setEnabled(true);
                actionDelInterface->setEnabled(true);
            }else{
                buttonAddInterface->setEnabled(false);
                buttonChangeInterface->setEnabled(false);
                buttonDelInterface->setEnabled(false);
                actionAddInterface->setEnabled(false);
                actionChangeInterface->setEnabled(false);
                actionDelInterface->setEnabled(false);
            }
        }

        connect(interfaceView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_buttonChangeInterface_clicked()));
        connect(interfaceView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onInterfaceMenu(const QPoint &)));
    }
    /*Заполнение и настройка модели Лицензии. Привязка мотели к вьюхе. Настройка режима редактирования*/
    {
        lm->select();
        lm->setHeaderData(lm->cIndex.namePO, Qt::Horizontal,tr("Наименование"));
        lm->setHeaderData(lm->cIndex.nameProd, Qt::Horizontal,tr("Производитель"));
        lm->setHeaderData(lm->cIndex.invN, Qt::Horizontal,tr("Инвентарный №"));
        lm->setHeaderData(lm->cIndex.versionN, Qt::Horizontal,tr("№ версии"));
        lm->setHeaderData(lm->cIndex.nameLic, Qt::Horizontal,tr("Тип лицензии"));
        lm->setHeaderData(lm->cIndex.nameState, Qt::Horizontal,tr("Состояние"));
        lm->setHeaderData(lm->cIndex.regKey, Qt::Horizontal,tr("Ключ регистрации"));

        licenseView->setModel(lm);
        for(int i = 1; i <= lm->columnCount(); i++)
            if(i != lm->cIndex.nameProd && i != lm->cIndex.invN && i != lm->cIndex.versionN &&
                    i != lm->cIndex.nameLic && i != lm->cIndex.nameState && i != lm->cIndex.regKey)
                licenseView->setColumnHidden(i,true);
        licenseView->resizeColumnToContents(lm->cIndex.namePO);
        licenseView->resizeColumnToContents(lm->cIndex.nameProd);
        licenseView->resizeColumnToContents(lm->cIndex.invN);
        licenseView->resizeColumnToContents(lm->cIndex.versionN);
        licenseView->resizeColumnToContents(lm->cIndex.nameLic);
        licenseView->resizeColumnToContents(lm->cIndex.nameState);
        licenseView->resizeColumnToContents(lm->cIndex.regKey);

        if(lm->rowCount() > 0){
            licenseView->setCurrentIndex(lm->index(0,0));
            if(formMode != Enums::FormModes::Read){
                changeLicenseButton->setEnabled(true);
                deleteLicenseButton->setEnabled(true);
                actionEditLicense->setEnabled(true);
                actionDelLicense->setEnabled(true);
            }else{
                addNewLicenseButton->setEnabled(false);
                selectLicenseButton->setEnabled(false);
                actionAddNewLicense->setEnabled(false);
                actionSelectlicense->setEnabled(false);
            }
        }

        connect(licenseView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_changeLicenseButton_clicked()));
        connect(licenseView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onLicenseMenu(const QPoint &)));
    }
}

/* Процедура очистки формы*/
void AddEditDevice::clearForm()
{
    // (если тип устройства комплектующее и находтся в составе устройства) или режим рабочего места
    if((m_type == 0 && m_parent_id > 0) || m_mode == Enums::WorkPlace){
        /*Очистка основных полей формы, вкладка Основное*/
        {
            if(m_formMode == Enums::FormModes::Copy && m_mode != Enums::WorkPlace)
                organization->setCurrentIndex(0);
            typeDevice->setCurrentIndex(0);
            name->setText("");
            networkName->setText("");
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
            domainWg->setCurrentIndex(0);
            note->setText("");
            if(m_mode == Enums::WorkPlace){
                m_parent_id = 0;
                orgTex->clear();
            }
        }
        /*Очистка делального описания, вкладка Детальное описание*/
        {
            detailDescription->setPlainText("");
        }
        /*Отключение кнопок сохранить и отмена*/
        {
            buttonSave->setEnabled(false);
            buttonCancel->setEnabled(false);
        }
        /*Очистка модели интерфейсов, вкладка Сеть*/
        {
            interfaceModel->revertAll();
            for(int i = 0;i<interfaceModel->rowCount();i++){
                if(interfaceView->isRowHidden(i,QModelIndex()))
                    interfaceView->setRowHidden(i,QModelIndex(),false);
            }
            buttonChangeInterface->setEnabled(false);
            buttonDelInterface->setEnabled(false);
            actionChangeInterface->setEnabled(false);
            actionDelInterface->setEnabled(false);
            interfaceIsChanged = false;
        }
        /*Очиска модели лицензий, вкладка Лицензии*/
        {
            for(int i = 0;i<lm->rowCount();i++){
                if(licenseView->isRowHidden(i,QModelIndex()))
                    licenseView->setRowHidden(i,QModelIndex(),false);
            }
            lm->clearModel();
            changeLicenseButton->setEnabled(false);
            deleteLicenseButton->setEnabled(false);
            actionEditLicense->setEnabled(false);
            actionDelLicense->setEnabled(false);
            licenseIsChanged = false;
        }
    }else{
        /*Очистка основных полей формы, вкладка Основное*/
        {
            organization->setCurrentIndex(0);
            typeDevice->setCurrentIndex(0);
            name->setText("");
            networkName->setText("");
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
            domainWg->setCurrentIndex(0);
            note->setText("");
        }
        /*Очистка делального описания, вкладка Детальное описание*/
        {
            detailDescription->setPlainText("");
        }
        /*Отключение кнопок сохранить и отмена*/
        {
            buttonSave->setEnabled(false);
            buttonCancel->setEnabled(false);
        }
        /*Очистка модели интерфейсов, вкладка Сеть*/
        {
            interfaceModel->revertAll();
            for(int i = 0;i<interfaceModel->rowCount();i++){
                if(interfaceView->isRowHidden(i,QModelIndex()))
                    interfaceView->setRowHidden(i,QModelIndex(),false);
            }
            buttonChangeInterface->setEnabled(false);
            buttonDelInterface->setEnabled(false);
            actionChangeInterface->setEnabled(false);
            actionDelInterface->setEnabled(false);
            interfaceIsChanged = false;
        }
        /*Очиска модели лицензий, вкладка Лицензии*/
        {
            for(int i = 0;i<lm->rowCount();i++){
                if(licenseView->isRowHidden(i,QModelIndex()))
                    licenseView->setRowHidden(i,QModelIndex(),false);
            }
            lm->clearModel();
            changeLicenseButton->setEnabled(false);
            deleteLicenseButton->setEnabled(false);
            actionEditLicense->setEnabled(false);
            actionDelLicense->setEnabled(false);
            licenseIsChanged = false;
        }
    }
}
/* Функция проверки заполнения ключевых значений формы*/
bool AddEditDevice::dataEntered()
{
    if(organization->currentIndex() != 0 && !name->text().isNull() && !name->text().isEmpty() &&
       typeDevice->currentIndex() != 0 && state->currentIndex() != 0 && m_workPlaceId != 0)
        return true;
    else
        return false;
}
/* Функция проверки является ли форма пустой*/
bool AddEditDevice::formIsEmpty()
{
    if(m_type == 0 && m_parent_id > 0){
        if(typeDevice->currentIndex() == 0 &&
                (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
                (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
                checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
                (note->text().isNull() || note->text().isEmpty()) && (detailDescription->toPlainText().isNull() || detailDescription->toPlainText().isEmpty()) &&
                state->currentIndex() == 0 && domainWg->currentIndex() == 0 && (networkName->text().isNull() || networkName->text().isEmpty()) &&
                !interfaceIsChanged && !licenseIsChanged)
            return true;
        else
            return false;
    }else if(m_mode == Enums::WorkPlace){
        if(m_parent_id == 0 && typeDevice->currentIndex() == 0 &&
                (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
                (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
                checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
                (note->text().isNull() || note->text().isEmpty()) && (detailDescription->toPlainText().isNull() || detailDescription->toPlainText().isEmpty()) &&
                state->currentIndex() == 0 && domainWg->currentIndex() == 0 && (networkName->text().isNull() || networkName->text().isEmpty()) &&
                !interfaceIsChanged && !licenseIsChanged)
            return true;
        else
            return false;
    }else{
        if(organization->currentIndex() == 0 && typeDevice->currentIndex() == 0 &&
                (name->text().isNull() || name->text().isEmpty()) && (inventoryN->text().isNull() || inventoryN->text().isEmpty()) &&
                (serialN->text().isNull() || serialN->text().isEmpty()) && provider->currentIndex() == 0 && producer->currentIndex() == 0 &&
                checkBoxDate1->isChecked() == false && checkBoxDate2->isChecked() == false && price->value() == 0.00 &&
                (note->text().isNull() || note->text().isEmpty()) && (detailDescription->toPlainText().isNull() || detailDescription->toPlainText().isEmpty()) &&
                state->currentIndex() == 0 && m_workPlaceId == 0 && !interfaceIsChanged && !licenseIsChanged &&
                m_parent_id == 0 && domainWg->currentIndex() == 0 && (networkName->text().isNull() || networkName->text().isEmpty()))
            return true;
        else
            return false;
    }
}
/* Функция проверки изменения исходных данных формы*/
bool AddEditDevice::dataChanged()
{
    if(m_rec.value(dm->cIndex.codTypeDevice) != typeDevice->itemData(typeDevice->currentIndex()) ||
            m_rec.value(dm->cIndex.name).toString() != name->text() ||
            m_rec.value(dm->cIndex.networkName).toString() != networkName->text() ||
            m_rec.value(dm->cIndex.inventoryN).toString() != inventoryN->text() ||
            m_rec.value(dm->cIndex.serialN).toString() != serialN->text() ||
            providerIsChanged() || producerIsChanged() || domainWgIsChanged() ||
            m_rec.value(dm->cIndex.price).toDouble() != price->value() ||
            m_rec.value(dm->cIndex.note).toString() != note->text() ||
            m_rec.value(dm->cIndex.detailDescription).toString() != detailDescription->toPlainText() ||
            m_rec.value(dm->cIndex.codState) != state->itemData(state->currentIndex()) ||
            !compareDate(checkBoxDate1,datePurchase,m_rec.value(dm->cIndex.datePurchase).toDate()) ||
            !compareDate(checkBoxDate2,endGuarantee,m_rec.value(dm->cIndex.endGuarantee).toDate()) ||
            !compareDate(checkBoxDate3,datePosting,m_rec.value(dm->cIndex.datePosting).toDate()) ||
            interfaceIsChanged || licenseIsChanged)
        return true;
    else
        return false;
}
/* Функция сравнения дат*/
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
/* Функция проверки изменения значения поля Поставщик*/
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
/* Функция проверки изменения значения поля Производитель*/
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
/* Функция проверки изменения значения поля Домен/Рабочая группа*/
bool AddEditDevice::domainWgIsChanged()
{
    if(!m_rec.value(dm->cIndex.codDomainWg).isNull()){
        if(m_rec.value(dm->cIndex.codDomainWg) != domainWg->itemData(domainWg->currentIndex()))
            return true;
        else
            return false;
    }else{
        if(domainWg->currentIndex() > 0)
            return true;
        else
            return false;
    }
}
/* Процедура заполнения комбобокса*/
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
/* Процедура заполнения формы значениями для редактирования*/
void AddEditDevice::setDefaultEditData()
{
    organization->setCurrentIndex(organization->findData(m_rec.value(dm->cIndex.codOrganization)));
    typeDevice->setCurrentIndex(typeDevice->findData(m_rec.value(dm->cIndex.codTypeDevice)));
    name->setText(m_rec.value(dm->cIndex.name).toString());
    networkName->setText(m_rec.value(dm->cIndex.networkName).toString());
    inventoryN->setText(m_rec.value(dm->cIndex.inventoryN).toString());
    serialN->setText(m_rec.value(dm->cIndex.serialN).toString());
    if(!m_rec.value(dm->cIndex.domainWgName).isNull())
        domainWg->setCurrentIndex(domainWg->findData(m_rec.value(dm->cIndex.codDomainWg)));
    else
        domainWg->setCurrentIndex(0);
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
    detailDescription->setPlainText(m_rec.value(dm->cIndex.detailDescription).toString());
    buttonSave->setEnabled(false);
    buttonCancel->setEnabled(false);
    interfaceModel->revertAll();
    for(int i = 0;i<interfaceModel->rowCount();i++){
        if(interfaceView->isRowHidden(i,QModelIndex()))
            interfaceView->setRowHidden(i,QModelIndex(),false);
    }
    if(interfaceModel->rowCount() > 0){
        interfaceView->setCurrentIndex(interfaceModel->index(0,0));
        buttonChangeInterface->setEnabled(true);
        buttonDelInterface->setEnabled(true);
        actionChangeInterface->setEnabled(true);
        actionDelInterface->setEnabled(true);
    }else{
        buttonChangeInterface->setEnabled(false);
        buttonDelInterface->setEnabled(false);
        actionChangeInterface->setEnabled(false);
        actionDelInterface->setEnabled(false);
    }
    interfaceIsChanged = false;
    lm->revertAll();
    for(int i = 0;i<lm->rowCount();i++){
        if(licenseView->isRowHidden(i,QModelIndex()))
            licenseView->setRowHidden(i,QModelIndex(),false);
    }
    if(lm->rowCount() > 0){
        licenseView->setCurrentIndex(lm->index(0,0));
        changeLicenseButton->setEnabled(true);
        deleteLicenseButton->setEnabled(true);
        actionEditLicense->setEnabled(true);
        actionDelLicense->setEnabled(true);
    }else{
        changeLicenseButton->setEnabled(false);
        deleteLicenseButton->setEnabled(false);
        actionEditLicense->setEnabled(false);
        actionDelLicense->setEnabled(false);
    }
    licenseIsChanged = false;
    licenseIdToUntie.clear();
}
void AddEditDevice::on_buttonCancel_clicked()
{
    if(m_formMode != Enums::FormModes::Edit)
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

    if(m_formMode != Enums::FormModes::Edit){
        workPlace->clear();
        m_workPlaceId = 0;
        m_parent_id = 0;
        orgTex->clear();
        lm->clearModel();
    }
    if(formIsEmpty())
        buttonCancel->setEnabled(false);
    else
        buttonCancel->setEnabled(true);
    if(m_mode != Enums::WorkPlace)
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
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_networkName_textChanged()
{
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_detailDescription_textChanged()
{
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_domainWg_currentIndexChanged(int)
{
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_buttonEditDomainWg_clicked()
{
    CeditTable edittable(this,QString("domainwg"));
    edittable.setWindowTitle(tr("Редактирование доменов/рабочих групп"));
    edittable.exec();
    domainWg->clear();
    populateCBox("CodDomainWg","domainwg","",tr("<Выберите Домен/Рабочую группу>"),domainWg);
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
    if(m_mode == Enums::WorkPlace){
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
                         " AND typedevice.Type = 1")
            .arg(m_workPlaceId)
            .arg(dm->aliasModelTable())
            .arg(dm->colTabName.id)
            .arg(dm->nameModelTable())
            .arg(dm->colTabName.codWorkerPlace);
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
    // Если не активирован режим редактирования
    if(m_formMode != Enums::FormModes::Edit){
        /*Заполняем поля и значеня для запроса добавления нового устройства*/
        {
            field = "("; val = "(";
            field += "parent_id,CodOrganization,CodWorkerPlace,CodTypeDevice,Name,CodState"; val += "?,?,?,?,?,?";
            bindval.enqueue(m_parent_id);
            bindval.enqueue(organization->itemData(organization->currentIndex()).toInt());
            bindval.enqueue(m_workPlaceId);
            bindval.enqueue(typeDevice->itemData(typeDevice->currentIndex()).toInt());
            bindval.enqueue(name->text());
            bindval.enqueue(state->itemData(state->currentIndex()).toInt());

            if(!networkName->text().isNull() && !networkName->text().isEmpty()){
                field += ",NetworkName"; val += ",?";
                bindval.enqueue(networkName->text());}
            if(!inventoryN->text().isNull() && !inventoryN->text().isEmpty()){
                field += ",InventoryN"; val += ",?";
                bindval.enqueue(inventoryN->text());}
            if(!serialN->text().isNull() && !serialN->text().isEmpty()){
                field += ",SerialN"; val += ",?";
                bindval.enqueue(serialN->text());}
            if(provider->currentIndex() != 0){
                field += ",CodProvider"; val += ",?";
                bindval.enqueue(provider->itemData(provider->currentIndex()).toInt());}
            if(domainWg->currentIndex() != 0){
                field += ",CodDomainWg"; val += ",?";
                bindval.enqueue(domainWg->itemData(domainWg->currentIndex()).toInt());}
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
            if(!detailDescription->toPlainText().isNull() && !detailDescription->toPlainText().isEmpty()){
                field += ",DetailDescription"; val += ",?";
                bindval.enqueue(detailDescription->toPlainText());}
            field += ",Price"; val += ",?";
            bindval.enqueue(price->value());
            field += ")"; val += ")";
        }
        /*Формируем запрос*/
        {
            queryStr = "INSERT INTO "+dm->nameModelTable()+" "+field+" VALUES "+val;
            addquery.prepare(queryStr);
            while(!bindval.empty()){
                addquery.addBindValue(bindval.dequeue());
            }
        }
        /*Выполняем запрос*/
        {
            addquery.exec();
            if (addquery.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось добавить устройство:\n %1")
                                         .arg(addquery.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }

        // Получаем ID нового устройства
        lastId = addquery.lastInsertId().toInt();

        /*Добавляем ID нового устройства во все записи модели интерфейсов и выполняем запись интерфейсов в базу*/
        {
            // Прописываем id устройства во все интерфейсы
            for(int i = 0;i<interfaceModel->rowCount();i++){
                if(interfaceModel->data(interfaceModel->index(i,1)).isNull())
                    interfaceModel->setData(interfaceModel->index(i,1),lastId);
            }
            // Записываем интерфейсы в базу
            bool ok = interfaceModel->submitAll();
            if(!ok){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("При добавлении сетевых интерфейсов возникла ошибка:\n %1")
                                         .arg(interfaceModel->lastError().text()),
                                         tr("Закрыть"));
            }else{
                if(interfaceModel->rowCount() > 0){
                    interfaceView->setCurrentIndex(interfaceModel->index(0,0));
                    buttonChangeInterface->setEnabled(true);
                    buttonDelInterface->setEnabled(true);
                    actionChangeInterface->setEnabled(true);
                    actionDelInterface->setEnabled(true);
                }
                interfaceIsChanged = false;
            }
        }
        /*Сохраняем лицензии и связи между устройством и лицензиями*/
        {
            if(lm->rowCount() > 0){
                for(int i = 0; i<lm->rowCount();i++){
                    lm->setData(lm->index(i,lm->cIndex.codOrganization),organization->itemData(organization->currentIndex()).toInt());
                }
                bool ok = lm->submitAll();
                if(!ok){
                    QMessageBox::information(this, tr("Ошибка"),
                                             tr("При добавлении лицензий возникла ошибка:\n %1")
                                             .arg(lm->getLastError().text()),
                                             tr("Закрыть"));
                }else{
                    QSqlQuery query;
                    for(int i = 0; i< lm->rowCount();i++){
                        ok = query.exec(QString("INSERT INTO licenseanddevice (CodDevice,CodLicense) VALUES (%1,%2)")
                                        .arg(lastId)
                                        .arg(lm->data(lm->index(i,lm->cIndex.key)).toInt()));
                        if(!ok){
                            QMessageBox::information(this, tr("Ошибка"),
                                                     tr("При добавлении связи лицензии и устройства возникла ошибка:\n %1")
                                                     .arg(query.lastError().text()),
                                                     tr("Закрыть"));
                        }
                        lm->setData(lm->index(i,lm->cIndex.statusRow),"");
                    }
                    licenseIsChanged = false;
                }
            }
        }

        // Отправляем сигнал о добавлении нового устройства
        emit deviceAdded(lastId,m_parent_id);
        // Очищаем форму если не установленна галочка запрета очистки
        if(!doNotClearForm->isChecked())
            clearForm();
    }else{
        /*Заполняем поля и значеня для запроса обновления данных об устройстве*/
        {
            field = "CodTypeDevice = ?,";
            field += "Name = ?,";
            bindval.enqueue(typeDevice->itemData(typeDevice->currentIndex()).toInt());
            bindval.enqueue(name->text());

            if(!networkName->text().isNull() && !networkName->text().isEmpty()){
                field += "NetworkName = ?, ";
                bindval.enqueue(networkName->text());
            }else
                field += "NetworkName = NULL, ";
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
            if(domainWg->currentIndex() > 0){
                field += "CodDomainWg = ?, ";
                bindval.enqueue(domainWg->itemData(domainWg->currentIndex()).toInt());
            }else
                field += "CodDomainWg = NULL, ";
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
            field += "Price = ?, CodState = ?, Note = ?, DetailDescription = ?, RV = ? WHERE id = ?";
            bindval.enqueue(price->value());
            bindval.enqueue(state->itemData(state->currentIndex()).toInt());
            bindval.enqueue(note->text());
            bindval.enqueue(detailDescription->toPlainText());
            if(m_rec.value(dm->cIndex.rv).toInt() == 254)
                bindval.enqueue(0);
            else
                bindval.enqueue(m_rec.value(dm->cIndex.rv).toInt()+1);
            bindval.enqueue(m_rec.value(dm->cIndex.id).toInt());
        }
        /*Формируем запрос*/
        {
            queryStr = QString("UPDATE %1 SET %2")
                    .arg(dm->nameModelTable())
                    .arg(field);
            addquery.prepare(queryStr);
            while(!bindval.empty()){
                addquery.addBindValue(bindval.dequeue());
            }
        }
        /*Выполняем запрос*/
        {
            addquery.exec();
            if (addquery.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось обновить данные устройства:\n %1")
                                         .arg(addquery.lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
        /*Сохраняем изменения в интерфейсах*/
        {
            bool ok = interfaceModel->submitAll();
            if(!ok){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("При обновлении сетевых интерфейсов возникла ошибка:\n %1")
                                         .arg(interfaceModel->lastError().text()),
                                         tr("Закрыть"));
            }else{
                if(interfaceModel->rowCount() > 0){
                    interfaceView->setCurrentIndex(interfaceModel->index(0,0));
                    buttonChangeInterface->setEnabled(true);
                    buttonDelInterface->setEnabled(true);
                    actionChangeInterface->setEnabled(true);
                    actionDelInterface->setEnabled(true);
                }
                interfaceIsChanged = false;
            }
        }
        /*Сохраняем изменения в связях с лицензиями*/
        {
            if(licenseIsChanged){
                QSqlQuery query;
                bool ok;

                if(lm->rowCount() > 0){
                    QVector<QVariant> isBindedId;
                    for(int i = 0; i< lm->rowCount();i++){
                        if(lm->index(i,lm->cIndex.statusRow).data().toString().isEmpty() ||
                                lm->index(i,lm->cIndex.statusRow).data().toString().isNull())
                            isBindedId.append(lm->data(lm->index(i,lm->cIndex.key)));
                    }
                    // Прописываем организацию в созданные лицензии
                    for(int i = 0; i<lm->rowCount();i++){
                        if(lm->index(i,lm->cIndex.statusRow).data().toString().compare("ins") == 0)
                            lm->setData(lm->index(i,lm->cIndex.codOrganization),m_rec.value(dm->cIndex.codOrganization).toInt());
                    }
                    ok = lm->submitAll();
                    if(!ok){
                        QMessageBox::information(this, tr("Ошибка"),
                                                 tr("При добавлении лицензий возникла ошибка:\n %1")
                                                 .arg(lm->getLastError().text()),
                                                 tr("Закрыть"));
                    }else{
                        for(int i = 0; i< lm->rowCount();i++){
                            if(!isBindedId.contains(lm->data(lm->index(i,lm->cIndex.key)))){
                                ok = query.exec(QString("INSERT INTO licenseanddevice (CodDevice,CodLicense) VALUES (%1,%2)")
                                                .arg(m_rec.value(dm->cIndex.id).toInt())
                                                .arg(lm->data(lm->index(i,lm->cIndex.key)).toInt()));
                                if(!ok){
                                    QMessageBox::information(this, tr("Ошибка"),
                                                             tr("При добавлении связи лицензии и устройства возникла ошибка:\n %1")
                                                             .arg(query.lastError().text()),
                                                             tr("Закрыть"));
                                }else{
                                    lm->setData(lm->index(i,lm->cIndex.statusRow),"");
                                }
                            }
                        }
                        licenseIsChanged = false;
                    }
                    licenseView->setCurrentIndex(lm->index(0,0));
                }else{
                    ok = lm->submitAll();
                    if(!ok){
                        QMessageBox::information(this, tr("Ошибка"),
                                                 tr("При добавлении лицензий возникла ошибка:\n %1")
                                                 .arg(lm->getLastError().text()),
                                                 tr("Закрыть"));
                    }else
                        licenseIsChanged = false;
                }
                if(licenseIdToUntie.size() > 0){
                    for(int i = 0; i<licenseIdToUntie.size();i++){
                        ok = query.exec(QString("DELETE FROM licenseanddevice WHERE CodDevice = %1 AND CodLicense = %2")
                                .arg(m_rec.value(dm->cIndex.id).toInt()).arg(licenseIdToUntie.value(i).toInt()));
                        if(!ok){
                            QMessageBox::information(this, tr("Ошибка"),
                                                     tr("При удалении связи лицензии и устройства возникла ошибка:\n %1")
                                                     .arg(query.lastError().text()),
                                                     tr("Закрыть"));
                        }
                    }
                    licenseIdToUntie.clear();
                }
            }
        }
        /*Сохраняем новые данные в переменной с исходными значениями*/
        {
            m_rec.replace(dm->cIndex.codTypeDevice,typeDevice->itemData(typeDevice->currentIndex()));
            m_rec.replace(dm->cIndex.name,name->text());
            m_rec.replace(dm->cIndex.networkName,networkName->text());
            m_rec.replace(dm->cIndex.codDomainWg,domainWg->itemData(domainWg->currentIndex()));
            m_rec.replace(dm->cIndex.inventoryN,inventoryN->text());
            m_rec.replace(dm->cIndex.serialN,serialN->text());
            m_rec.replace(dm->cIndex.codProvider,provider->itemData(provider->currentIndex()));
            m_rec.replace(dm->cIndex.codProducer,producer->itemData(producer->currentIndex()));
            if(checkBoxDate1->isChecked())
                m_rec.replace(dm->cIndex.datePurchase,datePurchase->date());
            else
                m_rec.replace(dm->cIndex.datePurchase,QVariant());
            if(checkBoxDate3->isChecked())
                m_rec.replace(dm->cIndex.datePosting,datePosting->date());
            else
                m_rec.replace(dm->cIndex.datePosting,QVariant());
            if(checkBoxDate2->isChecked())
                m_rec.replace(dm->cIndex.endGuarantee,endGuarantee->date());
            else
                m_rec.replace(dm->cIndex.endGuarantee,QVariant());
            m_rec.replace(dm->cIndex.price,price->value());
            m_rec.replace(dm->cIndex.codState,state->itemData(state->currentIndex()));
            m_rec.replace(dm->cIndex.note,note->text());
            m_rec.replace(dm->cIndex.detailDescription,detailDescription->toPlainText());
            if(m_rec.value(dm->cIndex.rv).toInt() == 254)
                m_rec.replace(dm->cIndex.rv,0);
            else
                m_rec.replace(dm->cIndex.rv,m_rec.value(dm->cIndex.rv).toInt()+1);
        }
        /*Отключаем кнопки сохранить и отмена*/
        {
            buttonSave->setEnabled(false);
            buttonCancel->setEnabled(false);
        }

        // Отправляем сигнал о изменении данных устройства
        emit deviceDataChanged();
    }
    QMessageBox::information(this, tr("Сохранение"),
                             tr("Данные успешно сохранены!"),
                             tr("Закрыть"));
}
void AddEditDevice::on_typeOrgTex_clicked(bool checked)
{
    if(checked){
        label_12->setVisible(false);
        orgTex->setVisible(false);
        if(tabWidget->count() < 4){
            tabWidget->insertTab(2,networkTab,tr("Сеть"));
            tabWidget->insertTab(3,licenseTab,tr("Лицензии"));
        }
        clearForm();
        m_type = 1;
        populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
    }
}
void AddEditDevice::on_typeHardware_clicked(bool checked)
{
    if(checked){
        label_12->setVisible(true);
        orgTex->setVisible(true);
        if(tabWidget->count() == 4){
            networkTab = tabWidget->widget(2);
            licenseTab = tabWidget->widget(3);
            tabWidget->removeTab(3);
            tabWidget->removeTab(2);
        }
        clearForm();
        m_type = 0;
        populateCBox("CodTypeDevice","typedevice",QString("Type = %1").arg(m_type),tr("<Выберите тип устройства>"),typeDevice);
    }
}
void AddEditDevice::on_buttonClose_clicked()
{
    if(m_formMode == Enums::FormModes::Edit){
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
                    if(button == 1) return;
                }else{
                    on_buttonSave_clicked();
                }
            }
        }

        lockedControl->stopLockRecordThread(m_rec.value(dm->cIndex.id).toInt());
        if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать запись:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
    }
    delete dm;
    reject();
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
    if(m_formMode == Enums::FormModes::Edit){
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
                        lockedControl->stopLockRecordThread(m_rec.value(dm->cIndex.id).toInt());
                        if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                            QMessageBox::warning(this,tr("Ошибка!!!"),
                                                 tr("Не удалось разблокировать запись:\n %1\n")
                                                 .arg(lockedControl->lastError().text()),
                                                 tr("Закрыть"));
                        }
                        delete dm;
                        event->accept();
                    }
                }else{
                    on_buttonSave_clicked();
                    lockedControl->stopLockRecordThread(m_rec.value(dm->cIndex.id).toInt());
                    if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                        QMessageBox::warning(this,tr("Ошибка!!!"),
                                             tr("Не удалось разблокировать запись:\n %1\n")
                                             .arg(lockedControl->lastError().text()),
                                             tr("Закрыть"));
                    }
                    delete dm;
                    event->accept();
                }
            }else{
                lockedControl->stopLockRecordThread(m_rec.value(dm->cIndex.id).toInt());
                if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось разблокировать запись:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                }
                delete dm;
                event->accept();
            }
        }else{
            lockedControl->stopLockRecordThread(m_rec.value(dm->cIndex.id).toInt());
            if(!lockedControl->unlockRecord(m_rec.value(dm->cIndex.id).toInt(),dm->colTabName.id,dm->nameModelTable())){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось разблокировать запись:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }
            delete dm;
            event->accept();
        }
    }else{
        delete dm;
        event->accept();
    }
}
void AddEditDevice::on_buttonAddInterface_clicked()
{
    int currentRowCount = interfaceModel->rowCount();
    int deviceId = -1;
    if(m_formMode == Enums::FormModes::Edit) deviceId = m_rec.value(dm->cIndex.id).toInt();

    AddEditNetworkInterface *aeni = new AddEditNetworkInterface(this, deviceId, interfaceModel);
    if(aeni->exec()){
        if(interfaceModel->rowCount() > 0){
            buttonChangeInterface->setEnabled(true);
            buttonDelInterface->setEnabled(true);
            actionChangeInterface->setEnabled(true);
            actionDelInterface->setEnabled(true);
            if(currentRowCount <= 0)
                interfaceView->setCurrentIndex(interfaceModel->index(0,0));
        }
        interfaceIsChanged = true;
        if(m_formMode != Enums::FormModes::Edit){
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
}
void AddEditDevice::on_buttonChangeInterface_clicked()
{
    int deviceId = -1;
    if(m_formMode == Enums::FormModes::Edit) deviceId = m_rec.value(dm->cIndex.id).toInt();
    AddEditNetworkInterface *aeni = new AddEditNetworkInterface(this, deviceId, interfaceModel, true,interfaceView->currentIndex());
    if(aeni->exec()){
        interfaceIsChanged = true;
        if(m_formMode != Enums::FormModes::Edit){
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
}
void AddEditDevice::on_buttonDelInterface_clicked()
{
    int currentRow = interfaceView->currentIndex().row();
    QModelIndex parent = interfaceView->currentIndex().parent();
    interfaceModel->removeRows(currentRow,1);
    interfaceView->setRowHidden(currentRow,parent,true);

    int count = 0;
    int row = currentRow;
    bool directionUp = false;
    bool currentIndexIsSet = false;
    while(count < interfaceModel->rowCount()){
        if(directionUp){
            row--;
            if(row >= 0){
                if(!interfaceView->isRowHidden(row,parent)){
                    interfaceView->setCurrentIndex(interfaceModel->index(row,0));
                    currentIndexIsSet = true;
                    break;
                }
            }else{
                row  = currentRow;
                directionUp = false;
            }
        }else{
            row++;
            if(row < interfaceModel->rowCount()){
                if(!interfaceView->isRowHidden(row,parent)){
                    interfaceView->setCurrentIndex(interfaceModel->index(row,0));
                    currentIndexIsSet = true;
                    break;
                }
            }else{
                row  = currentRow;
                directionUp = true;
            }
        }
        count++;
    }
    if(!currentIndexIsSet){
        buttonChangeInterface->setEnabled(false);
        buttonDelInterface->setEnabled(false);
        actionChangeInterface->setEnabled(false);
        actionDelInterface->setEnabled(false);
    }
    interfaceIsChanged = true;
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_actionAddInterface_triggered()
{
    on_buttonAddInterface_clicked();
}
void AddEditDevice::on_actionChangeInterface_triggered()
{
    on_buttonChangeInterface_clicked();
}
void AddEditDevice::on_actionDelInterface_triggered()
{
    on_buttonDelInterface_clicked();
}
void AddEditDevice::onInterfaceMenu(const QPoint &p){
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(actionAddInterface);
    ag->addAction(actionChangeInterface);
    ag->addAction(actionDelInterface);
    QMenu *menu = new QMenu(tr("Интерфейсы"), this);
    menu->addActions(ag->actions());
    menu->exec(interfaceView->viewport()->mapToGlobal(p));
}
void AddEditDevice::onLicenseMenu(const QPoint &p)
{
    QActionGroup *agl = new QActionGroup(this);
    agl->addAction(actionAddNewLicense);
    agl->addAction(actionSelectlicense);
    agl->addAction(actionEditLicense);
    agl->addAction(actionDelLicense);
    QMenu *menuL = new QMenu(tr("Лицензии"), this);
    menuL->addActions(agl->actions());
    menuL->exec(licenseView->viewport()->mapToGlobal(p));
}
void AddEditDevice::on_addNewLicenseButton_clicked()
{
    AddEditLicense *ael = new AddEditLicense(this,Enums::Device,0);
    connect(ael,SIGNAL(licenseAdded(QList<QVariant>)),this,SLOT(insertLicenseRow(QList<QVariant>)));
    ael->setAttribute(Qt::WA_DeleteOnClose);
    ael->exec();
}
void AddEditDevice::on_changeLicenseButton_clicked()
{
    QList<QVariant> licenseRowData;
    bool readOnly = false;
    QModelIndex curViewIndex = licenseView->currentIndex();
    if(m_formMode == Enums::FormModes::Read)
        readOnly = true;
    else
    {
        if(lm->index(curViewIndex.row(),lm->cIndex.key).data().toInt() != 0)
        {
            if(!lockedControl->recordIsLosked(lm->data(lm->index(curViewIndex.row(),lm->cIndex.key)).toInt(),
                                              "`"+lm->colTabName.key+"`",
                                              lm->nameModelTable()))
            {
                if(lockedControl->lastError().type() != QSqlError::NoError)
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось получить информацию о блокировке:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
            }else{
                QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                         tr("Запись заблокированна пользователем: %1")
                                         .arg(lockedControl->recordBlockingUser()),
                                         tr("Закрыть"));
                readOnly = true;
            }

            QSqlQuery query;
            bool ok;
            ok = query.exec(QString("SELECT %1, %2 FROM %3 WHERE `%4` = %5")
                            .arg(lm->colTabName.rv)
                            .arg(lm->colTabName.codOrganization)
                            .arg(lm->nameModelTable())
                            .arg(lm->colTabName.key)
                            .arg(lm->data(lm->index(curViewIndex.row(),lm->cIndex.key)).toInt()));
            if(ok){
                query.next();
                if(lm->index(curViewIndex.row(), lm->cIndex.rv).data().toInt() != query.value(0).toInt()){
                    if(query.value(1).toInt() != lm->index(curViewIndex.row(), lm->cIndex.codOrganization).data().toInt()){
                        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                                 tr("Лицензия была перемещена в другую организацию,\n"
                                                    "данная лицензия будет удалена из списка выбранных лицензий."),
                                                 tr("Закрыть"));
                        lm->removeRowFromModel(curViewIndex.row());
                        if(lm->rowCount() > 0)
                            licenseView->setCurrentIndex(lm->index(0,0));
                        return;
                    }else{
                        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                                 tr("Запись была изменена, будут загружены новые данные записи"),
                                                 tr("Закрыть"));
                        if(!lm->updateRowData(curViewIndex.row())){
                            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                                     tr("Не удалось загрузить новые данные:\n%1").arg(lm->getLastError().text()),
                                                     tr("Закрыть"));
                            return;
                        }
                    }
                }
            }else{
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о версии записи:\n %1\n")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
                return;
            }
        }
    }

    for(int i = 0; i<lm->columnCount();i++){
        licenseRowData.append(lm->data(lm->index(licenseView->currentIndex().row(),i)));
    }
    AddEditLicense *ael = new AddEditLicense(this,Enums::Device,true,licenseRowData,readOnly);
    connect(ael,SIGNAL(licenseDataChanged(QList<QVariant>)),this,SLOT(updateLicenseCurrentRow(QList<QVariant>)));
    ael->setAttribute(Qt::WA_DeleteOnClose);
    ael->exec();
}
void AddEditDevice::insertLicenseRow(const QList<QVariant> &licRow)
{
    lm->insertRow(lm->rowCount());
    for(int i = 0; i<lm->columnCount(); i++){
        lm->setData(lm->index(lm->rowCount()-1,i),licRow.value(i));
    }
    if(sender()->objectName() == "SelectLicense")
        lm->setData(lm->index(lm->rowCount()-1,lm->cIndex.statusRow),"bind");
    licenseIsChanged = true;
    licenseView->setCurrentIndex(lm->index(0,0));
    changeLicenseButton->setEnabled(true);
    deleteLicenseButton->setEnabled(true);
    actionEditLicense->setEnabled(true);
    actionDelLicense->setEnabled(true);
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::updateLicenseCurrentRow(const QList<QVariant> &licRow)
{
    QModelIndex curLicIndex = licenseView->currentIndex();
    lm->setRowData(curLicIndex,licRow);
}
void AddEditDevice::on_selectLicenseButton_clicked()
{
    if(organization->currentIndex() <= 0){
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Для выбора существующих лицензий необходимо указать организацию!!!"),
                             tr("Закрыть"));
        return;
    }
    QString filter, dontShowLic;
    dontShowLic = "";
    if(lm->rowCount()>0){
        dontShowLic = "AND l.key NOT IN ( ";
        if(lm->rowCount() == 1){
            if(!lm->data(lm->index(0,lm->cIndex.key)).toString().isEmpty() && !lm->data(lm->index(0,lm->cIndex.key)).toString().isNull())
                dontShowLic += lm->data(lm->index(0,lm->cIndex.key)).toString();
        }else{
            for(int i = 0; i<lm->rowCount();i++){
                if(!lm->data(lm->index(i,lm->cIndex.key)).isNull())
                    dontShowLic += lm->data(lm->index(i,lm->cIndex.key)).toString() + ",";
            }
            if(dontShowLic.length() > 0)
                dontShowLic = dontShowLic.remove(dontShowLic.length()-1,1);
        }
        dontShowLic += " )";
    }
    filter = QString("l.CodOrganization = %1 %2").arg(organization->currentData().toInt()).arg(dontShowLic);
    QMap<int,QString> organizations;
    for(int i = 1; i<organization->count();i++)
        organizations[organization->itemData(i).toInt()] = organization->itemText(i);
    SelectLicense *sl = new SelectLicense(this,filter,true,organizations,organization->currentData().toInt());
    connect(sl,SIGNAL(selectedLicense(QList<QVariant>)),this,SLOT(insertLicenseRow(QList<QVariant>)));
    sl->exec();
}
void AddEditDevice::on_deleteLicenseButton_clicked()
{
    QModelIndex curViewIndex = licenseView->currentIndex();
    if(lm->index(curViewIndex.row(),lm->cIndex.statusRow).data().toString().isEmpty() ||
            lm->index(curViewIndex.row(),lm->cIndex.statusRow).data().toString().isNull())
        licenseIdToUntie.append(lm->index(curViewIndex.row(),lm->cIndex.key).data());
    lm->removeRowFromModel(curViewIndex.row());
    licenseIsChanged = true;
    if(lm->rowCount() <= 0){
        changeLicenseButton->setEnabled(false);
        deleteLicenseButton->setEnabled(false);
        actionEditLicense->setEnabled(false);
        actionDelLicense->setEnabled(false);
    }
    if(m_formMode != Enums::FormModes::Edit){
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
void AddEditDevice::on_actionAddNewLicense_triggered()
{
    on_addNewLicenseButton_clicked();
}
void AddEditDevice::on_actionSelectlicense_triggered()
{
    on_selectLicenseButton_clicked();
}
void AddEditDevice::on_actionEditLicense_triggered()
{
    on_changeLicenseButton_clicked();
}
void AddEditDevice::on_actionDelLicense_triggered()
{
    on_deleteLicenseButton_clicked();
}
