#include <QtWidgets>
#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>
#include "headers/journalhistorymoved.h"
#include "headers/filterjournalhistorymoved.h"

JournalHistoryMoved::JournalHistoryMoved(QWidget *parent, int itemCod, int itemType, int wpCod) :
    QWidget(parent)
{
    setupUi(this);

    curFilter = "";
    populateCBox("id","departments","firm = 1","<Все>",organization);
    historyModel = new QStandardItemModel(0,14,this);
    historyModel->setHeaderData(0, Qt::Horizontal,tr("Тип"));
    historyModel->setHeaderData(1, Qt::Horizontal,tr("Наименование"));
    historyModel->setHeaderData(2, Qt::Horizontal,tr("Инвентарный №"));
    historyModel->setHeaderData(6, Qt::Horizontal,tr("Дата"));
    historyModel->setHeaderData(7, Qt::Horizontal,tr("Старое место нахождения"));
    historyModel->setHeaderData(8, Qt::Horizontal,tr("Новое место нахождения"));
    historyModel->setHeaderData(10, Qt::Horizontal,tr("Причина"));
    historyModel->setHeaderData(12, Qt::Horizontal,tr("Исполнитель"));
    historyModel->setHeaderData(13, Qt::Horizontal,tr("Примечание"));

    historyView->setModel(historyModel);
    historyView->setColumnHidden(3,true);
    historyView->setColumnHidden(4,true);
    historyView->setColumnHidden(5,true);
    historyView->setColumnHidden(9,true);
    historyView->setColumnHidden(11,true);
    historyView->setColumnHidden(14,true);
    historyView->setColumnHidden(16,true);
    historyView->setColumnHidden(17,true);

    if(itemCod > 0){
        label->setVisible(false);
        organization->setVisible(false);
        horizontalLayout_4->removeItem(horizontalSpacer_2);
        displayMode->setVisible(false);
        buttonSetFilter->setVisible(false);
        buttonClearFilter->setVisible(false);
        if(itemType == 0)
            initModel(QString("historymoved.TypeHistory = 0 AND historymoved.CodMovedDevice = %1").arg(itemCod));
        else if(itemType == 1)
            initModel(QString("historymoved.TypeHistory = 1 AND historymoved.CodMovedLicense = %1").arg(itemCod));
    }else if(wpCod > 0){
        label->setVisible(false);
        organization->setVisible(false);
        horizontalLayout_4->removeItem(horizontalSpacer_2);
        displayMode->setVisible(false);
        buttonSetFilter->setVisible(false);
        buttonClearFilter->setVisible(false);
        initModel(QString("historymoved.CodOldPlace = %1 OR historymoved.CodNewPlace = %1").arg(wpCod));
    }else
        initModel();

    historyView->resizeColumnToContents(1);
    historyView->resizeColumnToContents(2);
    historyView->resizeColumnToContents(7);
    historyView->resizeColumnToContents(8);

    connect(checkDevice,SIGNAL(clicked()),this,SLOT(updateHistoryModel()));
    connect(checkLicense,SIGNAL(clicked()),this,SLOT(updateHistoryModel()));
    connect(organization,SIGNAL(currentIndexChanged(int)),this,SLOT(updateHistoryModel()));
}

void JournalHistoryMoved::changeEvent(QEvent *e)
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

