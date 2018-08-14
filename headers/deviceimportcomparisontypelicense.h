#ifndef DEVICEIMPORTCOMPARISONTYPELICENSE_H
#define DEVICEIMPORTCOMPARISONTYPELICENSE_H

#include "ui_deviceimportcomparisontypelicense.h"

class QComboBox;

class DeviceImportComparisonTypeLicense : public QDialog, private Ui::DeviceImportComparisonTypeLicense
{
    Q_OBJECT
    
public:
    explicit DeviceImportComparisonTypeLicense(QWidget *parent = 0, const QString &reportFile = "", const QMap<QString,QVariant> comparisonTypeLic = QMap<QString,QVariant>());
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
    void setComparisonTypeLic(int index);
    void on_buttonApplay_clicked();
};

#endif // DEVICEIMPORTCOMPARISONTYPELICENSE_H
