#ifndef CONNECTDB_H
#define CONNECTDB_H

#include <QtSql>
#include <QMessageBox>
#include "ui_connectdb.h"

class connectDB : public QDialog, private Ui::connectDB {
    Q_OBJECT
    Q_DISABLE_COPY(connectDB)
public:
    explicit connectDB(QWidget *parent = 0);
protected:
    virtual void changeEvent(QEvent *e);
private slots:
    void on_connect_DB_clicked();
};

#endif // CONNECTDB_H
