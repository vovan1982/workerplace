#include "headers/editandselectproducer.h"
#include "headers/producers.h"

EditAndSelectProducer::EditAndSelectProducer(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    prod = new Producers(this,true);
    layoutProd->addWidget(prod);
}
void EditAndSelectProducer::on_selectButton_clicked()
{
    emit producerSelected(prod->currentItemId());
    this->accept();
}
void EditAndSelectProducer::setVisibleSelectButton(bool visible)
{
    selectButton->setVisible(visible);
}
void EditAndSelectProducer::changeEvent(QEvent *e)
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
