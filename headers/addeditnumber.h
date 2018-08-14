#ifndef ADDEDITNUMBER_H
#define ADDEDITNUMBER_H

#include "ui_addeditnumber.h"

class AddEditNumber : public QDialog, private Ui::AddEditNumber
{
    Q_OBJECT
    
public:
    explicit AddEditNumber(QWidget *parent = 0,
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
    void numberAdded();
    void numberDataChanged();
private slots:
    void on_buttonSave_clicked();
    void on_buttonRevert_clicked();
    void on_codTypeNumber_currentIndexChanged(int);
    void on_number_textChanged();
    void on_note_textChanged();
    void on_editTypeNumber_clicked();
    void updateLockRecord();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // ADDEDITNUMBER_H
