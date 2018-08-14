#ifndef ADDEDITLICENSE_H
#define ADDEDITLICENSE_H

#include <QSqlDatabase>
#include "ui_addeditlicense.h"

class DeviceModelControl;
class LicenseModel;

class AddEditLicense : public QDialog, private Ui::AddEditLicense
{
    Q_OBJECT

public:
    explicit AddEditLicense(QWidget *parent = 0, bool wpMode = false, int wpId = 0, bool editMode = false,
                            const QList<QVariant> &rec = QList<QVariant>(), bool readOnly = false);
private:
    QSqlDatabase db;
    int m_wpId, poId;
    bool m_wpMode, m_editMode;
    QList<QVariant> m_rec;
    DeviceModelControl *devModel;
    LicenseModel *licModel;
    bool deviceIsSelected, setDefaultDevice;
    bool m_readOnly;
    QTimer* timer;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    bool dataIsEntered();
    bool formIsEmpty();
    void clearForm();
    void setDefaultEditData();
    bool dataChanged();
    bool compareDate(QCheckBox *dateCheck, QDateEdit *dateEdit, QDate defDate);
    bool compareParentDevId(QVariant defDevId);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void licenseAdded();
    void licenseDataChanged();
private slots:
    void setWorkPlase(int id, const QString &name);
    void setPo(const QList<QVariant> &poData);
    void oahIndexChanged(const QModelIndex &index);
    void updateDevice();
    void on_buttonSelectWP_clicked();
    void on_buttonSelectPo_clicked();
    void on_buttonEditTypeLic_clicked();
    void on_buttonEditStateLic_clicked();
    void on_buttonEditProv_clicked();
    void on_checkBindDevice_clicked(bool checked);
    void on_note_textChanged();
    void on_typeLic_currentIndexChanged(int);
    void on_stateLic_currentIndexChanged(int);
    void on_versionN_textEdited();
    void on_invN_textEdited();
    void on_buttonRevert_clicked();
    void on_regName_textEdited();
    void on_regKey_textEdited();
    void on_regEmail_textEdited();
    void on_kolLic_valueChanged(int);
    void on_checkDateEndLic_clicked(bool checked);
    void on_prov_currentIndexChanged(int);
    void on_checkDatePurchase_clicked(bool checked);
    void on_price_valueChanged(double);
    void on_buttonSave_clicked();
    void updateLockRecord();
};

#endif // ADDEDITLICENSE_H
