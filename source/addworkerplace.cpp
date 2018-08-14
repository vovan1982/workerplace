#include "headers/addworkerplace.h"

AddWorkerPlace::AddWorkerPlace(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
}
void AddWorkerPlace::clearForm()
{
    name->setText("");
    internalNumber->setText("");
    location->setText("");
    saveButton->setEnabled(false);
}

void AddWorkerPlace::on_name_textEdited(QString text)
{
    if(text.isNull() || text.isEmpty())
        saveButton->setEnabled(false);
    else
        saveButton->setEnabled(true);
}

void AddWorkerPlace::on_saveButton_clicked()
{
    if(editWPAfterSave->isChecked()){
        AddWorkerPlace::accept();
        if(wh->isChecked())
            emit wpAdded(name->text(),internalNumber->text(),location->text(),true,1);
        else
            emit wpAdded(name->text(),internalNumber->text(),location->text(),true,0);
    }
    if(closeWinAfterSave->isChecked()){
        AddWorkerPlace::accept();
        if(wh->isChecked())
            emit wpAdded(name->text(),internalNumber->text(),location->text(),false,1);
        else
            emit wpAdded(name->text(),internalNumber->text(),location->text(),false,0);
    }
    if(addMoreWP->isChecked()){
        if(wh->isChecked())
            emit wpAdded(name->text(),internalNumber->text(),location->text(),false,1);
        else
            emit wpAdded(name->text(),internalNumber->text(),location->text(),false,0);
        clearForm();
    }
}

void AddWorkerPlace::changeEvent(QEvent *e)
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
