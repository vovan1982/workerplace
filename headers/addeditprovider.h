#ifndef ADDEDITPROVIDER_H
#define ADDEDITPROVIDER_H

#include <QSqlRecord>
#include "ui_addeditprovider.h"

class LockDataBase;

class AddEditProvider : public QDialog, private Ui::AddEditProvider {
    Q_OBJECT
public:
    AddEditProvider(QWidget *parent = 0, bool editMode = false, QSqlRecord rec = QSqlRecord(), bool readOnly = false);
private:
    bool m_editMode;
    QSqlRecord m_rec;
    bool m_readOnly;
    LockDataBase *lockedControl;
    bool formIsEmpty();
    void clearForm();
    bool dataChanged();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void providerAdded();
private slots:
    void on_saveButton_clicked();
    void on_revertButton_clicked();
    void on_name_textEdited(QString text);
    void on_adress_textEdited();
    void on_fax_textEdited();
    void on_phone_textEdited();
    void on_email_textEdited();
    void on_www_textEdited();
};

#endif // ADDEDITPROVIDER_H
