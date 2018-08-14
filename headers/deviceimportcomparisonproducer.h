#ifndef DEVICEIMPORTCOMPARISONPRODUCER_H
#define DEVICEIMPORTCOMPARISONPRODUCER_H

#include "ui_deviceimportcomparisonproducer.h"

class QComboBox;

class DeviceImportComparisonProducer : public QDialog, private Ui::DeviceImportComparisonProducer
{
    Q_OBJECT
    
public:
    explicit DeviceImportComparisonProducer(QWidget *parent = 0, const QString &reportFile = "", const QMap<QString,QVariant> comparisonProducer = QMap<QString,QVariant>());
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
    void setComparisonProducer(int index);
    void on_buttonApplay_clicked();
};

#endif // DEVICEIMPORTCOMPARISONPRODUCER_H
