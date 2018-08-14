#ifndef DEVICEIMPORTCOMPARISONWORKPLACE_H
#define DEVICEIMPORTCOMPARISONWORKPLACE_H

#include "ui_deviceimportcomparisonworkplace.h"

class DeviceImportComparisonWorkPlace : public QDialog, private Ui::DeviceImportComparisonWorkPlace
{
    Q_OBJECT
    
public:
    explicit DeviceImportComparisonWorkPlace(QWidget *parent = 0,const QString &reportFile = "", int depFilter = 0, const QMap<QString,QVariant> comparisonWP = QMap<QString,QVariant>());
private:
    int m_depFilter, curRow;
    QMap<QString,QVariant> comparison;
protected:
    void changeEvent(QEvent *e);
signals:
    void comparisonResult(const QMap<QString,QVariant> &comparisons);
private slots:
    void selectWorkPlaceClicked();
    void setComparisonWP(int id, const QString &name);
    void on_buttonAutoComparison_clicked();
    void on_buttonApplay_clicked();
};

#endif // DEVICEIMPORTCOMPARISONWORKPLACE_H
