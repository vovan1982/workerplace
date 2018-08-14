#include <QtSql>
#include <QStandardItemModel>
#include <QMessageBox>
#include "headers/journalhistoryusersonwp.h"
#include "headers/addexistinguser.h"
#include "headers/selectworkplace.h"

JournalHistoryUsersOnWP::JournalHistoryUsersOnWP(QWidget *parent, int codUser, int codWP) :
    QWidget(parent)
{
    setupUi(this);
    curFilter = "";
    historyModel = new QStandardItemModel(0,7,this);
    historyModel->setHeaderData(1, Qt::Horizontal,tr("Пользователь"));
    historyModel->setHeaderData(3, Qt::Horizontal,tr("Рабочее место"));
    historyModel->setHeaderData(4, Qt::Horizontal,tr("Дата прихода"));
    historyModel->setHeaderData(5, Qt::Horizontal,tr("Дата ухода"));

    view->setModel(historyModel);
    view->setColumnHidden(0,true);
    view->setColumnHidden(2,true);
    view->setColumnHidden(6,true);

    if(codUser > 0 || codWP > 0){
        if(codUser > 0 && codWP <= 0){
            initModel(QString("historyuseronwp.CodUser = %1").arg(codUser));
            if(historyModel->rowCount() > 0){
                label_3->setVisible(false);
                organizationFilter->setVisible(false);
                populateCBox("id","departments","firm = 1","<Все>",organizationFilter);
                organizationFilter->setCurrentIndex(organizationFilter->findData(historyModel->index(0,6).data().toInt()));
                userFilter->setText(historyModel->index(0,1).data().toString());
                userFilter->setData(codUser);
                userFilter->setEnabled(false);
            }else
                boxFilter->setVisible(false);
        }else if(codUser <= 0 && codWP > 0){
            initModel(QString("historyuseronwp.CodWorkerPlace = %1").arg(codWP));
            if(historyModel->rowCount() > 0){
                label_3->setVisible(false);
                organizationFilter->setVisible(false);
                populateCBox("id","departments","firm = 1","<Все>",organizationFilter);
                organizationFilter->setCurrentIndex(organizationFilter->findData(historyModel->index(0,6).data().toInt()));
                wpFilter->setText(historyModel->index(0,3).data().toString());
                wpFilter->setData(codWP);
                wpFilter->setEnabled(false);
            }else
                boxFilter->setVisible(false);
        }else{
            boxFilter->setVisible(false);
            initModel(QString("historyuseronwp.CodUser = %1 AND historyuseronwp.CodWorkerPlace = %1").arg(codUser).arg(codWP));
        }
    }else{
        initModel();
        populateCBox("id","departments","firm = 1","<Все>",organizationFilter);
        connect(organizationFilter,SIGNAL(currentIndexChanged(int)),this,SLOT(updateHistoryModel()));
    }
    view->resizeColumnToContents(1);
    view->resizeColumnToContents(3);
}

void JournalHistoryUsersOnWP::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void JournalHistoryUsersOnWP::initModel(const QString &filter)
{
    QString queryStr, curFilter;
    QSqlQuery query;
    bool ok;

    curFilter = filter;

    if(curFilter.isEmpty())
        queryStr = QString("SELECT historyuseronwp.CodUser"
                           ", users.FIOSummary"
                           ", historyuseronwp.CodWorkerPlace"
                           ", workerplace.Name"
                           ", historyuseronwp.DateIn"
                           ", historyuseronwp.DateOut"
                           ", users.CodOrganization "
                           "FROM historyuseronwp "
                           "INNER JOIN users "
                           "ON historyuseronwp.CodUser = users.CodUser "
                           "INNER JOIN workerplace "
                           "ON historyuseronwp.CodWorkerPlace = workerplace.CodWorkerPlace");
    else
        queryStr = QString("SELECT historyuseronwp.CodUser"
                           ", users.FIOSummary"
                           ", historyuseronwp.CodWorkerPlace"
                           ", workerplace.Name"
                           ", historyuseronwp.DateIn"
                           ", historyuseronwp.DateOut"
                           ", users.CodOrganization "
                           "FROM historyuseronwp "
                           "INNER JOIN users "
                           "ON historyuseronwp.CodUser = users.CodUser "
                           "INNER JOIN workerplace "
                           "ON historyuseronwp.CodWorkerPlace = workerplace.CodWorkerPlace "
                           "WHERE %1").arg(curFilter);
    ok = query.exec(queryStr);
    if(!ok){ qDebug()<<query.lastError().text()<<"\n"<<query.lastQuery(); return;}

    historyModel->removeRows(0,historyModel->rowCount());
    if(query.size() > 0){
        while(query.next()){
            historyModel->insertRow(historyModel->rowCount());
            for(int j = 0; j<historyModel->columnCount();j++){
                historyModel->setData(historyModel->index(historyModel->rowCount()-1,j),query.value(j));
            }
        }
    }
    if(historyModel->rowCount() > 0)
        view->setCurrentIndex(historyModel->index(0,1));
}

void JournalHistoryUsersOnWP::updateHistoryModel()
{
    QString filter = "";
    if(organizationFilter->currentIndex() > 0){
        filter += QString("users.CodOrganization = %1").arg(organizationFilter->itemData(organizationFilter->currentIndex()).toInt());
    }
    if(!userFilter->text().isEmpty()){
        if(!filter.isEmpty())
            filter += " AND ";
        filter += QString("historyuseronwp.CodUser = %1").arg(userFilter->data().toInt());
    }
    if(!wpFilter->text().isEmpty()){
        if(!filter.isEmpty())
            filter += " AND ";
        filter += QString("historyuseronwp.CodWorkerPlace = %1").arg(wpFilter->data().toInt());
    }
    initModel(filter);
}

void JournalHistoryUsersOnWP::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    cBox->addItem(nullStr,0);
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

void JournalHistoryUsersOnWP::setFilterUser(const QString &userName, int userId)
{
    userFilter->setText(userName);
    userFilter->setData(userId);
    updateHistoryModel();
}

void JournalHistoryUsersOnWP::setFilterWP(int wpId, const QString &wpName)
{
    wpFilter->setText(wpName);
    wpFilter->setData(wpId);
    updateHistoryModel();
}

void JournalHistoryUsersOnWP::on_userFilter_runButtonClicked()
{
    AddExistingUser *aeu = new AddExistingUser(this,organizationFilter->itemData(organizationFilter->currentIndex()).toInt());
    aeu->setWindowTitle(tr("Выбор пользователя"));
    aeu->setAddButtonName(tr("Выбрать"));
    connect(aeu,SIGNAL(userAdded(QString,int)),this,SLOT(setFilterUser(QString,int)));
    aeu->exec();
}

void JournalHistoryUsersOnWP::on_userFilter_clearButtonClicked()
{
    updateHistoryModel();
}

void JournalHistoryUsersOnWP::on_wpFilter_runButtonClicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this,organizationFilter->itemData(organizationFilter->currentIndex()).toInt(),
                                               (organizationFilter->currentIndex()<=0));
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(setFilterWP(int,QString)));
    swp->setAttribute(Qt::WA_DeleteOnClose);
    swp->exec();
}

void JournalHistoryUsersOnWP::on_wpFilter_clearButtonClicked()
{
    updateHistoryModel();
}

void JournalHistoryUsersOnWP::on_buttonClose_clicked()
{
    emit closeJhuWin();
}

//void JournalHistoryUsersOnWP::on_organizationFilter_currentIndexChanged()
//{
//    updateHistoryModel();
//}
