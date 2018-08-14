#ifndef PROVIDERS_H
#define PROVIDERS_H

#include "ui_providers.h"
class QSqlTableModel;

class Providers : public QWidget, private Ui::Providers {
    Q_OBJECT
public:
    explicit Providers(QWidget *parent = 0, bool editMode = false, bool selectMode = false);
private:
    QSqlTableModel *model;
    void populateModel();
protected:
    void changeEvent(QEvent *e);
signals:
    void closeProviderWin();
    void selectedRowData(const QList<QVariant> provider);
private slots:
    void onProvMenu(const QPoint &p);
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

#endif // PROVIDERS_H
