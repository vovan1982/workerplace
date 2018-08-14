#ifndef ADDEDITNETWORKDATA_H
#define ADDEDITNETWORKDATA_H

#include "ui_addeditnetworkdata.h"

class InterfaceModel;

class AddEditNetworkData : public QWidget, private Ui::AddEditNetworkData
{
    Q_OBJECT
    
public:
    explicit AddEditNetworkData(QWidget *parent = 0, int wpId = 0, bool readOnly = false);
private:
    InterfaceModel *interfaceModel;
    int m_wpId, networkDataId, domainWgId;
    bool m_readOnly;
    QString networkName;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void setDefaultData();
    bool dataChanged();
    bool dataIsEntered();
protected:
    void changeEvent(QEvent *e);
private slots:
    void onInterfaceMenu(const QPoint &p);
    void updateCurIndexInterface();
    void on_buttonEditDomainWg_clicked();
    void on_name_textEdited();
    void on_domainWg_currentIndexChanged(int);
    void on_buttonRevertNetworkData_clicked();
    void on_buttonSaveNetworkData_clicked();
    void on_buttonDelNetworkData_clicked();
    void on_buttonAddInterface_clicked();
    void on_buttonChangeInterface_clicked();
    void on_buttonDelInterface_clicked();
    void on_actionAddInterface_triggered();
    void on_actionChangeInterface_triggered();
    void on_actionDelInterface_triggered();
};

#endif // ADDEDITNETWORKDATA_H
