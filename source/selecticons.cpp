#include <QDir>
#include "headers/selecticons.h"
#include "headers/filelistmodel.h"

SelectIcons::SelectIcons(QWidget *parent, const QString &dirIcon) :
    QDialog(parent)
{
    setupUi(this);
    model = new FileListModel(iconView);
    model->setHideFileName(false);
    model->setDirPath(QDir::currentPath()+dirIcon);
    iconView->setModel(model);
    iconView->setIconSize(QSize(32,32));
    iconView->setCurrentIndex(model->index(0));
}
void SelectIcons::on_selectButton_clicked()
{
    emit iconSelected(model->data(iconView->currentIndex(),Qt::EditRole).toString());
    this->accept();
}
void SelectIcons::changeEvent(QEvent *e)
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
