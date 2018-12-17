#include "headers/devicemodelcontrol.h"
#include "headers/devicemodel.h"
#include "headers/delegats.h"

DeviceModelControl::DeviceModelControl(QObject *parent, QTreeView *view, const QString &dbConnectionName, const QSqlDatabase &connectionData, const QString &modelFilter, bool populateModelnInit, bool inThread) :
    QObject(parent),
    m_view(view),
    devFilter(modelFilter)
{
    devModel = new DeviceModel(dbConnectionName,connectionData,this,inThread);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(devModel);
    connect(devModel,SIGNAL(newTreeIsSet()),this,SLOT(dataIsLoaded()));

    view->setModel(proxyModel);
    view->setColumnHidden(devModel->cIndex.id,true);
    view->setColumnHidden(devModel->cIndex.parent_id,true);
    view->setColumnHidden(devModel->cIndex.codTypeDevice,true);
    view->setColumnHidden(devModel->cIndex.codOrganization,true);
    view->setColumnHidden(devModel->cIndex.codWorkerPlace,true);
    view->setColumnHidden(devModel->cIndex.orgName,true);
    view->setColumnHidden(devModel->cIndex.wpName,true);
    view->setColumnHidden(devModel->cIndex.codDomainWg,true);
    view->setColumnHidden(devModel->cIndex.codProducer,true);
    view->setColumnHidden(devModel->cIndex.codProvider,true);
    view->setColumnHidden(devModel->cIndex.codState,true);
    view->setColumnHidden(devModel->cIndex.type,true);
    view->setColumnHidden(devModel->cIndex.iconPath,true);
    view->setColumnHidden(devModel->cIndex.rv,true);
    view->setColumnHidden(devModel->cIndex.detailDescription,true);
    view->setItemDelegateForColumn(devModel->cIndex.price,new DoubleSpinBoxDelegat(0.00,9999999.99,0.01,2,view));

    if(populateModelnInit){
        dataIsLoad = false;
        populateDevModel(devFilter);
    }
}

DeviceModelControl::DeviceModelControl(QObject *parent, const QString &dbConnectionName, const QSqlDatabase &connectionData, const QString &modelFilter, bool populateModelnInit, bool inThread) :
    QObject(parent),
    devFilter(modelFilter)
{
    devModel = new DeviceModel(dbConnectionName,connectionData,this,inThread);
    m_view = 0;
    if(populateModelnInit){
        dataIsLoad = false;
        populateDevModel(devFilter);
    }
}

DeviceModelControl::DeviceModelControl(QObject *parent, QTreeView *view, const QSqlDatabase &connectionData, const QString &dbConnectionName) :
    QObject(parent)
{
    devModel = new DeviceModel(dbConnectionName,connectionData,this,false,true);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(devModel);
    connect(devModel,SIGNAL(newTreeIsSet()),this,SLOT(dataIsLoaded()));

    view->setModel(proxyModel);
    view->setColumnHidden(devModel->cIndex.id,true);
    view->setColumnHidden(devModel->cIndex.parent_id,true);
    view->setColumnHidden(devModel->cIndex.codTypeDevice,true);
    view->setColumnHidden(devModel->cIndex.codOrganization,true);
    view->setColumnHidden(devModel->cIndex.codWorkerPlace,true);
    view->setColumnHidden(devModel->cIndex.orgName,true);
    view->setColumnHidden(devModel->cIndex.wpName,true);
    view->setColumnHidden(devModel->cIndex.codDomainWg,true);
    view->setColumnHidden(devModel->cIndex.codProducer,true);
    view->setColumnHidden(devModel->cIndex.codProvider,true);
    view->setColumnHidden(devModel->cIndex.codState,true);
    view->setColumnHidden(devModel->cIndex.type,true);
    view->setColumnHidden(devModel->cIndex.iconPath,true);
    view->setColumnHidden(devModel->cIndex.rv,true);
    view->setColumnHidden(devModel->cIndex.detailDescription,true);
    view->setItemDelegateForColumn(devModel->cIndex.price,new DoubleSpinBoxDelegat(0.00,9999999.99,0.01,2,view));
    devModel->setHeaderData(devModel->cIndex.typeDevName, Qt::Horizontal,tr("Тип устройства"));
    devModel->setHeaderData(devModel->cIndex.name, Qt::Horizontal,tr("Наименование"));
    devModel->setHeaderData(devModel->cIndex.networkName, Qt::Horizontal,tr("Сетевое имя"));
    devModel->setHeaderData(devModel->cIndex.domainWgName, Qt::Horizontal,tr("Домен/Рабочая группа"));
    devModel->setHeaderData(devModel->cIndex.inventoryN, Qt::Horizontal,tr("Инвентарный №"));
    devModel->setHeaderData(devModel->cIndex.serialN, Qt::Horizontal,tr("Серийный №"));
    devModel->setHeaderData(devModel->cIndex.producerName, Qt::Horizontal,tr("Производитель"));
    devModel->setHeaderData(devModel->cIndex.providerName, Qt::Horizontal,tr("Поставщик"));
    devModel->setHeaderData(devModel->cIndex.datePurchase, Qt::Horizontal,tr("Дата покупки"));
    devModel->setHeaderData(devModel->cIndex.datePosting, Qt::Horizontal,tr("Дата оприходования"));
    devModel->setHeaderData(devModel->cIndex.endGuarantee, Qt::Horizontal,tr("Конец гарантии"));
    devModel->setHeaderData(devModel->cIndex.price, Qt::Horizontal,tr("Стоимость"));
    devModel->setHeaderData(devModel->cIndex.stateName, Qt::Horizontal,tr("Состояние"));
    devModel->setHeaderData(devModel->cIndex.note, Qt::Horizontal,tr("Примечание"));
//    devModel->initEmpty();
}

