#ifndef ADDEDITUSERS_H
#define ADDEDITUSERS_H

#include "ui_addeditusers.h"

class LockDataBase;

class AddEditUsers : public QDialog, private Ui::AddEditUsers
{
    Q_OBJECT
    
public:
    explicit AddEditUsers(QWidget *parent = 0, bool wpMode = false, const QMap<QString,QVariant> &datas = QMap<QString,QVariant>(), bool editMode = false);
private:
    bool m_wpMode;
    QMap<QString,QVariant> m_data;
    bool m_editMode;
    LockDataBase *lockedControl;
    bool dataEntered();
    bool formIsEmpty();
    bool dataChanged();
    void checkData(bool checkDataEntered = false);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void setDefaultData();
signals:
    void userAdded();
    void userDataChanged();
private slots:
    void clearForm();
    void on_codFirm_currentIndexChanged(int);
    void on_lastName_textChanged();
    void on_name_textChanged();
    void on_middleName_textChanged();
    void on_codPost_currentIndexChanged(int);
    void on_fioSummary_textChanged();
    void on_email_textChanged();
    void on_additionalemail_textChanged();
    void on_codAD_textChanged(const QString &);
    void on_buttonSave_clicked();
    void on_buttonRevert_clicked();
    void on_buttonEditPost_clicked();
    void on_buttonPopulateFio_clicked();
    void on_note_textChanged(const QString &);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // ADDEDITUSERS_H
