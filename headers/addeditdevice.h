#ifndef ADDEDITDEVICE_H
#define ADDEDITDEVICE_H

#include <QList>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "ui_addeditdevice.h"
#include "headers/enums.h"

class DeviceModel;
class LicenseTableModel;
class LockDataBase;

class AddEditDevice : public QDialog, private Ui::AddEditDevice {
    Q_OBJECT
public:
    AddEditDevice(QWidget *parent = 0, int type = 1, int parent_id = 0, Enums::FormModes formMode = Enums::FormModes::Add,
                  const QList<QVariant> &rec = QList<QVariant>(), int workPlaceId = 0, const QString &workPlaceName = "",
                  Enums::Modes mode = Enums::Standart,int firmId = 0);
private:
    QSqlDatabase db;
    int m_type, m_parent_id;
    Enums::FormModes m_formMode;
    QList<QVariant> m_rec;
    int  m_workPlaceId;
    QString m_workPlaceName;
    Enums::Modes m_mode;
    DeviceModel *dm;
    QWidget *networkTab, *licenseTab;
    QSqlTableModel *interfaceModel;
    LicenseTableModel *lm;
    bool interfaceIsChanged, licenseIsChanged;
    QList<QVariant> licenseIdToUntie;
    LockDataBase *lockedControl;
    void clearForm();
    bool dataEntered();
    bool formIsEmpty();
    bool dataChanged();
    bool compareDate(QCheckBox *dateCheck, QDateEdit *dateEdit, QDate defDate);
    bool providerIsChanged();
    bool producerIsChanged();
    bool domainWgIsChanged();
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void setDefaultEditData();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void deviceAdded(int newId, int parentId);
    void deviceDataChanged();
private slots:
    void setWorkPlase(int wpId, const QString &wpName);
    void setOrgTex(const QList<QVariant> &orgTexData);
    void onInterfaceMenu(const QPoint &p);
    void onLicenseMenu(const QPoint &p);
    void insertLicenseRow(const QList<QVariant> &licRow);
    void updateLicenseCurrentRow(const QList<QVariant> &licRow);
    void on_orgTex_runButtonClicked();
    void on_orgTex_clearButtonClicked();
    void on_workPlace_textChanged();
    void on_typeHardware_clicked(bool checked);
    void on_typeOrgTex_clicked(bool checked);
    void on_buttonEditProvider_clicked();
    void on_provider_currentIndexChanged(int);
    void on_producer_currentIndexChanged(int);
    void on_organization_currentIndexChanged(int index);
    void on_workPlace_runButtonClicked();
    void on_buttonSave_clicked();
    void on_buttonEditTypeDevice_clicked();
    void on_checkBoxDate1_clicked(bool check);
    void on_checkBoxDate2_clicked(bool check);
    void on_checkBoxDate3_clicked(bool check);
    void on_note_textChanged();
    void on_detailDescription_textChanged();
    void on_price_valueChanged(double);
    void on_serialN_textChanged();
    void on_inventoryN_textChanged();
    void on_buttonCancel_clicked();
    void on_name_textChanged();
    void on_networkName_textChanged();
    void on_domainWg_currentIndexChanged(int);
    void on_typeDevice_currentIndexChanged(int);
    void on_state_currentIndexChanged(int);
    void on_buttonEditState_clicked();
    void on_buttonEditDomainWg_clicked();
    void on_buttonEditProducer_clicked();
    void on_buttonClose_clicked();
    void on_buttonAddInterface_clicked();
    void on_buttonChangeInterface_clicked();
    void on_buttonDelInterface_clicked();
    void on_actionAddInterface_triggered();
    void on_actionChangeInterface_triggered();
    void on_actionDelInterface_triggered();
    void on_addNewLicenseButton_clicked();
    void on_changeLicenseButton_clicked();
    void on_selectLicenseButton_clicked();
    void on_deleteLicenseButton_clicked();
    void on_actionAddNewLicense_triggered();
    void on_actionSelectlicense_triggered();
    void on_actionEditLicense_triggered();
    void on_actionDelLicense_triggered();
};

#endif // ADDEDITDEVICE_H
