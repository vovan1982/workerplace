#ifndef CREFERENCEBOOK_H
#define CREFERENCEBOOK_H

#include <QtSql>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include "ui_referencebook.h"

class CReferenceView : public QTableView {
    Q_OBJECT
public:
    CReferenceView(QWidget *parent = 0);
private:
    virtual void resizeEvent(QResizeEvent *event);
};

class CReferenceBook : public QWidget, private Ui::ReferenceBooks {
    Q_OBJECT
    Q_DISABLE_COPY(CReferenceBook)
public:
    explicit CReferenceBook(QWidget *parent = 0);
private:
    bool modelSet, editMode, readOnly;
    QString curReferenceBook;
    QSqlRelationalTableModel *model;
    QSqlRelationalTableModel *model2;
    CReferenceView *tableReference;
    QTimer* timer;
    void updateTable(QString tableName);
    void activeButton(bool act);
    void changeButton(bool ch);
    virtual void keyPressEvent ( QKeyEvent * e );
signals:
    void closeWin(bool);
private slots:
    void dataModelEdit();
    void dataTableChange(int);
    void on_addButton_clicked();
    void on_delButton_clicked();
    void on_submitButton_clicked();
    void on_revertButton_clicked();
    void updateLockReferenceBook();
protected:
    virtual void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // CREFERENCEBOOK_H
