#include <QMessageBox>
#include "headers/selectlicense.h"
#include "headers/licensemodel.h"
#include "headers/filterlicense.h"
#include "headers/licensemodelcontrol.h"

SelectLicense::SelectLicense(QWidget *parent, const QString &filter, bool multiselections, bool locationIsSet, int wpId) :
    QDialog(parent),
    m_filter(filter),
    m_multiselections(multiselections),
    m_locationIsSet(locationIsSet),
    m_wpId(wpId)
{
    setupUi(this);
    filterIsSet = false;
    lockSelected = false;
    selectedFilter = "";
    licenseFilter = curFilter = filter;

    lModel = new LicenseModelControl(licenseView,licenseView,showParentDevice->isChecked(),licenseFilter);

    connect(licenseView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setAccessToActions(QModelIndex)));
    if(lModel->model()->rowCount() == 0){
        selectButton->setEnabled(false);
        setFilterButton->setEnabled(false);
        clearFilterButton->setEnabled(false);
        showParentDevice->setEnabled(false);
    }else
        lModel->setCurrentIndexFirstRow();
}

void SelectLicense::changeEvent(QEvent *e)
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

void SelectLicense::updateLicenseModel()
{
    lModel->updateLicModel();
    if(lModel->model()->rowCount() > 0)
        lModel->setCurrentIndexFirstRow();
    setAccessToActions(licenseView->currentIndex());
    if(showParentDevice->isChecked())
        licenseView->expandAll();
}

void SelectLicense::on_showParentDevice_clicked(bool checked)
{
    lModel->setShowParentDevice(checked);
    lModel->setLicFilterWhithOutUpdate(licenseFilter);
    updateLicenseModel();
}

void SelectLicense::setAccessToActions(const QModelIndex &index)
{
    if(lModel->model()->rowCount() == 0 && (licenseFilter.isEmpty() || licenseFilter.isNull())){
        selectButton->setEnabled(false);
        setFilterButton->setEnabled(false);
        clearFilterButton->setEnabled(false);
    }else{
        if(index.isValid()){
            if(lModel->model()->index(index.row(),lModel->model()->cIndex.isLicense,index.parent()).data().toInt() == 1){
                selectButton->setEnabled(true);
            }else{
                selectButton->setEnabled(false);
            }
            setFilterButton->setEnabled(true);
        }else
            selectButton->setEnabled(false);
        clearFilterButton->setEnabled(filterIsSet);
    }
}

void SelectLicense::on_selectButton_clicked()
{
    QList<QVariant> license;
    lockSelected = false;
    for(int i = 0;i<lModel->model()->columnCount();i++){
        license<<lModel->model()->data(lModel->model()->index(licenseView->currentIndex().row(),i,licenseView->currentIndex().parent()));
    }
    if(!m_multiselections){
        emit selectedLicense(license);
        if(lockSelected) return;
        SelectLicense::close();
    }else{
        emit selectedLicense(license);
        if(lockSelected) return;
        QModelIndex curIndex = licenseView->currentIndex();
        QModelIndex curParentIndex = licenseView->currentIndex().parent();
        lModel->model()->removeRow(curIndex.row(),curIndex.parent());
        curIndex = curParentIndex;
        if(curIndex != QModelIndex())
            while(curIndex != QModelIndex()){
                lModel->model()->removeRow(curIndex.row(),curIndex.parent());
                curIndex = curIndex.parent();
            }
        selectedLic<<license.value(lModel->model()->cIndex.key);
        if(selectedLic.count() > 0){
            QString dontShow = "l.key NOT IN (";
            dontShow += selectedLic.value(0).toString();
            for(int i = 1; i < selectedLic.count(); i++)
                dontShow += ","+selectedLic.value(i).toString();
            dontShow += ")";
            if(!m_filter.isEmpty())
                curFilter = m_filter+" AND "+dontShow;
            else
                curFilter = dontShow;
            if(selectedFilter.isEmpty())
                licenseFilter = curFilter;
            else
                licenseFilter = curFilter+" AND "+selectedFilter;
        }
    }
    if(lModel->model()->rowCount() <= 0){
        selectButton->setEnabled(false);
        setFilterButton->setEnabled(filterIsSet);
    }
}

void SelectLicense::on_setFilterButton_clicked()
{
    FilterLicense *fl = new FilterLicense(this,m_locationIsSet,m_wpId);
    connect(fl,SIGNAL(setFilter(QString)),SLOT(setFilter(QString)));
    fl->exec();
}

void SelectLicense::setFilter(const QString &filter)
{
    selectedFilter = filter;
    if(!curFilter.isEmpty() && !selectedFilter.isEmpty())
        licenseFilter = curFilter+" AND "+selectedFilter;
    if(!curFilter.isEmpty() && selectedFilter.isEmpty())
        licenseFilter = curFilter;
    if(curFilter.isEmpty() && !selectedFilter.isEmpty())
        licenseFilter = selectedFilter;
    if(curFilter.isEmpty() && selectedFilter.isEmpty())
        licenseFilter = "";
    if(selectedFilter.isNull() || selectedFilter.isEmpty()){
        filterIsSet = false;
    }else{
        filterIsSet = true;
    }
    lModel->setLicFilterWhithOutUpdate(licenseFilter);
    updateLicenseModel();
}

void SelectLicense::on_clearFilterButton_clicked()
{
    setFilter("");
}

void SelectLicense::lockedErrorMessage(const QString &msg, bool updateModel)
{
    QMessageBox::warning(this,tr("Ошибка!!!"), msg, tr("Закрыть"));
    lockSelected = true;
    if(updateModel){
        lModel->updateLicModel();
        licenseView->setCurrentIndex(lModel->model()->index(0,0));
    }
}
