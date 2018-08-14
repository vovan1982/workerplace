#ifndef USERIMPORT_H
#define USERIMPORT_H

#include "ui_userimport.h"

class UserImport : public QDialog, private Ui::UserImport
{
    Q_OBJECT
    
public:
    explicit UserImport(QWidget *parent = 0);
private:
    QMap<QString,QVariant> comparisonOrganization;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    bool acceptImport();
protected:
    void changeEvent(QEvent *e);
private slots:
    void setComparisonOrganization(const QMap<QString,QVariant> &comparisons);
    void on_buttonClose_clicked();
    void on_fileForImport_runButtonClicked();
    void on_buttonLoginTypeEdit_clicked();
    void on_loginType_currentIndexChanged(int index);
    void on_buttonComparisonOrganization_clicked();
    void on_buttonImport_clicked();
};

#endif // USERIMPORT_H
