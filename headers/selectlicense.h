#ifndef SELECTLICENSE_H
#define SELECTLICENSE_H

#include <QMap>
#include "ui_selectlicense.h"

class LicenseModelControl;
class LoadIndicator;

class SelectLicense : public QDialog, private Ui::SelectLicense
{
    Q_OBJECT

public:
    explicit SelectLicense(QWidget *parent = 0, const QString &filter = "", bool multiselections = false, QMap<int,QString> organizations = QMap<int,QString>(), int curOrgId = 0);
private:
    LicenseModelControl *lModel;
    LoadIndicator *li;
    QString licenseFilter, m_filter, curFilter, selectedFilter;
    bool m_multiselections, filterIsSet, lockSelected;
    QMap<int,QString> m_organizations;
    int m_curOrgId;
    QList<QVariant> selectedLic;
    bool dontShowLoadindicator;
protected:
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *event);
    void changeEvent(QEvent *e);
signals:
    void selectedLicense(const QList<QVariant> &lic);
private slots:
    void dataIsLoaded();
    void showLoadIndicator();
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
