#include "headers/licensemodelcontrol.h"
#include "headers/licensemodel.h"

LicenseModelControl::LicenseModelControl(QObject *parent, QTreeView *view, bool showParentDevice, const QString &modelFilter) :
    QObject(parent),
    m_view(view),
    m_showParentDevice(showParentDevice),
    licFilter(modelFilter)
{
    licModel = new LicenseModel(view);
    view->setModel(licModel);
    populateLicModel(licFilter);
    for(int i = 1; i <= licModel->columnCount(); i++)
        if(i != licModel->cIndex.nameProd && i != licModel->cIndex.invN && i != licModel->cIndex.versionN &&
                i != licModel->cIndex.nameLic && i != licModel->cIndex.nameState)
            view->setColumnHidden(i,true);
    view->resizeColumnToContents(licModel->cIndex.namePO);
    view->resizeColumnToContents(licModel->cIndex.nameProd);
    view->resizeColumnToContents(licModel->cIndex.invN);
    view->resizeColumnToContents(licModel->cIndex.versionN);
    view->resizeColumnToContents(licModel->cIndex.nameLic);
    view->resizeColumnToContents(licModel->cIndex.nameState);
    if(showParentDevice){
        setShowParentDevice(showParentDevice);
        populateLicModel(licFilter);
    }
}

void LicenseModelControl::populateLicModel(const QString &filter)
{
    licModel->setFilter(filter);
    licModel->select();
    licModel->setHeaderData(licModel->cIndex.namePO, Qt::Horizontal,tr("Наименование"));
    licModel->setHeaderData(licModel->cIndex.nameProd, Qt::Horizontal,tr("Производитель"));
    licModel->setHeaderData(licModel->cIndex.invN, Qt::Horizontal,tr("Инвентарный №"));
    licModel->setHeaderData(licModel->cIndex.versionN, Qt::Horizontal,tr("№ версии"));
    licModel->setHeaderData(licModel->cIndex.nameLic, Qt::Horizontal,tr("Тип лицензии"));
    licModel->setHeaderData(licModel->cIndex.nameState, Qt::Horizontal,tr("Состояние"));
    m_view->setRootIsDecorated(m_showParentDevice);
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
    m_view->setCurrentIndex(licModel->index(0,0,QModelIndex()));
    emit licModelUpdated();
}

void LicenseModelControl::setCurrentIndexFirstRow(){
    m_view->setCurrentIndex(licModel->index(0,0,QModelIndex()));
}

void LicenseModelControl::setShowParentDevice(bool show)
{
    QString devId = "";
    m_showParentDevice = show;
    if(show){
        devId = "(";
        if(licModel->rowCount() == 1){
            devId += QString("%1").arg(abs(licModel->data(licModel->index(0,licModel->cIndex.codDevice)).toInt()));
        }else{
            devId += QString("%1").arg(abs(licModel->data(licModel->index(0,licModel->cIndex.codDevice)).toInt()));
            for(int i = 1; i<licModel->rowCount();i++){
                devId += ","+QString("%1").arg(abs(licModel->data(licModel->index(i,licModel->cIndex.codDevice)).toInt()));
            }
        }
        devId += ")";
    }
    licModel->setShowParent(m_showParentDevice,devId);
}

bool LicenseModelControl::showParentDevice()
{
    return m_showParentDevice;
}
