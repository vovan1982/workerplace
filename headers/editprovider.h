#ifndef EDITPROVIDER_H
#define EDITPROVIDER_H

#include "ui_editprovider.h"

class Providers;

class editProvider : public QDialog, private Ui::editProvider {
    Q_OBJECT
public:
    editProvider(QWidget *parent = 0);
private:
    Providers *prov;
protected:
    void changeEvent(QEvent *e);
};

#endif // EDITPROVIDER_H
