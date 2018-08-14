#include "headers/movetreeitem.h"

MoveTreeItem::MoveTreeItem(QWidget *parent, const QString &nameItem, int itemId, int itemParentId,
                           const QString &tableName, int countRows, bool delMode, bool filP, int filPId,
                           bool moveDep, bool moveWp) :
    QDialog(parent),
    m_itemRowCount(countRows),
    m_moveDep(moveDep),
    m_moveWp(moveWp),
    m_delMode(delMode){
    setupUi(this);
    tabName  = tableName;
    if(moveDep){
        if (filP){
            filter1 = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id "
                              "AND c.id NOT IN (SELECT c.id FROM (SELECT * FROM %4) n, (SELECT * FROM tree) t, (SELECT * FROM %4) c "
                              "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id) "
                              "AND c.id != %2 AND c.id != %3")
                    .arg(filPId)
                    .arg(itemId)
                    .arg(itemParentId)
                    .arg(tableName);
            filter2 = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id "
                              "AND c.id NOT IN (SELECT c.id FROM (SELECT * FROM %3) n, (SELECT * FROM tree) t, (SELECT * FROM %3) c "
                              "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id) "
                              "AND c.id != %2")
                    .arg(filPId)
                    .arg(itemId)
                    .arg(tableName);
        }else{
            filter1 = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id "
                              "AND c.id NOT IN ( "
                              "SELECT c.id FROM "
                              "(SELECT * FROM %4) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %4) c "
                              "WHERE n.id IN "
                              "(SELECT c.id FROM "
                              "(SELECT * FROM %4) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %4) c "
                              "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1) "
                              "AND n.id = t.parent_id AND t.id = c.id)"
                              "AND c.id NOT IN (SELECT c.id FROM (SELECT * FROM %4) n, (SELECT * FROM tree) t, (SELECT * FROM %4) c "
                              "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id) "
                              "AND c.id != %2 AND c.id != %3")
                    .arg(filPId)
                    .arg(itemId)
                    .arg(itemParentId)
                    .arg(tableName);
            filter2 = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id "
                              "AND c.id NOT IN ( "
                              "SELECT c.id FROM "
                              "(SELECT * FROM %3) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %3) c "
                              "WHERE n.id IN "
                              "(SELECT c.id FROM "
                              "(SELECT * FROM %3) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %3) c "
                              "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1) "
                              "AND n.id = t.parent_id AND t.id = c.id)"
                              "AND c.id NOT IN (SELECT c.id FROM (SELECT * FROM %3) n, (SELECT * FROM tree) t, (SELECT * FROM %3) c "
                              "WHERE  n.id = %2 AND n.id = t.parent_id AND t.id = c.id) "
                              "AND c.id != %2")
                    .arg(filPId)
                    .arg(itemId)
                    .arg(tableName);
        }

        movedItem->setText(nameItem);
        curFilter = filter1;
        modelDep = new QSqlQueryModel(newParents);
        populateModel(curFilter, modelDep);
        if (!delMode && countRows == 0)
            moveVariants->setEnabled(false);
        if (delMode && countRows > 0){
            moveVariants->setEnabled(false);
            onlyChildrenItem->setChecked(true);
        }
        if (delMode && countRows == 0)
            moveVariants->setEnabled(false);

        newParents->setModel(modelDep);
        newParents->setColumnHidden(0,true);
        newParents->setColumnHidden(1,true);
        newParents->setColumnHidden(3,true);
        newParents->setColumnHidden(4,true);
        newParents->setColumnHidden(5,true);
        newParents->setColumnHidden(6,true);
        newParents->setColumnHidden(7,true);
        newParents->setCurrentIndex(modelDep->index(0,0));

        if(modelDep->rowCount(QModelIndex())==0){
            if(countRows > 0){
                onlyChildrenItem->setChecked(true);
                newParents->setCurrentIndex(modelDep->index(0,0));
            }else{
                moveButton->setEnabled(false);
                newParents->setEnabled(false);
            }
            lineFilter->setEnabled(false);
            findButton->setEnabled(false);
            moveVariants->setEnabled(false);
        }
        if(!moveWp){
            lineFilterWp->setVisible(false);
            findWpButton->setVisible(false);
            groupBox_2->setVisible(false);
            delWp->setVisible(false);
            delDep->setVisible(false);
            MoveTreeItem::adjustSize();
        }
    }
    if(moveWp){
        if (filP){
            filterWp = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.id != %2")
                    .arg(filPId)
                    .arg(itemId);
        }else{
            filterWp = QString("WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id "
                              "AND c.id NOT IN ( "
                              "SELECT c.id FROM "
                              "(SELECT * FROM %3) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %3) c "
                              "WHERE n.id IN "
                              "(SELECT c.id FROM "
                              "(SELECT * FROM %3) n, "
                              "(SELECT * FROM tree) t, "
                              "(SELECT * FROM %3) c "
                              "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1) "
                              "AND n.id = t.parent_id AND t.id = c.id) AND c.id != %2")
                    .arg(filPId)
                    .arg(itemId)
                    .arg(tableName);
        }
        label->setText(tr("Рабочее место"));
        movedItem->setText(nameItem);
        curFilterWp = filterWp;
        modelWp = new QSqlQueryModel(newParentsWp);
        populateModel(curFilterWp, modelWp);

        newParentsWp->setModel(modelWp);
        newParentsWp->setColumnHidden(0,true);
        newParentsWp->setColumnHidden(1,true);
        newParentsWp->setColumnHidden(3,true);
        newParentsWp->setColumnHidden(4,true);
        newParentsWp->setCurrentIndex(modelWp->index(0,0));
        if(modelWp->rowCount() == 0){
            moveButton->setEnabled(false);
            newParentsWp->setEnabled(false);
            lineFilterWp->setEnabled(false);
            findWpButton->setEnabled(false);
        }
        if(!moveDep){
            lineFilter->setVisible(false);
            findButton->setVisible(false);
            groupBox->setVisible(false);
            delWp->setVisible(false);
            delDep->setVisible(false);
            moveVariants->setVisible(false);
            MoveTreeItem::adjustSize();
        }
    }
}

