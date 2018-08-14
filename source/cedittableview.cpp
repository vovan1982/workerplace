#include "headers/cedittableview.h"

CeditTableView::CeditTableView(QWidget *parent) : QTableView(parent)
{
}
void CeditTableView::resizeEvent(QResizeEvent *event)
{
    resizeRowsToContents();
    QTableView::resizeEvent(event);
}
