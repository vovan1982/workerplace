#include <QtSql>
#include <QMessageBox>
#include "headers/addexistinguser.h"
#include "headers/delegats.h"

AddExistingUser::AddExistingUser(QWidget *parent, int orgId, int wpId, QList<int> existUserId) :
    QDialog(parent),
    m_orgId(orgId),
    m_wpId(wpId),
    m_existUserId(existUserId)
{
    setupUi(this);
    if(m_wpId > 0)
        dateInWp->setDate(QDate::currentDate());
    else{
        dateInWp->setVisible(false);
        label_2->setVisible(false);
    }
    model = new QSqlRelationalTableModel(userView);
    userView->setModel(model);
    populateCBox("CodPost","post","","<Все>",post);
    populateModel();
    userView->setItemDelegate(new QSqlRelationalDelegate(userView));
    userView->setItemDelegateForColumn(1, new SpinBoxDelegat(0,9999999,1,userView));
    userView->setFocus();
    populateFindIn();
    connect(post,SIGNAL(currentIndexChanged(int)),this,SLOT(populateModel()));
    connect(findButton,SIGNAL(clicked()),this,SLOT(populateModel()));
    connect( (QObject*) userView->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortingClick()));
    if(orgId <= 0)
        groupBox_2->setEnabled(false);
}
void AddExistingUser::populateModel()
{
    QString filter,tmp;
    if(m_orgId > 0)
        filter = QString("CodOrganization = %1").arg(m_orgId);
    else
        filter = "";
    if(post->currentIndex() > 0){
        if(!filter.isEmpty())
            filter.append(" AND ");
        filter.append("relTblAl_6.CodPost=" + post->itemData(post->currentIndex()).toString());
    }
    if(showFilialsUsers->isChecked()){
        if(!filter.isEmpty())
            filter.append(" AND ");
        if(filials->currentIndex() >= 0)
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CODFILPRED = " +
                          filials->itemData(filials->currentIndex()).toString() + ")");
        else
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CODFILPRED = 0)");
    }
    if(showDepartmentsUsers->isChecked()){
        if(!filter.isEmpty())
            filter.append(" AND ");
        if(departments->currentIndex() >= 0)
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CodDepartment = " +
                          departments->itemData(departments->currentIndex()).toString() + ")");
        else
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CodDepartment = 0)");
    }
    if(showFilDepUsers->isChecked()){
        if(!filter.isEmpty())
            filter.append(" AND ");
        if(departments->currentIndex() >= 0)
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CodDepartment = " +
                          departments->itemData(departments->currentIndex()).toString() + ")");
        else
            filter.append("CodUser IN ( SELECT CodUser FROM tounits WHERE CodDepartment = 0)");
    }
    if(showFreeUsers->isChecked()){
        if(!filter.isEmpty())
            filter.append(" AND ");
        filter.append("CODUSER NOT IN ("
                      "SELECT CodUser FROM tounits WHERE "
                      "CODFILPRED IS NOT NULL OR CODDEPARTMENT IS NOT NULL)");
    }
    if(!searchLine->text().isNull() && !searchLine->text().isEmpty()){
        if(findIn->itemData(findIn->currentIndex()).toString() == "CodAD"){
            if(searchLine->text() == "0" || searchLine->text() == "null"){
                if(!filter.isEmpty())
                    filter.append(" AND ");
                filter.append("CodAD IS NULL");
            }else{
                if(!filter.isEmpty())
                    filter.append(" AND ");
                filter.append(findIn->itemData(findIn->currentIndex()).toString() + " = '" +
                              searchLine->text() + "'");
            }
        }else{
            if(!filter.isEmpty())
                filter.append(" AND ");
            filter.append("UCASE( users."+findIn->itemData(findIn->currentIndex()).toString()+" ) REGEXP UCASE( '"+searchLine->text()+"' )");
        }
    }
    if(m_existUserId.size() > 0){
        if(!filter.isEmpty())
            tmp = " AND ";
        else
            tmp = "";
        tmp += "CodUser NOT IN (";
        tmp += QString("%1").arg(m_existUserId[0]);
        for(int i = 1; i < m_existUserId.size(); i++)
            tmp += "," + QString("%1").arg(m_existUserId[i]);
        tmp += ")";
        filter.append(tmp);
    }
    model->clear();
    model->setTable("users");
    model->setFilter(filter);
    model->setSort(5, Qt::AscendingOrder);
    model->setRelation(6,QSqlRelation("post", "CodPost", "Name"));
    model->setHeaderData(1, Qt::Horizontal,tr("Код"));
    model->setHeaderData(2, Qt::Horizontal,tr("Фамилия"));
    model->setHeaderData(3, Qt::Horizontal,tr("Имя"));
    model->setHeaderData(4, Qt::Horizontal,tr("Отчество"));
    model->setHeaderData(5, Qt::Horizontal,tr("Ф.И.О"));
    model->setHeaderData(6, Qt::Horizontal,tr("Должность"));
    model->setHeaderData(7, Qt::Horizontal,tr("Электронный адрес"));
    model->setHeaderData(8, Qt::Horizontal,tr("Дополнительный эл. адрес"));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    userView->setColumnHidden(0,true);
    userView->setColumnHidden(9,true);
    userView->setColumnHidden(10,true);
    userView->setColumnHidden(11,true);
    userView->setColumnHidden(12,true);
    if(model->rowCount() <= 0)
        addButton->setEnabled(false);
    else{
        addButton->setEnabled(true);
        userView->setCurrentIndex(model->index(0,1));
    }
}
void AddExistingUser::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    if(!nullStr.isNull() && !nullStr.isEmpty())
        cBox->addItem(nullStr);
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
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}
void AddExistingUser::populateCBox(const QString &select, const QString &fromWhere, QComboBox *cBox)
{
    QSqlQuery query;
    query.exec(QString("SELECT %1 FROM %2").arg(select).arg(fromWhere));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось заполнить комбобокс %2:\n %1")
                                 .arg(query.lastError().text())
                                 .arg(cBox->objectName()),
                                 tr("Закрыть"));
        return;
    }
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