DeviceModelControl::~DeviceModelControl(){
    delete devModel;
}

void DeviceModelControl::populateDevModel(const QString &filter)
{
    devModel->setFilter(filter);
    devModel->setHeaderData(devModel->cIndex.typeDevName, Qt::Horizontal,tr("Тип устройства"));
    devModel->setHeaderData(devModel->cIndex.name, Qt::Horizontal,tr("Наименование"));
    devModel->setHeaderData(devModel->cIndex.networkName, Qt::Horizontal,tr("Сетевое имя"));
    devModel->setHeaderData(devModel->cIndex.domainWgName, Qt::Horizontal,tr("Домен/Рабочая группа"));
    devModel->setHeaderData(devModel->cIndex.inventoryN, Qt::Horizontal,tr("Инвентарный №"));
    devModel->setHeaderData(devModel->cIndex.serialN, Qt::Horizontal,tr("Серийный №"));
    devModel->setHeaderData(devModel->cIndex.producerName, Qt::Horizontal,tr("Производитель"));
    devModel->setHeaderData(devModel->cIndex.providerName, Qt::Horizontal,tr("Поставщик"));
    devModel->setHeaderData(devModel->cIndex.datePurchase, Qt::Horizontal,tr("Дата покупки"));
    devModel->setHeaderData(devModel->cIndex.datePosting, Qt::Horizontal,tr("Дата оприходования"));
    devModel->setHeaderData(devModel->cIndex.endGuarantee, Qt::Horizontal,tr("Конец гарантии"));
    devModel->setHeaderData(devModel->cIndex.price, Qt::Horizontal,tr("Стоимость"));
    devModel->setHeaderData(devModel->cIndex.stateName, Qt::Horizontal,tr("Состояние"));
    devModel->setHeaderData(devModel->cIndex.note, Qt::Horizontal,tr("Примечание"));
    devModel->select();
}

DeviceModel *DeviceModelControl::model(){
    return devModel;
}

QString DeviceModelControl::filter(){
    return devFilter;
}

void DeviceModelControl::clearDevFilter(){
    devFilter = "";
    dataIsLoad = false;
    populateDevModel(devFilter);
}

void DeviceModelControl::setDevFilter(const QString &filter){
    devFilter = filter;
    dataIsLoad = false;
    populateDevModel(devFilter);
}

void DeviceModelControl::updateDevModel(){
    dataIsLoad = false;
    populateDevModel(devFilter);
}

void DeviceModelControl::setCurrentIndexFirstRow(){
    m_view->setCurrentIndex(proxyModel->index(0,0,QModelIndex()));
}

void DeviceModelControl::setHeaderData()
{
    devModel->setHeaderData(devModel->cIndex.typeDevName, Qt::Horizontal,tr("Тип устройства"));
    devModel->setHeaderData(devModel->cIndex.name, Qt::Horizontal,tr("Наименование"));
    devModel->setHeaderData(devModel->cIndex.networkName, Qt::Horizontal,tr("Сетевое имя"));
    devModel->setHeaderData(devModel->cIndex.domainWgName, Qt::Horizontal,tr("Домен/Рабочая группа"));
    devModel->setHeaderData(devModel->cIndex.inventoryN, Qt::Horizontal,tr("Инвентарный №"));
    devModel->setHeaderData(devModel->cIndex.serialN, Qt::Horizontal,tr("Серийный №"));
    devModel->setHeaderData(devModel->cIndex.producerName, Qt::Horizontal,tr("Производитель"));
    devModel->setHeaderData(devModel->cIndex.providerName, Qt::Horizontal,tr("Поставщик"));
    devModel->setHeaderData(devModel->cIndex.datePurchase, Qt::Horizontal,tr("Дата покупки"));
    devModel->setHeaderData(devModel->cIndex.datePosting, Qt::Horizontal,tr("Дата оприходования"));
    devModel->setHeaderData(devModel->cIndex.endGuarantee, Qt::Horizontal,tr("Конец гарантии"));
    devModel->setHeaderData(devModel->cIndex.price, Qt::Horizontal,tr("Стоимость"));
    devModel->setHeaderData(devModel->cIndex.stateName, Qt::Horizontal,tr("Состояние"));
    devModel->setHeaderData(devModel->cIndex.note, Qt::Horizontal,tr("Примечание"));
    m_view->resizeColumnToContents(devModel->cIndex.typeDevName);
    m_view->resizeColumnToContents(devModel->cIndex.name);
}

QModelIndex DeviceModelControl::realModelIndex(const QModelIndex &viewIndex) const
{
    return proxyModel->mapToSource(viewIndex);
}

QModelIndex DeviceModelControl::realViewIndex(const QModelIndex &modelIndex) const
{
    return proxyModel->mapFromSource(modelIndex);
}

void DeviceModelControl::dataIsLoaded()
{
    if(m_view != 0 && devModel->rowCount() > 0){
        m_view->resizeColumnToContents(devModel->cIndex.typeDevName);
        m_view->resizeColumnToContents(devModel->cIndex.name);
    }
    dataIsLoad = true;
    emit dataIsPopulated();
}
