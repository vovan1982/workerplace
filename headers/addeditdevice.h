#ifndef ADDEDITDEVICE_H
#define ADDEDITDEVICE_H

#include <QList>
#include <QSqlDatabase>
#include "ui_addeditdevice.h"

class DeviceModel;

class AddEditDevice : public QDialog, private Ui::AddEditDevice {
    Q_OBJECT
public:
    AddEditDevice(QWidget *parent = 0, int type = 1, int parent_id = 0, int workPlaceId = 0, const QString &workPlaceName = "",
                  bool compositionMode = false, bool editMode = false, const QList<QVariant> &rec = QList<QVariant>(),
                  bool wpMode = false, int firmId = 0, bool readOnly = false);
private:
    QSqlDatabase db;
    int m_type, m_parent_id, m_workPlaceId;
    QString m_workPlaceName;
    bool m_compositionMode,m_editMode;
    QList<QVariant> m_rec;
    bool m_wpMode;
    int m_firmId;
    bool m_readOnly;
    QTimer* timer;
    DeviceModel *dm;
    void clearForm();
    bool dataEntered();
    bool formIsEmpty();
    bool dataChanged();
    bool compareDate(QCheckBox *dateCheck, QDateEdit *dateEdit, QDate defDate);
    bool providerIsChanged();
    bool producerIsChanged();
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void setDefaultEditData();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void deviceAdded();
    void deviceAdded(int newId);
    void deviceAdded(int newId, int parentId);
    void deviceDataChanged();
private slots:
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
    void on_price_valueChanged(double);
    void on_serialN_textChanged();
    void on_inventoryN_textChanged();
    void on_buttonCancel_clicked();
    void on_name_textChanged();
    void on_typeDevice_currentIndexChanged(int);
    void on_state_currentIndexChanged(int);
    void on_buttonEditState_clicked();
    void setWorkPlase(int wpId, const QString &wpName);
    void setOrgTex(const QList<QVariant> &orgTexData);
    void on_buttonEditProducer_clicked();
    void updateLockRecord();
    void on_buttonClose_clicked();
};

#endif // ADDEDITDEVICE_H
