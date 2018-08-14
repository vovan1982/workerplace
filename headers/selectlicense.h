#ifndef SELECTLICENSE_H
#define SELECTLICENSE_H

#include "ui_selectlicense.h"

class LicenseModelControl;

class SelectLicense : public QDialog, private Ui::SelectLicense
{
    Q_OBJECT

public:
    explicit SelectLicense(QWidget *parent = 0, const QString &filter = "", bool multiselections = false, bool locationIsSet = false, int wpId = 0);
private:
     LicenseModelControl *lModel;
     QString licenseFilter, m_filter, curFilter, selectedFilter;
     bool m_multiselections, m_locationIsSet, filterIsSet, lockSelected;
     QList<QVariant> selectedLic;
     int m_wpId;
protected:
    void changeEvent(QEvent *e);
signals:
    void selectedLicense(const QList<QVariant> &lic);
private slots:
    void updateLicenseModel();
    void setAccessToActions(const QModelIndex &index = QModelIndex());
    void setFilter(const QString &filter);
    void on_showParentDevice_clicked(bool checked);
    void on_selectButton_clicked();
    void on_setFilterButton_clicked();
    void on_clearFilterButton_clicked();
    void lockedErrorMessage(const QString &msg, bool updateModel);
};

#endif // SELECTLICENSE_H