void JournalHistoryMoved::updateHistoryModel()
{
    if(checkDevice->isChecked() && checkLicense->isChecked())
        initModel(curFilter);
    if(!checkDevice->isChecked() && checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 1"+(curFilter.isEmpty()?"":" AND "+curFilter));
    if(checkDevice->isChecked() && !checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 0"+(curFilter.isEmpty()?"":" AND "+curFilter));
    if(!checkDevice->isChecked() && !checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 2"+(curFilter.isEmpty()?"":" AND "+curFilter));
}

void JournalHistoryMoved::updateHistoryModel(const QString &filter)
{
    if(filter.isEmpty() || filter.isNull())
        return;
    curFilter = filter;
    buttonClearFilter->setEnabled(true);
    if(checkDevice->isChecked() && checkLicense->isChecked())
        initModel(curFilter);
    if(!checkDevice->isChecked() && checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 1 AND "+curFilter);
    if(checkDevice->isChecked() && !checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 0 AND "+curFilter);
    if(!checkDevice->isChecked() && !checkLicense->isChecked())
        initModel("historymoved.TypeHistory = 2");
}

void JournalHistoryMoved::on_buttonClose_clicked()
{
    emit closeJhmWin();
}

void JournalHistoryMoved::initModel(const QString &filter)
{
    QString queryStr, organizationFilter, curFilter;
    QSqlQuery query;
    bool ok;
    organizationFilter = "";
    if(organization->currentIndex() > 0){
        organizationFilter = QString("(historymoved.CodMovedDevice IN "
                                     "(SELECT id FROM device WHERE CodOrganization = %1) "
                                     "OR historymoved.CodMovedLicense IN "
                                     "(SELECT `key` FROM licenses WHERE "
                                     "CodWorkerPlace IN (SELECT CodWorkerPlace FROM workerplace WHERE "
                                     "CodDepartment IN (SELECT c.id FROM (SELECT * FROM departments) n, "
                                     "(SELECT * FROM tree) t, "
                                     "(SELECT * FROM departments) c WHERE "
                                     "n.id = %1 AND n.id = t.parent_id AND t.id = c.id))))")
                .arg(organization->itemData(organization->currentIndex()).toInt());
        if(filter.isEmpty())
            curFilter = organizationFilter;
        else
            curFilter = organizationFilter+" AND "+filter;
    }else
        curFilter = filter;

    if(curFilter.isEmpty())
        queryStr = QString("SELECT historymoved.TypeHistory "
                           ", device.Name "
                           ", po.Name "
                           ", device.InventoryN "
                           ", licenses.InvN "
                           ", historymoved.id "
                           ", historymoved.CodMovedDevice "
                           ", historymoved.CodMovedLicense "
                           ", historymoved.DateMoved "
                           ", historymoved.OldPlace "
                           ", historymoved.NewPlace "
                           ", historymoved.CodCause "
                           ", cause.Name "
                           ", historymoved.CodPerformer "
                           ", users.FIOSummary "
                           ", historymoved.Note "
                           ", historymoved.CodOldPlace "
                           ", historymoved.CodNewPlace "
                           "FROM historymoved "
                           "LEFT OUTER JOIN device "
                           "ON historymoved.CodMovedDevice = device.id "
                           "LEFT OUTER JOIN licenses "
                           "ON historymoved.CodMovedLicense = licenses.`key` "
                           "LEFT OUTER JOIN cause "
                           "ON historymoved.CodCause = cause.CodCause "
                           "LEFT OUTER JOIN users "
                           "ON historymoved.CodPerformer = users.CodUser "
                           "LEFT OUTER JOIN po "
                           "ON licenses.CodPO = po.CodPO "
                           "ORDER BY historymoved.DateMoved DESC");
    else
        queryStr = QString("SELECT historymoved.TypeHistory "
                           ", device.Name "
                           ", po.Name "
                           ", device.InventoryN "
                           ", licenses.InvN "
                           ", historymoved.id "
                           ", historymoved.CodMovedDevice "
                           ", historymoved.CodMovedLicense "
                           ", historymoved.DateMoved "
                           ", historymoved.OldPlace "
                           ", historymoved.NewPlace "
                           ", historymoved.CodCause "
                           ", cause.Name "
                           ", historymoved.CodPerformer "
                           ", users.FIOSummary "
                           ", historymoved.Note "
                           ", historymoved.CodOldPlace "
                           ", historymoved.CodNewPlace "
                           "FROM historymoved "
                           "LEFT OUTER JOIN device "
                           "ON historymoved.CodMovedDevice = device.id "
                           "LEFT OUTER JOIN licenses "
                           "ON historymoved.CodMovedLicense = licenses.`key` "
                           "LEFT OUTER JOIN cause "
                           "ON historymoved.CodCause = cause.CodCause "
                           "LEFT OUTER JOIN users "
                           "ON historymoved.CodPerformer = users.CodUser "
                           "LEFT OUTER JOIN po "
                           "ON licenses.CodPO = po.CodPO "
                           "WHERE %1 "
                           "ORDER BY historymoved.DateMoved DESC").arg(curFilter);
    ok = query.exec(queryStr);
    if(!ok){ qDebug()<<query.lastError().text()<<"\n"<<query.lastQuery(); return;}

    historyModel->removeRows(0,historyModel->rowCount());
    if(query.size() > 0){
        while(query.next()){
            historyModel->insertRow(historyModel->rowCount());
            historyView->setCurrentIndex(historyModel->index(0,0));
            if(query.value(0).toInt() == 0){
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,0),tr("Устройство"));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,1),query.value(1));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,2),query.value(3));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,14),0);
                historyModel->item(historyModel->rowCount()-1)->setIcon(QIcon(":/22x22/computers/ico/computer_22x22.png"));
            }else{
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,0),tr("Лицензия"));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,1),query.value(2));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,2),query.value(4));
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,14),1);
                historyModel->item(historyModel->rowCount()-1)->setIcon(QIcon(":/64x64/licence/ico/licence_64x64.png"));
            }

            for(int j = 3; j<historyModel->columnCount();j++){
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,j),query.value(j+2));
            }
        }
    }
}

void JournalHistoryMoved::populateCBox(const QString &idName, const QString &tableName,
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

void JournalHistoryMoved::on_buttonSetFilter_clicked()
{
    int orgId;
    if(organization->currentIndex() > 0)
        orgId = organization->itemData(organization->currentIndex()).toInt();
    else
        orgId = 0;
    FilterJournalHistoryMoved *f;
    if(organization->currentIndex() > 0 || !checkDevice->isChecked() || !checkLicense->isChecked())
        f = new FilterJournalHistoryMoved(this,false,orgId);
    else
        f = new FilterJournalHistoryMoved(this,true,orgId);
    connect(f,SIGNAL(setFilter(QString)),this,SLOT(updateHistoryModel(QString)));
    f->exec();
}

void JournalHistoryMoved::on_buttonClearFilter_clicked()
{
    curFilter = "";
    updateHistoryModel();
    buttonClearFilter->setEnabled(false);
}
