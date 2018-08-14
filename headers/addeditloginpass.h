#ifndef ADDEDITLOGINPASS_H
#define ADDEDITLOGINPASS_H

#include "ui_addeditloginpass.h"

class AddEditLoginPass : public QDialog, private Ui::AddEditLoginPass
{
    Q_OBJECT

public:
    explicit AddEditLoginPass(QWidget *parent = 0,
                              const QMap<QString,QVariant> &datas = QMap<QString,QVariant>(),
                              bool editMode = false, bool readOnly = false);
private:
    QMap<QString,QVariant> m_data;
    bool m_editMode, m_readOnly;
    QTimer* timer;
    void clearForm();
    bool dataEntered();
    bool formIsEmpty();
    bool dataChanged();
    void checkData(bool checkDataEntered = false);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void setDefaultData();
signals:
    void loginAdded();
    void loginDataChanged();
private slots:
    void on_buttonSave_clicked();
    void on_buttonRevert_clicked();
    void on_codLoginType_currentIndexChanged(int);
    void on_login_textChanged();
    void on_pass_textChanged();
    void on_note_textChanged();
    void on_editLoginType_clicked();
    void updateLockRecord();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // ADDEDITLOGINPASS_H
