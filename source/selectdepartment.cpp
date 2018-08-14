#include "headers/selectdepartment.h"
#include "headers/sqltreemodel.h"

SelectDepartment::SelectDepartment(QWidget *parent, int orgId) :
    QDialog(parent),
    m_orgId(orgId){
    setupUi(this);

    modelDepart = new SqlTreeModel(treeViewDep);
    modelDepart->setHeaderData(0,Qt::Horizontal,tr("Подразделения"));

    treeViewDep->setModel(modelDepart);
    treeViewDep->setColumnHidden(1,true);
    treeViewDep->setColumnHidden(2,true);
    treeViewDep->setColumnHidden(3,true);
    treeViewDep->setColumnHidden(4,true);
    treeViewDep->setColumnHidden(5,true);

    populateCBox("id","departments","firm = 1",organization);
    if(orgId > 0)
        organization->setCurrentIndex(organization->findData(orgId));
}
void SelectDepartment::populateModDep(int organizationId)
{
    modelDepart->setTable("departments");
    modelDepart->setFilter(QString("id IN (SELECT c.id FROM "
                                   "(SELECT * FROM departments) n, "
                                   "(SELECT * FROM tree) t, "
                                   "(SELECT * FROM departments) c "
                                   "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id) ")
                           .arg(organizationId));
    modelDepart->setHideRoot(true);
    modelDepart->select();
    treeViewDep->expandAll();
}
void SelectDepartment::on_addButton_clicked()
{
    QList<QString> dn;
    QString depName = "";
    QModelIndex index = treeViewDep->currentIndex();
    int i;
    dn<< modelDepart->data(index).toString();
    index = index.parent();
    while(index.isValid()){
        dn<< modelDepart->data(index).toString()+"/";
        index = index.parent();
    }
    i = dn.count();
    while(i>0){
        depName += dn.value(i-1);
        i--;
    }
    emit addDepartment(modelDepart->data(modelDepart->index(treeViewDep->currentIndex().row(),1,
                                                            treeViewDep->currentIndex().parent()))
                       .toInt(),depName,organization->itemData(organization->currentIndex()).toInt());
    SelectDepartment::close();
}
void SelectDepartment::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, QComboBox *cBox)
{
    QSqlQuery query;
    if(filter.isNull() || filter.isEmpty())
        query.exec(QString("SELECT %2,name FROM %1;").arg(tableName).arg(idName));
    else
        query.exec(QString("SELECT %2,name FROM %1 WHERE %3;").arg(tableName).arg(idName).arg(filter));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось заполнить комбобокс %2:\n %1")
                                 .arg(query.lastError().text())
                                 .arg(cBox->objectName()),
                                 tr("Закрыть"));
        return;
    }
    cBox->setMaxCount(query.size()+1);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}
void SelectDepartment::on_organization_currentIndexChanged(int index)
{
    populateModDep(organization->itemData(index).toInt());
    treeViewDep->setCurrentIndex(modelDepart->index(0,0));
}
void SelectDepartment::changeEvent(QEvent *e)
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
