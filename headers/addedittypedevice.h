#ifndef ADDEDITTYPEDEVICE_H
#define ADDEDITTYPEDEVICE_H

#include "ui_addedittypedevice.h"
class QSqlTableModel;
class LockDataBase;

class AddEditTypeDevice : public QDialog, private Ui::AddEditTypeDevice {
    Q_OBJECT
public:
    AddEditTypeDevice(QWidget *parent = 0,const QString &tName = "", int type = 1, bool allType = false);
private:
    bool editMode, readOnly;
    QString m_tName;
    int m_type;
    QSqlTableModel *model;
    LockDataBase *lockedControl;
    void changeButton(bool ch);
    virtual void keyPressEvent ( QKeyEvent * e );
signals:
    void closeTypeDeviceWin();
private slots:
    void on_closeButton_clicked();
    void on_addButton_clicked();
    void on_delButton_clicked();
    void on_submitButton_clicked();
    void on_revertButton_clicked();
    void on_delIcoButton_clicked();
    void dataModelEdit();
    void on_buttonEditIcon_clicked();
    void setCurentIcon(const QString &icoFile);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // ADDEDITTYPEDEVICE_H
