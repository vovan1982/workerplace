#ifndef ADDEDITDEPARTMENTS_H
#define ADDEDITDEPARTMENTS_H

#include "ui_addeditdepartments.h"

class LockDataBase;

class AddEditDepartments : public QDialog, private Ui::AddEditDepartments {
    Q_OBJECT
public:
    explicit AddEditDepartments(QWidget *parent = 0, bool editMode = false, const QString &name = "",
                   const QString &tabName = "", int editId = 0, bool readOnly = false);
private:
    bool m_editMode;
    QString m_name, m_tabName;
    int m_editId;
    bool m_readOnly;
    LockDataBase *lockedControl;
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void newDepartmentAdd(const QString &name, bool multiMode);
private slots:
    void on_addButton_clicked();
    void on_newDepartment_textChanged(QString );
    void on_cancelButton_clicked();
};

#endif // ADDEDITDEPARTMENTS_H
