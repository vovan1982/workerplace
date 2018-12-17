#ifndef LICENSES_H
#define LICENSES_H

#include "ui_licenses.h"

class LicenseModelControl;

class Licenses : public QWidget, private Ui::Licenses
{
    Q_OBJECT

public:
    explicit Licenses(QWidget *parent = 0, bool readOnly = false);
private:
    LicenseModelControl *lModel;
    QString licenseFilter;
    bool filterIsSet;
    bool m_readOnly;
    QMenu *menu;
    QString readDataModel(const QModelIndex &index);
signals:
    void closeLicWin();
protected:
    void changeEvent(QEvent *e);
private slots:
    void onLicMenu(const QPoint &p);
    void doubleClickedLicenseView();
    void updateLicenseModel();
    void updateLicenseRow();
    void setAccessToActions(const QModelIndex &index = QModelIndex());
    void setFilter(const QString &filter);
    void on_closeButton_clicked();
    void on_actionAddLicense_triggered();
    void on_actionEditLicense_triggered();
    void on_actionDelLicense_triggered();
    void on_moveButton_clicked();
    void on_showParentDevice_clicked(bool checked);
    void on_setFilterButton_clicked();
    void on_clearFilterButton_clicked();
    void on_actionMoveLicense_triggered();
    void on_actionHistoryMoved_triggered();
    void on_actionCopyLicenseInBufer_triggered();
    void on_buttonCopyAllInBufer_clicked();
};

#endif // LICENSES_H
