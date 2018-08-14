#ifndef ADDWORKPLACEOFUSERS_H
#define ADDWORKPLACEOFUSERS_H

#include "ui_addworkplaceofusers.h"

class SqlTreeModel;
//class QSqlTableModel;
class WorkPlaceModel;

class AddWorkplaceOfUsers : public QDialog, private Ui::AddWorkplaceOfUsers {
    Q_OBJECT
public:
    AddWorkplaceOfUsers(QWidget *parent = 0, SqlTreeModel *modelDep = 0, int userId = 0);
private:
    SqlTreeModel *modelDepart;
    int usId;
    WorkPlaceModel *wpModel;
    void populateWPModel(const QString &filter = "");
protected:
    void changeEvent(QEvent *e);
signals:
    void addWorkPlace();
private slots:
    void on_addButton_clicked();
    void on_wpSubsidiaryDep_clicked();
    void updateWPModel(const QModelIndex &idx = QModelIndex());
    void sortWPClick();
};

#endif // ADDWORKPLACEOFUSERS_H
