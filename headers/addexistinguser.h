#ifndef ADDEXISTINGUSER_H
#define ADDEXISTINGUSER_H

#include "ui_addexistinguser.h"

class QSqlRelationalTableModel;

class AddExistingUser : public QDialog, private Ui::AddExistingUser {
    Q_OBJECT
public:
    AddExistingUser(QWidget *parent = 0, int orgId = 0, int wpId = 0, QList<int> existUserId = QList<int>());
    void setAddButtonName(const QString &name);
private:
    int m_orgId, m_wpId;
    QList<int> m_existUserId;
    QSqlRelationalTableModel *model;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void populateCBox(const QString &select, const QString &fromWhere, QComboBox *cBox);
    void populateFindIn();
signals:
    void userAdded();
    void userAdded(const QString &userName, int userId);
    void userAdded(const QList<QVariant> datas);
private slots:
    void on_addButton_clicked();
    void on_clearFilterButton_clicked();
    void on_departments_currentIndexChanged(int);
    void on_showAllUsers_clicked();
    void on_showFreeUsers_clicked();
    void on_filials_currentIndexChanged(int index);
    void on_showFilDepUsers_clicked();
    void on_showDepartmentsUsers_clicked();
    void on_showFilialsUsers_clicked();
    void populateModel(/*int postFilter, int orgFilter*/);
    void sortingClick();
protected:
    void changeEvent(QEvent *e);
};

#endif // ADDEXISTINGUSER_H
