#ifndef ADDEDITFIRM_H
#define ADDEDITFIRM_H

#include <QtSql>
#include "ui_addeditfirm.h"

class AddEditFirm : public QDialog, private Ui::AddEditFirm {
    Q_OBJECT
public:
    AddEditFirm(QWidget *parent = 0, const QString &tabName = "", bool editMode = false, int editId = 0, bool readOnly = false);
private:
    QString m_tabName, curName;
    bool m_editMode;
    int m_editId;
    bool m_readOnly;
    QTimer* timer;
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void newFirmAdd(const QString &name, int id);
    void editFirm(const QString &newName);
private slots:
    void on_saveButton_clicked();
    void on_firmName_textEdited(QString );
    void on_cancelButton_clicked();
    void updateLockRecord();
};

#endif // ADDEDITFIRM_H