void MoveTreeItem::populateModel(const QString &filters, QSqlQueryModel *model)
{
    model->setQuery(QString("SELECT c.* FROM %2 n, tree t, %2 c %1 ORDER BY 2").arg(filters).arg(tabName));
    model->setHeaderData(2, Qt::Horizontal,tr("Подразделения"));
}

void MoveTreeItem::changeEvent(QEvent *e)
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

void MoveTreeItem::on_findButton_clicked()
{
    if(!lineFilter->text().isNull() && !lineFilter->text().isEmpty()){
        QString newFilter = curFilter;
        newFilter += QString(" AND c.name REGEXP '%1'").arg(lineFilter->text());
        populateModel(newFilter, modelDep);
    }else
        populateModel(curFilter, modelDep);
    newParents->setCurrentIndex(modelDep->index(0,0));
}

void MoveTreeItem::on_findWpButton_clicked()
{
    if(!lineFilterWp->text().isNull() && !lineFilterWp->text().isEmpty()){
        QString newFilter = curFilterWp;
        newFilter += QString(" AND c.name REGEXP '%1'").arg(lineFilterWp->text());
        populateModel(newFilter, modelWp);
    }else
        populateModel(curFilterWp, modelWp);
    newParentsWp->setCurrentIndex(modelWp->index(0,0));
}

void MoveTreeItem::on_moveButton_clicked()
{
    //  Режимы moveMode в newParentItem
    //  0 - Перемещение текущего подразделения
    //  1 - Перемещение дочерних подразделений с удалением текущего
    //  2 - Удаление текущего подразделения и всех дочерних

    //  Режимы moveWpMode в newParentItemWp
    //  0 - Перемещение даного рабочего места
    //  1 - Перемещение всех рабочих мест данного подразделения
    //  2 - Переместить все рабочие места текущего и дочерних подразделений
    //  3 - Удаление всех рабочих мест данного подразделения и дочерних подразделений в случае их удаления

    if(m_moveDep && m_moveWp){
        int newParent = modelDep->data(modelDep->index(newParents->currentIndex().row(),0)).toInt();
        int newParentWp = modelWp->data(modelWp->index(newParentsWp->currentIndex().row(),0)).toInt();
        if(fullItem->isChecked())
            emit newParentItem(newParent, 0);
        if(onlyChildrenItem->isChecked()){
            if(delDep->isChecked())
                emit newParentItem(newParent, 2);
            else
                emit newParentItem(newParent, 1);
        }
        if(m_delMode){
            if(delWp->isChecked())
                emit newParentItemWp(newParentWp,3);
            else if(delDep->isChecked())
                emit newParentItemWp(newParentWp,2);
            else
                emit newParentItemWp(newParentWp,1);
        }else
            emit newParentItemWp(newParentWp,0);
    }
    if(!m_moveDep && m_moveWp){
        int newParentWp = modelWp->data(modelWp->index(newParentsWp->currentIndex().row(),0)).toInt();
        if(m_delMode){
            emit newParentItemWp(newParentWp,1);
        }else
            emit newParentItemWp(newParentWp,0);
    }
    if(m_moveDep && !m_moveWp){
        int newParent = modelDep->data(modelDep->index(newParents->currentIndex().row(),0)).toInt();
        if(fullItem->isChecked())
            emit newParentItem(newParent, 0);
        if(onlyChildrenItem->isChecked()){
            emit newParentItem(newParent, 1);
        }
    }
    MoveTreeItem::accept();
}

void MoveTreeItem::on_onlyChildrenItem_toggled(bool checked)
{
    if(checked){
        curFilter = filter2;
        lineFilter->setText("");
        populateModel(curFilter, modelDep);
        newParents->setCurrentIndex(modelDep->index(0,0));
    }else{
        curFilter = filter1;
        lineFilter->setText("");
        populateModel(curFilter, modelDep);
        newParents->setCurrentIndex(modelDep->index(0,0));
    }
}
