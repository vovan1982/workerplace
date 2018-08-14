#ifndef EDITANDSELECTPRODUCER_H
#define EDITANDSELECTPRODUCER_H

#include "ui_editandselectproducer.h"

class Producers;

class EditAndSelectProducer : public QDialog, private Ui::EditAndSelectProducer
{
    Q_OBJECT
public:
    explicit EditAndSelectProducer(QWidget *parent = 0);
    void setVisibleSelectButton(bool visible);
private:
    Producers *prod;
signals:
    void producerSelected(int idProd);
private slots:
    void on_selectButton_clicked();
protected:
    void changeEvent(QEvent *e);
};

#endif // EDITANDSELECTPRODUCER_H
