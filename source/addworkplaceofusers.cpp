#include <QtSql>
#include "headers/addworkplaceofusers.h"
#include "headers/sqltreemodel.h"
#include "headers/workplacemodel.h"

AddWorkplaceOfUsers::AddWorkplaceOfUsers(QWidget *parent, SqlTreeModel *modelDep, int userId) :
    QDialog(parent),
    modelDepart(modelDep),
    usId(userId){
    setupUi(this);
    dateInWp->setDate(QDate::currentDate());
    treeViewDep->setModel(modelDep);
    for(int i = 1;i<modelDep->columnCount();i++)
        treeViewDep->setColumnHidden(i,true);
    treeViewDep->expandAll();
    treeViewDep->setCurrentIndex(modelDepart->index(0,0));

    wpModel = new WorkPlaceModel(treeViewWp);
    updateWPModel(treeViewDep->currentIndex());

    treeViewWp->setModel(wpModel);
    wpModel->sort(3,Qt::AscendingOrder);
    for(int i = 0;i<wpModel->columnCount();i++){
        if(i != 3 && (i < 5 || i > 7))
            treeViewWp->setColumnHidden(i,true);
    }
    treeViewWp->resizeColumnToContents(3);
    treeViewWp->resizeColumnToContents(5);
    treeViewWp->setCurrentIndex(wpModel->index(0,3));

    connect(treeViewDep->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updateWPModel(QModelIndex)));
    connect( (QObject*) treeViewWp->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortWPClick()));
}
void AddWorkplaceOfUsers::populateWPModel(const QString &filter)
{
    wpModel->setFilter(filter);
    wpModel->select();
}
void AddWorkplaceOfUsers::on_wpSubsidiaryDep_clicked()
{
    updateWPModel(treeViewDep->currentIndex());
}
void AddWorkplaceOfUsers::updateWPModel(const QModelIndex &idx)
{
    QString filter;
    if(idx.isValid()){
        if(wpSubsidiaryDep->isChecked()){
            filter = "CodDepartment = " + modelDepart->data(modelDepart->index(idx.row(),1,idx.parent())).toString();
            filter.append(QString(" AND CodWorkerPlace NOT IN (SELECT CodWorkerPlace FROM workplaceandusers WHERE CodUser = %1)").arg(usId));
        }else{
            filter = QString("coddepartment IN ( SELECT c.id FROM "
                             "departments n, tree t, departments c "
                             "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id)")
                    .arg(modelDepart->data(modelDepart->index(idx.row(),1,idx.parent())).toInt());
            filter.append(QString(" AND CodWorkerPlace NOT IN (SELECT CodWorkerPlace FROM workplaceandusers WHERE CodUser = %1)").arg(usId));
        }
        populateWPModel(filter);
        treeViewWp->setCurrentIndex(wpModel->index(0,3));
    }
    if(wpModel->rowCount() <= 0)
        addButton->setEnabled(false);
    else
        addButton->setEnabled(true);
}
void AddWorkplaceOfUsers::on_addButton_clicked()
{
    QSqlQuery query;
    query.exec(QString("INSERT INTO workplaceandusers VALUES (%1, %2)")
               .arg(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt())
               .arg(usId));
    if(query.lastError().type() != QSqlError::NoError){
        qDebug()<<query.lastError().text();
        return;
    }
    query.prepare(QString("INSERT INTO historyuseronwp SET CodUser = ?, CodWorkerPlace = ?, DateIn = ?"));
    query.addBindValue(usId);
    query.addBindValue(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt());
    query.addBindValue(dateInWp->date());
    query.exec();
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось записать историю прихода на рабочее место:\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
    }
    emit addWorkPlace();
    updateWPModel(treeViewDep->currentIndex());
}
void AddWorkplaceOfUsers::sortWPClick()
{
    treeViewWp->setCurrentIndex(wpModel->index(0,3));
}
void AddWorkplaceOfUsers::changeEvent(QEvent *e)
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
