#ifndef USERIMPORTCOMPARISONORGANIZATION_H
#define USERIMPORTCOMPARISONORGANIZATION_H

#include "ui_userimportcomparisonorganization.h"

class QComboBox;

class UserImportComparisonOrganization : public QDialog, private Ui::UserImportComparisonOrganization
{
    Q_OBJECT
    
public:
    explicit UserImportComparisonOrganization(QWidget *parent = 0, const QString &reportFile = "", const QMap<QString,QVariant> comparisonOrganization = QMap<QString,QVariant>());
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
    void setComparisonOrganization(int index);
    void on_buttonApplay_clicked();
};

#endif // USERIMPORTCOMPARISONORGANIZATION_H
