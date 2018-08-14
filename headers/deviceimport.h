#ifndef DEVICEIMPORT_H
#define DEVICEIMPORT_H

#include "ui_deviceimport.h"

class DeviceImport : public QWidget, private Ui::DeviceImport
{
    Q_OBJECT
    
public:
    explicit DeviceImport(QWidget *parent = 0);
private:
    QMap<QString,QVariant> comparisonWP;
    QMap<QString,QVariant> comparisonTypeDev;
    QMap<QString,QVariant> comparisonTypeLic;
    QMap<QString,QVariant> comparisonProducer;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void populateCBox(const QString &select, const QString &fromWhere, const QString &nullStr, QComboBox *cBox);
    bool acceptImport();
    bool comparisonTypeLicIsSet,comparisonTypeDevIsSet;
protected:
    void changeEvent(QEvent *e);
private slots:
    void setComparisonWP(const QMap<QString,QVariant> &comparisons);
    void setComparisonTypeDev(const QMap<QString,QVariant> &comparisons);
    void setComparisonTypeLic(const QMap<QString,QVariant> &comparisons);
    void setComparisonProducer(const QMap<QString,QVariant> &comparisons);
    void on_buttonClose_clicked();
    void on_fileForImport_runButtonClicked();
    void on_buttonDeviceStateEdit_clicked();
    void on_buttonLicenseeStateEdit_clicked();
    void on_organization_currentIndexChanged(int index);
    void on_deviceState_currentIndexChanged(int index);
    void on_licenseeState_currentIndexChanged(int index);
    void on_buttonComparisonWP_clicked();
    void on_buttonComparisonTypeDevice_clicked();
    void on_buttonComparisonTypeLicense_clicked();
    void on_buttonImport_clicked();
    void on_buttonComparisonProducer_clicked();
};

#endif // DEVICEIMPORT_H
