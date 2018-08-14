#include <QtSql>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QFileDialog>
#include "headers/deviceimport.h"
#include "headers/edittable.h"
#include "headers/deviceimportcomparisonworkplace.h"
#include "headers/deviceimportcomparisontypedevice.h"
#include "headers/deviceimportcomparisontypelicense.h"
#include "headers/deviceimportcomparisonproducer.h"
#include "headers/devicereportparser.h"

DeviceImport::DeviceImport(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    fileForImport->setVisibleClearButtron(false);
    populateCBox("CodState","statedev","",tr("<Выберите состояние устройств>"),deviceState);
    populateCBox("CodStatePO","statepo","",tr("<Выберите состояние лицензий>"),licenseeState);
    populateCBox("id","departments","firm = 1",tr("<Выберите фирму>"),organization);
    comparisonWP = QMap<QString,QVariant>();
    comparisonTypeDev = QMap<QString,QVariant>();
    comparisonTypeLic = QMap<QString,QVariant>();
    comparisonProducer = QMap<QString,QVariant>();
    comparisonTypeLicIsSet = false;
    comparisonTypeDevIsSet = false;
}

void DeviceImport::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void DeviceImport::populateCBox(const QString &idName, const QString &tableName,
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

void DeviceImport::populateCBox(const QString &select, const QString &fromWhere, const QString &nullStr, QComboBox *cBox)
{
    QSqlQuery query;
    query.exec(QString("SELECT %1 FROM %2").arg(select).arg(fromWhere));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось заполнить комбобокс %2:\n %1")
                                 .arg(query.lastError().text())
                                 .arg(cBox->objectName()),
                                 tr("Закрыть"));
        return;
    }
    if(!nullStr.isEmpty())
        cBox->addItem(nullStr,0);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

bool DeviceImport::acceptImport()
{
    if(!fileForImport->text().isNull() && !fileForImport->text().isEmpty() && organization->currentIndex() > 0 &&
            deviceState->currentIndex() > 0 && licenseeState->currentIndex() > 0 &&
            !comparisonWP.isEmpty() && !comparisonTypeDev.isEmpty() && comparisonTypeLicIsSet && comparisonTypeDevIsSet)
        return true;
    else
        return false;
}

