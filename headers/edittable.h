#ifndef CEDITTABLE_H
#define CEDITTABLE_H

#include "ui_edittable.h"

class CeditTableView;
class QSqlTableModel;

class CeditTable : public QDialog, private Ui::editTable {
    Q_OBJECT
public:
    explicit CeditTable(QWidget *parent = 0, QString tName = "", bool selectMode = false);
private:
    QString m_tName;
    bool editMode, m_selectMode, readOnly;
    QSqlTableModel *model;
    CeditTableView *tableView;
    QTimer* timer;
    void changeButton(bool ch);
    virtual void keyPressEvent ( QKeyEvent * e );
signals:
    void selectedRowData(const QList<QVariant> datas);
private slots:
    void on_addButton_clicked();
    void on_delButton_clicked();
    void on_submitButton_clicked();
    void on_revertButton_clicked();
    void dataModelEdit();
    void on_selectButton_clicked();
    void updateLockReferenceBook();
protected:
    virtual void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // CEDITTABLE_H
