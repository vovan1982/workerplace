#include <QMessageBox>
#include "headers/licensemodel.h"
#include "headers/selectlicense.h"
#include "headers/filterlicense.h"
#include "headers/loadindicator.h"
#include "headers/licensemodelcontrol.h"

SelectLicense::SelectLicense(QWidget *parent, const QString &filter, bool multiselections, QMap<int,QString> organizations, int curOrgId) :
    QDialog(parent),
    m_filter(filter),
    m_multiselections(multiselections),
    m_organizations(organizations),
    m_curOrgId(curOrgId)
{
    setupUi(this);
    dontShowLoadindicator = false;
    filterIsSet = false;
    lockSelected = false;
    selectedFilter = "";
    licenseFilter = curFilter = filter;

    li = new LoadIndicator(licenseView,tr("Подождите идёт загрузка..."));
    lModel = new LicenseModelControl(licenseView,licenseView,showParentDevice->isChecked(),licenseFilter,true);

    connect(lModel,SIGNAL(dataIsPopulated()),this,SLOT(dataIsLoaded()));
    connect(licenseView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setAccessToActions(QModelIndex)));
    connect(licenseView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_selectButton_clicked()));
}
void SelectLicense::showEvent(QShowEvent *e)
{
    QDialog::showEvent( e );
    if(!dontShowLoadindicator){
        dontShowLoadindicator = true;
        showLoadIndicator();
    }
}
void SelectLicense::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    li->updatePosition();
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
void SelectLicense::dataIsLoaded()
{
    li->stop();
    if(lModel->model()->lastError().type() == QSqlError::NoError){
        if(lModel->model()->rowCount() == 0)
            setAccessToActions();
        else{
            showParentDevice->setEnabled(true);
            setAccessToActions(licenseView->currentIndex());
        }
        if(showParentDevice->isChecked())
            licenseView->collapseAll();
    }else{
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Ошибка получения данных:\n %1").arg(lModel->model()->lastError().text()),
                                 tr("Закрыть"));
    }
}
void SelectLicense::showLoadIndicator()
{
    selectButton->setEnabled(false);
    setFilterButton->setEnabled(false);
    clearFilterButton->setEnabled(false);
    showParentDevice->setEnabled(false);
    li->start();
}
void SelectLicense::updateLicenseModel()
{
    showLoadIndicator();
    lModel->updateLicModel();
}

void SelectLicense::on_showParentDevice_clicked(bool checked)
{
    showLoadIndicator();
    lModel->setShowParentDevice(checked);
    lModel->setLicFilterWhithOutUpdate(licenseFilter);
    updateLicenseModel();
}

void SelectLicense::setAccessToActions(const QModelIndex &index)
{
    QModelIndex curModelIndex = lModel->realModelIndex(index);
    if(lModel->model()->rowCount() == 0 && (licenseFilter.isEmpty() || licenseFilter.isNull())){
        selectButton->setEnabled(false);
        setFilterButton->setEnabled(false);
        clearFilterButton->setEnabled(false);
    }else{
        if(curModelIndex.isValid()){
            if(lModel->model()->index(curModelIndex.row(),lModel->model()->cIndex.isLicense,curModelIndex.parent()).data().toInt() == 1){
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
    QModelIndex curIndex = lModel->realModelIndex(licenseView->currentIndex());
    lockSelected = false;

    if(lModel->model()->index(curIndex.row(),lModel->model()->cIndex.isLicense,curIndex.parent()).data().toInt() != 1)
        return;

    for(int i = 0;i<lModel->model()->columnCount();i++){
        license<<lModel->model()->data(lModel->model()->index(curIndex.row(),i,curIndex.parent()));
    }
    if(!m_multiselections){
        emit selectedLicense(license);
        if(lockSelected) return;
        SelectLicense::close();
    }else{
        emit selectedLicense(license);
        if(lockSelected) return;
        QModelIndex curParentIndex = curIndex.parent();
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
    FilterLicense *fl = new FilterLicense(this,m_organizations,m_curOrgId);
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
