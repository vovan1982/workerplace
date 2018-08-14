#include <QtSql>
#include "headers/sqltreemodel.h"
#include "headers/workplacemodel.h"
#include "headers/selectworkplace.h"

SelectWorkPlace::SelectWorkPlace(QWidget *parent, int orgId, bool devMode, bool orgTexMode, int wpId) :
    QDialog(parent),
    m_orgId(orgId),
    m_devMode(devMode),
    m_orgTexMode(orgTexMode),
    m_wpId(wpId){
    setupUi(this);

    if(!devMode){
        label->setVisible(false);
        organization->setVisible(false);
        horizontalLayout_3->removeItem(horizontalSpacer_2);
    }

    modelDepart = new SqlTreeModel(treeViewDep);
    modelDepart->setHeaderData(0,Qt::Horizontal,tr("Подразделения"));

    treeViewDep->setModel(modelDepart);
    treeViewDep->setColumnHidden(1,true);
    treeViewDep->setColumnHidden(2,true);
    treeViewDep->setColumnHidden(3,true);
    treeViewDep->setColumnHidden(4,true);
    treeViewDep->setColumnHidden(5,true);

    if(devMode){
        populateCBox("id","departments","firm = 1",organization);
        if(orgId > 0)
            organization->setCurrentIndex(organization->findData(orgId));
    }else{
        populateModDep(orgId);
    }
    treeViewDep->setCurrentIndex(modelDepart->index(0,0));
    wpModel = new WorkPlaceModel(treeViewWp);
    updateWPModel(treeViewDep->currentIndex());

    treeViewWp->setModel(wpModel);
    wpModel->sort(3,Qt::AscendingOrder);
    treeViewWp->setColumnHidden(0,true);
    treeViewWp->setColumnHidden(1,true);
    treeViewWp->setColumnHidden(2,true);
    treeViewWp->setColumnHidden(4,true);
    treeViewWp->setColumnHidden(8,true);
    treeViewWp->setColumnHidden(9,true);
    treeViewWp->setColumnHidden(10,true);
    treeViewWp->setColumnHidden(11,true);
    treeViewWp->resizeColumnToContents(3);
    treeViewWp->resizeColumnToContents(5);
    treeViewWp->setCurrentIndex(wpModel->index(0,3));

    connect(treeViewDep->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updateWPModel(QModelIndex)));
    connect( (QObject*) treeViewWp->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortWPClick()));
}
void SelectWorkPlace::populateModDep(int organizationId)
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

void SelectWorkPlace::populateWPModel(const QString &filter)
{
    wpModel->setFilter(filter);
    wpModel->select();
}
void SelectWorkPlace::on_wpSubsidiaryDep_clicked()
{
    updateWPModel(treeViewDep->currentIndex());
}
void SelectWorkPlace::updateWPModel(const QModelIndex &idx)
{
    QString filter;
    if(idx.isValid()){
        if(wpSubsidiaryDep->isChecked()){
            filter = "CodDepartment = " + modelDepart->data(modelDepart->index(idx.row(),1,idx.parent())).toString();
        }else{
            filter = QString("coddepartment IN ( SELECT c.id FROM "
                             "(SELECT * FROM departments) n, "
                             "(SELECT * FROM tree) t, "
                             "(SELECT * FROM departments) c "
                             "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id)")
                    .arg(modelDepart->data(modelDepart->index(idx.row(),1,idx.parent())).toInt());
        }
        populateWPModel(filter);
        treeViewWp->setCurrentIndex(wpModel->index(0,3));
    }
    if(wpModel->rowCount() <= 0)
        addButton->setEnabled(false);
    else
        addButton->setEnabled(true);
}
void SelectWorkPlace::on_addButton_clicked()
{
    if(m_orgTexMode && (m_wpId == wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt())){
        QMessageBox::information(this, tr("Внимание!!!"),
                                 tr("Невозможно выполнить перемещение\n"
                                    "в указанное рабочее место или склад!!!"),
                                 tr("Закрыть"));
        return;
    }
    QList<QVariant> data;
    QSqlQuery query;
    bool ok;

    ok = query.exec(QString("SELECT departments.id, departments.Name, departments.FP, departments.Firm FROM workerplace "
                            "INNER JOIN departments ON workerplace.CodDepartment = departments.id "
                            "WHERE workerplace.CodWorkerPlace = %1")
                    .arg(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt()));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next();
        data << "dep";
        if(query.value(2).toInt() != 1 && query.value(3).toInt() != 1)
            data << query.value(1).toString();
        else
            data << "";
        data << query.value(0).toInt();
    }else{ data << "dep"; data << ""; data << 0; }

    ok = query.exec(QString("select p.id, p.Name from departments n, tree t, departments p "
                            "where n.id = %1 and n.id = t.id and t.parent_id = p.id AND p.FP = 1;")
                    .arg(data.value(data.indexOf("dep") + 2).toInt()));
    if(!ok){ qDebug()<<query.lastError().text(); return;}
    if(query.size()>0){
        query.next(); data << "fp"; data << query.value(1).toString(); data << query.value(0).toInt();
    }else{ data << "fp"; data << ""; data << 0; }

    if(m_orgId == organization->itemData(organization->currentIndex()).toInt()){
//        ---
        data << "org";
        data << organization->currentText();
        data << m_orgId;

        data << "wpwh";
        data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString();
        data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt();
//        ---
        emit addWorkPlace(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt(),
                          wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString(),
                          m_orgId);
        emit addWorkPlace(data);
        SelectWorkPlace::accept();
    }else{
        if(m_devMode){
            if(m_orgId > 0){
                int button = QMessageBox::question(this,tr("Внимание!!!"),
                                                   tr("Для перемещения выбрана другая фирма!!!\n"
                                                      "Вы действительно хотите выбрать данную фирму?"),
                                                   tr("Да"),tr("Нет"),"",1,1);
                if (button == 1)
                    return;
            }
//        ---
            data << "org";
            data << organization->currentText();
            data << organization->itemData(organization->currentIndex()).toInt();

            data << "wpwh";
            data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString();
            data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt();
//        ---
            emit addWorkPlace(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt(),
                              wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString(),
                              organization->itemData(organization->currentIndex()).toInt());
            emit addWorkPlace(data);
        }else{
            data << "org";
            data << "";
            data << m_orgId;

            data << "wpwh";
            data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString();
            data << wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt();
//        ---
            emit addWorkPlace(wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),0)).toInt(),
                              wpModel->data(wpModel->index(treeViewWp->currentIndex().row(),3)).toString(),
                              m_orgId);
            emit addWorkPlace(data);
        }
        SelectWorkPlace::accept();
    }
}
void SelectWorkPlace::sortWPClick()
{
    treeViewWp->setCurrentIndex(wpModel->index(0,3));
}
void SelectWorkPlace::populateCBox(const QString &idName, const QString &tableName,
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
void SelectWorkPlace::on_organization_currentIndexChanged(int index)
{
    populateModDep(organization->itemData(index).toInt());
    treeViewDep->setCurrentIndex(modelDepart->index(0,0));
}

void SelectWorkPlace::changeEvent(QEvent *e)
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
