#ifndef EDITPO_H
#define EDITPO_H

#include "ui_editpo.h"

class Po;

class EditPo : public QDialog, private Ui::EditPo
{
    Q_OBJECT
public:
    explicit EditPo(QWidget *parent = 0, int editId = 0);
private:
    Po *ep;
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // EDITPO_H
