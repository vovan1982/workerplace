#include <QtWidgets>
#include "headers/deluserwithwp.h"

DelUserWithWp::DelUserWithWp(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    dateOut->setDate(QDate::currentDate());
}

void DelUserWithWp::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void DelUserWithWp::on_buttonAccept_clicked()
{
    emit userDateOut(dateOut->date().toString(Qt::ISODate));
    accept();
}
