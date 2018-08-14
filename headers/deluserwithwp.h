#ifndef DELUSERWITHWP_H
#define DELUSERWITHWP_H

#include "ui_deluserwithwp.h"

class DelUserWithWp : public QDialog, private Ui::DelUserWithWp
{
    Q_OBJECT
    
public:
    explicit DelUserWithWp(QWidget *parent = 0);
    
protected:
    void changeEvent(QEvent *e);
signals:
    void userDateOut(const QString &date);
private slots:
    void on_buttonAccept_clicked();
};

#endif // DELUSERWITHWP_H
