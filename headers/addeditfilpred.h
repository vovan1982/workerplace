#ifndef ADDEDITFILPRED_H
#define ADDEDITFILPRED_H

#include <QtSql>
#include "ui_addeditfilpred.h"

class AddEditFilPred : public QDialog, private Ui::AddEditFilPred {
    Q_OBJECT
public:
    AddEditFilPred(QWidget *parent = 0, const QString &tabName = "", int parentId = 0, bool editMode = false, int editId = 0, bool readOnly = false);
private:
    QString m_tabName, curName;
    int m_parentId;
    bool m_editMode;
    int m_editId;
    bool m_readOnly;
    QTimer* timer;
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void newFilPredAdd(const QString &name, int id);
    void editFilPred(const QString &newName);
private slots:
    void on_saveButton_clicked();
    void on_filpredName_textEdited(QString );
    void on_cancelButton_clicked();
    void updateLockRecord();
};

#endif // ADDEDITFILPRED_H
