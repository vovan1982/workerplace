#include <QLabel>
#include <QMovie>
#include <QMessageBox>
#include <QtWidgets>
#include "headers/selectdevice.h"
#include "headers/devicemodel.h"
#include "headers/devicemodelcontrol.h"
#include "headers/filterdevice.h"
#include "headers/lockdatabase.h"
#include "headers/loadindicator.h"

SelectDevice::SelectDevice(QWidget *parent, const QString &filter, bool multiselections,
                           bool onlyHardwareMode, bool onlyOrgTexMode, bool locationIsSet) :
    QDialog(parent),
    m_filter(filter),
    m_multiselections(multiselections),
    m_onlyHardwareMode(onlyHardwareMode),
    m_onlyOrgTexMode(onlyOrgTexMode),
    m_locationIsSet(locationIsSet){
    setupUi(this);
    lockSelected = false;
    loadIndicatorIsShowed = false;
    orgTexFilter = "";
    curFilter = filter;
    filterIsSet = false;
    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());
    li = new LoadIndicator(deviceView,tr("Подождите идёт загрузка..."));
    devModel = new DeviceModelControl(deviceView,deviceView,"selectDevice",db,filter);
    connect(devModel,SIGNAL(dataIsPopulated()),this,SLOT(dataIsLoaded()));
    connect(deviceView,SIGNAL(collapsed(QModelIndex)),this,SLOT(setCurIndexIfCollapsed(QModelIndex)));
    connect(deviceView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_buttonSelect_clicked()));

}
void SelectDevice::dataIsLoaded()
{
    li->stop();
    if(devModel->model()->rowCount(QModelIndex()) > 0){
        devModel->setCurrentIndexFirstRow();
        buttonSelect->setEnabled(true);
        buttonSetFilter->setEnabled(true);
    }else{
        if(filterIsSet)
            buttonSetFilter->setEnabled(true);
        else
            buttonSetFilter->setEnabled(false);
    }
    buttonClearFilter->setEnabled(filterIsSet);
}
void SelectDevice::setCurIndexIfCollapsed(const QModelIndex &index)
{
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    if(curViewIndex.parent() == devModel->realModelIndex(index))
        deviceView->setCurrentIndex(index);
}
void SelectDevice::on_buttonSelect_clicked()
{
    QModelIndex curViewIndex = devModel->realModelIndex(deviceView->currentIndex());
    QList<QVariant> device;
    lockSelected = false;
    for(int i = 0;i<devModel->model()->columnCount();i++)
        device<<devModel->model()->data(devModel->model()->index(curViewIndex.row(),i,curViewIndex.parent()));
    if(!m_multiselections){
        emit selectedDevice(device);
        if(lockSelected) return;
        SelectDevice::close();
    }else{
        if(!m_onlyHardwareMode){
            emit selectedDevice(device);
            if(lockSelected) return;
            devModel->model()->removeRow(curViewIndex.row(),curViewIndex.parent());
            selectedDev<<device.value(devModel->model()->cIndex.id);
            if(selectedDev.count() > 0){
                QString dontShow = QString(
                            "%1.%2 NOT IN (SELECT c.%2 FROM %3 n, "
                            "%4 t, %3 c WHERE n.%2 IN (")
                        .arg(devModel->model()->aliasModelTable())
                        .arg(devModel->model()->colTabName.id)
                        .arg(devModel->model()->nameModelTable())
                        .arg(devModel->model()->nameModelTreeTable());
                dontShow += selectedDev.value(0).toString();
                for(int i = 1; i < selectedDev.count(); i++)
                    dontShow += ","+selectedDev.value(i).toString();
                dontShow += QString(") AND n.%1 = t.%2 AND t.%1 = c.%1)")
                        .arg(devModel->model()->colTabName.id)
                        .arg(devModel->model()->colTabName.parent_id);
                if(!m_filter.isEmpty())
                    curFilter = m_filter+" AND "+dontShow;
                else
                    curFilter = dontShow;
            }
        }else{
            if(device.value(devModel->model()->cIndex.type).toInt() == 0){
                emit selectedDevice(device);
                if(lockSelected) return;
                devModel->model()->removeRow(curViewIndex.row(),curViewIndex.parent());
                selectedDev<<device.value(devModel->model()->cIndex.id);
                if(selectedDev.count() > 0){
                    QString dontShow = QString(
                                "%1.%2 NOT IN (SELECT c.%2 FROM %3 n, "
                                "%4 t, %3 c WHERE n.%2 IN (")
                            .arg(devModel->model()->aliasModelTable())
                            .arg(devModel->model()->colTabName.id)
                            .arg(devModel->model()->nameModelTable())
                            .arg(devModel->model()->nameModelTreeTable());;
                    dontShow += selectedDev.value(0).toString();
                    for(int i = 1; i < selectedDev.count(); i++)
                        dontShow += ","+selectedDev.value(i).toString();
                    dontShow += QString(") AND n.%1 = t.%2 AND t.%1 = c.%1)")
                            .arg(devModel->model()->colTabName.id)
                            .arg(devModel->model()->colTabName.parent_id);
                    if(!m_filter.isEmpty())
                        curFilter = m_filter+" AND "+dontShow;
                    else
                        curFilter = dontShow;
                }
            }
        }
    }
    if(devModel->model()->rowCount() <= 0){
        buttonSelect->setEnabled(false);
        buttonSetFilter->setEnabled(filterIsSet);
    }
}
void SelectDevice::on_buttonSetFilter_clicked()
{
    FilterDevice *f = new FilterDevice(this,m_locationIsSet,m_onlyHardwareMode,m_onlyOrgTexMode);
    connect(f,SIGNAL(setFilter(QString)),this,SLOT(setOrgTexFilter(QString)));
    f->exec();
}
void SelectDevice::setOrgTexFilter(const QString &filter)
{
    orgTexFilter = filter;
    if(!curFilter.isEmpty() && !orgTexFilter.isEmpty()){
        showLoadIndicator();
        devModel->setDevFilter(curFilter+" AND "+orgTexFilter);
    }
    if(!curFilter.isEmpty() && orgTexFilter.isEmpty()){
        showLoadIndicator();
        devModel->setDevFilter(curFilter);
    }
    if(curFilter.isEmpty() && !orgTexFilter.isEmpty()){
        showLoadIndicator();
        devModel->setDevFilter(orgTexFilter);
    }
    if(curFilter.isEmpty() && orgTexFilter.isEmpty()){
        showLoadIndicator();
        devModel->setDevFilter("");
    }
    if(filter.isNull() || filter.isEmpty())
        filterIsSet = false;
    else
        filterIsSet = true;
}
void SelectDevice::on_buttonClearFilter_clicked()
{
    setOrgTexFilter("");
}
void SelectDevice::showLoadIndicator()
{
    buttonSelect->setEnabled(false);
    buttonSetFilter->setEnabled(false);
    buttonClearFilter->setEnabled(false);
    li->start();
}
void SelectDevice::lockedErrorMessage(const QString &msg, bool updateModel)
{
    QMessageBox::warning(this,tr("Ошибка!!!"), msg, tr("Закрыть"));
    lockSelected = true;
    if(updateModel)
        devModel->updateDevModel();
}
void SelectDevice::changeEvent(QEvent *e)
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
void SelectDevice::showEvent(QShowEvent *e)
{
    QDialog::showEvent( e );
    if (!loadIndicatorIsShowed){
        loadIndicatorIsShowed = true;
        showLoadIndicator();
    }
}
void SelectDevice::setViewRootIsDecorated(bool show){
    deviceView->setRootIsDecorated(show);
}
void SelectDevice::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    li->updatePosition();
}
