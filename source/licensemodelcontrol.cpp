#include "headers/licensemodelcontrol.h"
#include "headers/licensemodel.h"

LicenseModelControl::LicenseModelControl(QObject *parent, QTreeView *view, bool showParentDevice, const QString &modelFilter, bool inThread) :
    QObject(parent),
    m_view(view),
    m_showParentDevice(showParentDevice),
    licFilter(modelFilter)
{
    licModel = new LicenseModel(view,inThread);
    licModel->setShowParent(m_showParentDevice);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSourceModel(licModel);
    connect(licModel,SIGNAL(newLicensesIsSet()),this,SLOT(dataIsLoaded()));

    view->setModel(proxyModel);

    for(int i = 1; i <= licModel->columnCount(); i++)
        if(i != licModel->cIndex.nameProd && i != licModel->cIndex.invN && i != licModel->cIndex.versionN &&
                i != licModel->cIndex.nameLic && i != licModel->cIndex.nameState && i != licModel->cIndex.regKey &&
                i != licModel->cIndex.kolLicense)
            view->setColumnHidden(i,true);
    view->sortByColumn(0,Qt::AscendingOrder);

    populateLicModel(licFilter);

    if(!inThread){
        view->resizeColumnToContents(licModel->cIndex.namePO);
        view->resizeColumnToContents(licModel->cIndex.nameProd);
        view->resizeColumnToContents(licModel->cIndex.invN);
        view->resizeColumnToContents(licModel->cIndex.versionN);
        view->resizeColumnToContents(licModel->cIndex.nameLic);
        view->resizeColumnToContents(licModel->cIndex.nameState);
        view->resizeColumnToContents(licModel->cIndex.regKey);
        view->resizeColumnToContents(licModel->cIndex.kolLicense);
    }

}

void LicenseModelControl::populateLicModel(const QString &filter)
{
    licModel->clear();
    licModel->setFilter(filter);
    licModel->setHeaderData(licModel->cIndex.namePO, Qt::Horizontal,tr("Наименование"));
    licModel->setHeaderData(licModel->cIndex.nameProd, Qt::Horizontal,tr("Производитель"));
    licModel->setHeaderData(licModel->cIndex.regKey, Qt::Horizontal,tr("Ключ регистрации"));
    licModel->setHeaderData(licModel->cIndex.invN, Qt::Horizontal,tr("Инвентарный №"));
    licModel->setHeaderData(licModel->cIndex.versionN, Qt::Horizontal,tr("№ версии"));
    licModel->setHeaderData(licModel->cIndex.nameLic, Qt::Horizontal,tr("Тип лицензии"));
    licModel->setHeaderData(licModel->cIndex.nameState, Qt::Horizontal,tr("Состояние"));
    licModel->setHeaderData(licModel->cIndex.kolLicense, Qt::Horizontal,tr("Кол-во лицензий"));
    m_view->setRootIsDecorated(m_showParentDevice);
    licModel->select();
}

LicenseModel *LicenseModelControl::model(){
    return licModel;
}

QString LicenseModelControl::filter(){
    return licFilter;
}

void LicenseModelControl::clearLicFilter(){
    licFilter = "";
    populateLicModel(licFilter);
}

void LicenseModelControl::setLicFilterAndUpdate(const QString &filter){
    licFilter = filter;
    populateLicModel(licFilter);
}

void LicenseModelControl::setLicFilterWhithOutUpdate(const QString &filter)
{
    licFilter = filter;
}

void LicenseModelControl::updateLicModel(){
    populateLicModel(licFilter);
}

void LicenseModelControl::setCurrentIndexFirstRow(){
    if(licModel->rowCount() > 0)
        m_view->setCurrentIndex(proxyModel->index(0,0,QModelIndex()));
}

void LicenseModelControl::setShowParentDevice(bool show)
{
    m_showParentDevice = show;
    licModel->setShowParent(m_showParentDevice);
}

bool LicenseModelControl::showParentDevice()
{
    return m_showParentDevice;
}

QModelIndex LicenseModelControl::realModelIndex(const QModelIndex &viewIndex) const
{
    return proxyModel->mapToSource(viewIndex);
}

QModelIndex LicenseModelControl::realViewIndex(const QModelIndex &modelIndex) const
{
    return proxyModel->mapFromSource(modelIndex);
}

void LicenseModelControl::dataIsLoaded()
{
    if(m_view != 0 && licModel->rowCount() > 0){
        m_view->resizeColumnToContents(licModel->cIndex.namePO);
        m_view->resizeColumnToContents(licModel->cIndex.nameProd);
        m_view->resizeColumnToContents(licModel->cIndex.invN);
        m_view->resizeColumnToContents(licModel->cIndex.versionN);
        m_view->resizeColumnToContents(licModel->cIndex.nameLic);
        m_view->resizeColumnToContents(licModel->cIndex.nameState);
        m_view->resizeColumnToContents(licModel->cIndex.regKey);
        m_view->resizeColumnToContents(licModel->cIndex.kolLicense);
        setCurrentIndexFirstRow();
    }
    if(licModel->lastError().type() != QSqlError::NoError)
        qDebug()<<licModel->lastError().text();
    emit dataIsPopulated();
}
