#ifndef PRODUCERS_H
#define PRODUCERS_H

#include "ui_producers.h"
class QSqlTableModel;

class Producers : public QWidget, private Ui::Producers {
    Q_OBJECT
public:
    explicit Producers(QWidget *parent = 0, bool editMode = false, bool selectMode = false);
    int currentItemId();
private:
    QSqlTableModel *model;
    void populateModel();
protected:
    void changeEvent(QEvent *e);
signals:
    void closeProducerWin();
    void selectedRowData(const QList<QVariant> producer);
private slots:
    void onProdMenu(const QPoint &p);
    void on_delButton_clicked();
    void on_editButton_clicked();
    void on_closeButton_clicked();
    void on_addButton_clicked();
    void updateView();
    void on_selectButton_clicked();
    void on_actionAdd_triggered();
    void on_actionDel_triggered();
    void on_actionEdit_triggered();
};

#endif // PRODUCERS_H
