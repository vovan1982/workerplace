#ifndef DEVICEIMPORTCOMPARISONTYPEDEVICE_H
#define DEVICEIMPORTCOMPARISONTYPEDEVICE_H

#include "ui_deviceimportcomparisontypedevice.h"

class QComboBox;

class DeviceImportComparisonTypeDevice : public QDialog, private Ui::DeviceImportComparisonTypeDevice
{
    Q_OBJECT
    
public:
    explicit DeviceImportComparisonTypeDevice(QWidget *parent = 0, const QString &reportFile = "", const QMap<QString,QVariant> comparisonTypeDev = QMap<QString,QVariant>());
private:
    int curRow;
    QMap<QString,QVariant> comparison;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
protected:
    void changeEvent(QEvent *e);
signals:
    void comparisonResult(const QMap<QString,QVariant> &comparisons);
private slots:
    void setComparisonTypeDev(int index);
    void on_buttonApplay_clicked();
};

#endif // DEVICEIMPORTCOMPARISONTYPEDEVICE_H
