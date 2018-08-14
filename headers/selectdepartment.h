#ifndef SELECTDEPARTMENT_H
#define SELECTDEPARTMENT_H

#include "ui_selectdepartment.h"

class SqlTreeModel;

class SelectDepartment : public QDialog, private Ui::SelectDepartment {
    Q_OBJECT
public:
    SelectDepartment(QWidget *parent = 0, int orgId = 0);
private:
    SqlTreeModel *modelDepart;
    int m_orgId;
    void populateModDep(int organizationId);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, QComboBox *cBox);
protected:
    void changeEvent(QEvent *e);
signals:
    void addDepartment(int depId, const QString &depName, int organizationId);
private slots:
    void on_organization_currentIndexChanged(int index);
    void on_addButton_clicked();
};

#endif // SELECTDEPARTMENT_H