void DeviceImport::setComparisonWP(const QMap<QString,QVariant> &comparisons)
{
    comparisonWP = comparisons;
    buttonComparisonWP->setIcon(QIcon(":/16x16/apply/ico/agt_action_success_16x16.png"));
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::setComparisonTypeDev(const QMap<QString,QVariant> &comparisons)
{
    comparisonTypeDev = comparisons;
    buttonComparisonTypeDevice->setIcon(QIcon(":/16x16/apply/ico/agt_action_success_16x16.png"));
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::setComparisonTypeLic(const QMap<QString,QVariant> &comparisons)
{
    comparisonTypeLic = comparisons;
    buttonComparisonTypeLicense->setIcon(QIcon(":/16x16/apply/ico/agt_action_success_16x16.png"));
    comparisonTypeLicIsSet = true;
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::setComparisonProducer(const QMap<QString,QVariant> &comparisons)
{
    comparisonProducer = comparisons;
    buttonComparisonProducer->setIcon(QIcon(":/16x16/apply/ico/agt_action_success_16x16.png"));
    comparisonTypeDevIsSet = true;
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::on_buttonClose_clicked()
{
    QWidget *parent = this->parentWidget();
    while (parent) {
        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow *>(parent);
        if (subWindow) {
            subWindow->close();
            break;
        }
        parent = parent->parentWidget();
    }
}

void DeviceImport::on_fileForImport_runButtonClicked()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Выберите файл"),
                                              QString(), tr("XML files (*.xml);;All Files (*)"));
    if (fn.isEmpty()) return;
    if(!fn.isNull() && !fn.isEmpty()){
        fileForImport->setText(fn);
        importFileStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
        organization->setEnabled(true);
        deviceState->setEnabled(true);
        buttonDeviceStateEdit->setEnabled(true);
        licenseeState->setEnabled(true);
        buttonLicenseeStateEdit->setEnabled(true);
        buttonComparisonWP->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonWP.clear();
        buttonComparisonTypeDevice->setEnabled(true);
        buttonComparisonTypeDevice->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        buttonComparisonTypeLicense->setEnabled(true);
        buttonComparisonTypeLicense->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        buttonComparisonProducer->setEnabled(true);
        buttonComparisonProducer->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
    }
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::on_buttonDeviceStateEdit_clicked()
{
    CeditTable edittable(this,QString("statedev"));
    edittable.setWindowTitle(tr("Редактирование состояний устройств"));
    edittable.exec();
    deviceState->clear();
    populateCBox("CodState","statedev","",tr("<Выберите состояние устройств>"),deviceState);
}

void DeviceImport::on_buttonLicenseeStateEdit_clicked()
{
    CeditTable edittable(this,QString("statepo"));
    edittable.setWindowTitle(tr("Редактирование состояний лицензии"));
    edittable.exec();
    licenseeState->clear();
    populateCBox("CodStatePO","statepo","",tr("<Выберите состояние лицензий>"),licenseeState);
}

void DeviceImport::on_organization_currentIndexChanged(int index)
{
    if(index > 0){
        organizationStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
        filials->setEnabled(true);
        populateCBox("c.id, c.name",
                     QString("departments n, tree t, departments c WHERE n.id = %1"
                             " AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1").arg(organization->itemData(index).toInt()),
                     tr("<Выберите филиал/представительство>"),
                     filials);
        buttonComparisonWP->setEnabled(true);
        buttonComparisonWP->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonWP.clear();
    }else{
        organizationStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        filials->setEnabled(false);
        filials->clear();
        buttonComparisonWP->setEnabled(false);
        buttonComparisonWP->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonWP.clear();
    }
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::on_deviceState_currentIndexChanged(int index)
{
    if(index > 0)
        deviceStateStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
    else
        deviceStateStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::on_licenseeState_currentIndexChanged(int index)
{
    if(index > 0)
        licenseeStateStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
    else
        licenseeStateStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
    buttonImport->setEnabled(acceptImport());
}

void DeviceImport::on_buttonComparisonWP_clicked()
{
    DeviceImportComparisonWorkPlace *dicwp = new DeviceImportComparisonWorkPlace(this,fileForImport->text(),
                                                                                 (filials->currentIndex()>0)?
                                                                                     filials->itemData(filials->currentIndex()).toInt():
                                                                                     organization->itemData(organization->currentIndex()).toInt(),
                                                                                 comparisonWP);
    connect(dicwp,SIGNAL(comparisonResult(QMap<QString,QVariant>)),this,SLOT(setComparisonWP(QMap<QString,QVariant>)));
    dicwp->exec();
}

void DeviceImport::on_buttonComparisonTypeDevice_clicked()
{
    DeviceImportComparisonTypeDevice *dictd = new DeviceImportComparisonTypeDevice(this,fileForImport->text(), comparisonTypeDev);
    connect(dictd,SIGNAL(comparisonResult(QMap<QString,QVariant>)),this,SLOT(setComparisonTypeDev(QMap<QString,QVariant>)));
    dictd->exec();
}

void DeviceImport::on_buttonComparisonTypeLicense_clicked()
{
    DeviceImportComparisonTypeLicense *dictl = new DeviceImportComparisonTypeLicense(this,fileForImport->text(), comparisonTypeLic);
    connect(dictl,SIGNAL(comparisonResult(QMap<QString,QVariant>)),this,SLOT(setComparisonTypeLic(QMap<QString,QVariant>)));
    dictl->exec();
}

void DeviceImport::on_buttonComparisonProducer_clicked()
{
    DeviceImportComparisonProducer *dicp = new DeviceImportComparisonProducer(this,fileForImport->text(), comparisonProducer);
    connect(dicp,SIGNAL(comparisonResult(QMap<QString,QVariant>)),this,SLOT(setComparisonProducer(QMap<QString,QVariant>)));
    dicp->exec();
}

void DeviceImport::on_buttonImport_clicked()
{
    QList<QString> attr;
    attr<<organization->itemData(organization->currentIndex()).toString()
       <<deviceState->itemData(deviceState->currentIndex()).toString()
      <<licenseeState->itemData(licenseeState->currentIndex()).toString();
    DeviceReportParser handler(attr,comparisonWP,comparisonTypeDev,comparisonTypeLic,comparisonProducer);
    QFile             file(fileForImport->text());
    QXmlInputSource   source(&file);
    QXmlSimpleReader  reader;

    reader.setContentHandler(&handler);
    reader.parse(source);
    if (handler.errorString().isEmpty()){
        QMessageBox::information(this, tr("Импорт"), tr("Импорт устройст выполнен успешно."), tr("Закрыть"));
        fileForImport->clearButton_clicked();
        importFileStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        organization->setCurrentIndex(0);
        on_organization_currentIndexChanged(0);
        organization->setEnabled(false);
        deviceState->setCurrentIndex(0);
        on_deviceState_currentIndexChanged(0);
        deviceState->setEnabled(false);
        buttonDeviceStateEdit->setEnabled(false);
        licenseeState->setCurrentIndex(0);
        on_licenseeState_currentIndexChanged(0);
        licenseeState->setEnabled(false);
        buttonLicenseeStateEdit->setEnabled(false);
        buttonComparisonWP->setEnabled(false);
        buttonComparisonWP->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonWP.clear();
        buttonComparisonTypeDevice->setEnabled(false);
        buttonComparisonTypeDevice->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonTypeDev.clear();
        buttonComparisonTypeLicense->setEnabled(false);
        buttonComparisonTypeLicense->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonTypeLic.clear();
        buttonComparisonProducer->setEnabled(false);
        buttonComparisonProducer->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonProducer.clear();
        buttonImport->setEnabled(acceptImport());
    }else
        QMessageBox::warning(this, tr("Ошибка"), tr("Во время импорта произошла ошибка:\n%1")
                             .arg(handler.errorString()), tr("Закрыть"));
}
