#include "headers/editprovider.h"
#include "headers/providers.h"

editProvider::editProvider(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    prov = new Providers(this,true);
    layoutProv->addWidget(prov);
}

void editProvider::changeEvent(QEvent *e)
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
