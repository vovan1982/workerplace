#ifndef ADDEDITPRODUCER_H
#define ADDEDITPRODUCER_H

#include <QSqlRecord>
#include "ui_addeditproducer.h"

class AddEditProducer : public QDialog, private Ui::AddEditProducer {
    Q_OBJECT
public:
    explicit AddEditProducer(QWidget *parent = 0, bool editMode = false, QSqlRecord rec = QSqlRecord(), bool readOnly = false);
private:
    bool m_editMode;
    QSqlRecord m_rec;
    bool m_readOnly;
    QTimer* timer;
    bool formIsEmpty();
    void clearForm();
    bool dataChanged();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void producerAdded();
private slots:
    void on_saveButton_clicked();
    void on_revertButton_clicked();
    void on_name_textEdited(QString text);
    void on_www_textEdited();
    void on_note_textEdited();
    void updateLockRecord();
};

#endif // ADDEDITPRODUCER_H
