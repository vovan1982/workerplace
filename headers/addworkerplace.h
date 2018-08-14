#ifndef ADDWORKERPLACE_H
#define ADDWORKERPLACE_H

#include "ui_addworkerplace.h"

class AddWorkerPlace : public QDialog, private Ui::AddWorkerPlace {
    Q_OBJECT
public:
    AddWorkerPlace(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
    void clearForm();

signals:
    void wpAdded(const QString &wpName, const QString &wpInternalNum, const QString &wpLocation, bool openEdit, int wh);

private slots:
    void on_saveButton_clicked();
    void on_name_textEdited(QString text);
};

#endif // ADDWORKERPLACE_H
