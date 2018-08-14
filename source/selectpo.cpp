#include "headers/editpo.h"
#include "headers/selectpo.h"
#include "headers/pomodel.h"

SelectPo::SelectPo(QWidget *parent, const QString &filter, bool selectOnlyPo, bool showEditButton) :
    QDialog(parent),
    m_selectOnlyPo(selectOnlyPo)
{
    setupUi(this);
    curFilter = "";
    poModel = new PoModel(viewPo);
    viewPo->setModel(poModel);
    populatePoModel(filter);
    for(int i = 1;i<poModel->columnCount();i++)
        viewPo->setColumnHidden(i,true);
    viewPo->setCurrentIndex(poModel->index(0,0,QModelIndex()));
    buttonEditPo->setVisible(showEditButton);
}
void SelectPo::populatePoModel(const QString &filter)
{
    curFilter = filter;
    poModel->setFilter(filter);
    poModel->select();
    poModel->setHeaderData(0, Qt::Horizontal,tr("Наименование"));
}
void SelectPo::on_selectButton_clicked()
{
    QList<QVariant> po;
    for(int i = 0;i<poModel->columnCount();i++){
        po<<poModel->data(poModel->index(viewPo->currentIndex().row(),i,viewPo->currentIndex().parent()));
    }
    if(!m_selectOnlyPo){
        emit itemSelected(poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt());
        emit itemSelected(po);
        this->accept();
    }else{
        if(poModel->data(poModel->index(viewPo->currentIndex().row(),3,viewPo->currentIndex().parent())).toInt() == 0){
            emit itemSelected(poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt());
            emit itemSelected(po);
            this->accept();
        }
    }
}
void SelectPo::on_buttonEditPo_clicked()
{
    int curId = poModel->data(poModel->index(viewPo->currentIndex().row(),1,viewPo->currentIndex().parent())).toInt();
    EditPo *ep = new EditPo(this,curId);
    ep->exec();
    populatePoModel(curFilter);
    viewPo->setCurrentIndex(poModel->findData(curId));
}
void SelectPo::changeEvent(QEvent *e)
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