void AddExistingUser::populateFindIn()
{
    for(int i = 1; i < 9; i++){
        if(i != 6)
            findIn->addItem(model->headerData(i,Qt::Horizontal).toString(),model->record().fieldName(i));
    }
    findIn->setCurrentIndex(4);
}
void AddExistingUser::setAddButtonName(const QString &name)
{
    addButton->setText(name);
}
void AddExistingUser::on_showFilialsUsers_clicked()
{
    departments->clear();
    filials->clear();
    populateCBox("c.id, c.name",
                 QString("departments n, tree t, departments c WHERE n.id = %1"
                         " AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1").arg(m_orgId),
                 filials);
    if(filials->currentIndex() == -1)
        populateModel();
}
void AddExistingUser::on_showAllUsers_clicked()
{
    filials->clear();
    departments->clear();
    populateModel();
}
void AddExistingUser::on_showDepartmentsUsers_clicked()
{
    QSqlQuery query;
    filials->clear();
    departments->clear();
    populateCBox("c.id, c.name",
                 QString("departments n, tree t, departments c WHERE "
                           "n.id IN (SELECT c.id FROM "
                           "(SELECT * FROM departments) n, "
                           "(SELECT * FROM tree) t, "
                           "(SELECT * FROM departments) c "
                            "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.FP != 1 AND t.level >= 1) "
                            "AND n.id = t.parent_id AND t.id = c.id").arg(m_orgId),
                 departments);
    if(departments->count() > 0){
        for(int i=0;i<departments->count();i++){
            query.exec(QString("SELECT c.Name FROM departments n, tree t, departments c "
                               "WHERE n.id = %1 AND n.id = t.id AND t.parent_id = c.id AND c.FP = 1").arg(departments->itemData(i).toInt()));
            if (query.lastError().type() == QSqlError::NoError)
            {
                if(query.size() > 0){
                    query.next();
                    departments->setItemText(i,departments->itemText(i)+"->"+query.value(0).toString());
                }
            }
        }
    }
    if(departments->currentIndex() == -1)
        populateModel();
}
void AddExistingUser::on_showFilDepUsers_clicked()
{
    filials->clear();
    populateCBox("c.id, c.name",
                 QString("departments n, tree t, departments c WHERE n.id = %1"
                         " AND n.id = t.parent_id AND t.id = c.id AND c.FP = 1").arg(m_orgId),
                 filials);
}
void AddExistingUser::on_showFreeUsers_clicked()
{
    filials->clear();
    departments->clear();
    populateModel();
}

void AddExistingUser::on_filials_currentIndexChanged(int index)
{
    if(showFilDepUsers->isChecked()){
        departments->clear();
        populateCBox("c.id, c.name",
                     QString("departments n, tree t, departments c WHERE "
                               "n.id IN (SELECT c.id FROM "
                               "(SELECT * FROM departments) n, "
                               "(SELECT * FROM tree) t, "
                               "(SELECT * FROM departments) c "
                                "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id AND c.FP != 1 AND t.level >= 1) "
                                "AND n.id = t.parent_id AND t.id = c.id").arg(filials->itemData(index).toInt()),
                     departments);
    }
    if(!showAllUsers->isChecked() && !showFreeUsers->isChecked())
        populateModel();
}
void AddExistingUser::on_departments_currentIndexChanged(int)
{
    if(!showAllUsers->isChecked() && !showFreeUsers->isChecked())
        populateModel();
}
void AddExistingUser::on_clearFilterButton_clicked()
{
    searchLine->clear();
    populateModel();
}
void AddExistingUser::sortingClick()
{
    userView->setCurrentIndex(model->index(0,1));
}
void AddExistingUser::on_addButton_clicked()
{
    QSqlQuery query;
    QModelIndex index = userView->currentIndex();
    if(m_wpId > 0){
        query.exec(QString("INSERT INTO workplaceandusers SET CodWorkerPlace = %1, CodUser = %2;")
                   .arg(m_wpId).arg(model->data(model->index(index.row(),0)).toInt()));
        if(query.lastError().type() != QSqlError::NoError){
            qDebug()<<query.lastError().text();
            return;
        }
        query.prepare(QString("INSERT INTO historyuseronwp SET CodUser = ?, CodWorkerPlace = ?, DateIn = ?"));
        query.addBindValue(model->data(model->index(index.row(),0)).toInt());
        query.addBindValue(m_wpId);
        query.addBindValue(dateInWp->date());
        query.exec();
        if (query.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось записать историю прихода на рабочее место:\n %1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
        }
        emit userAdded();
    }else{
        QList<QVariant> datas;
        datas<<model->data(model->index(index.row(),0)).toInt()<<model->data(model->index(index.row(),5)).toString();
        emit userAdded(model->data(model->index(index.row(),5)).toString(), model->data(model->index(index.row(),0)).toInt());
        emit userAdded(datas);
        accept();
    }
    m_existUserId<<model->data(model->index(index.row(),0)).toInt();
    populateModel();
}

void AddExistingUser::changeEvent(QEvent *e)
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
