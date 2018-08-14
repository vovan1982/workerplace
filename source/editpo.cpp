#include <QtWidgets>
#include "headers/po.h"
#include "headers/editpo.h"

EditPo::EditPo(QWidget *parent, int editId) :
    QDialog(parent)
{
    setupUi(this);
    ep = new Po(this,editId);
    ep->showCloseButton(false);
    poLayout->addWidget(ep);
}

void EditPo::changeEvent(QEvent *e)
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
void EditPo::closeEvent(QCloseEvent *event)
{
    ep->close();
    event->accept();
}
