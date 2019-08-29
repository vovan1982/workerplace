#include <QtWidgets>
#include <QtSql>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QSortFilterProxyModel>
#include "headers/users.h"
#include "headers/sqltreemodel.h"
#include "headers/addworkplaceofusers.h"
#include "headers/addexistinguser.h"
#include "headers/workplacemodel.h"
#include "headers/addeditusers.h"
#include "headers/addeditloginpass.h"
#include "headers/addeditnumber.h"
#include "headers/lockdatabase.h"
#include "headers/journalhistoryusersonwp.h"
#include "headers/deluserwithwp.h"

Cusers::Cusers(QWidget *parent, bool wpMode, int firmId, int wpId, bool readOnly) :
    QWidget(parent), m_parent(parent), m_wpMode(wpMode), m_firmId(firmId), m_wpId(wpId), m_readOnly(readOnly)
{
    setupUi(this);
    if (wpMode){
        groupBoxPost->setVisible(false);
        groupBoxOrg->setVisible(false);
        closeButton->setVisible(false);
        horizontalLayout_7->removeItem(horizontalSpacer_3);
        addWPButton->setVisible(false);
        delWPButton->setVisible(false);
        lineEditOrganization->setVisible(false);
        label_15->setVisible(false);
        groupBoxUsers->setTitle(tr("Список пользователей"));
    }else{
        setPrimaryUserButton->setVisible(false);
        addButton->setVisible(false);
    }
    populateCBox("CodPost","post","","<Все>",post);
    populateCBox("id","departments","firm = 1","<Все>",organization);

    QActionGroup *agLoginPass = new QActionGroup(this);
    QActionGroup *agNumber = new QActionGroup(this);
    QActionGroup *agWorkPlace = new QActionGroup(this);
    agLoginPass->setExclusive(true);
    agNumber->setExclusive(true);
    agWorkPlace->setExclusive(true);
    agLoginPass->addAction(actionAddLoginPass);
    agLoginPass->addAction(actionDelLoginPass);
    agLoginPass->addAction(actionEditLoginPass);
    agNumber->addAction(actionAddNumber);
    agNumber->addAction(actionDelNumber);
    agNumber->addAction(actionEditNumber);
    agWorkPlace->addAction(actionAddWP);
    agWorkPlace->addAction(actionDelWP);

    menuLoginPass = new QMenu(tr("Логины и пароли"), this);
    menuNumber = new QMenu(tr("Телефонные номера"), this);
    menuWorkPlace = new QMenu(tr("Рабочие места"), this);
    menuLoginPass->addActions(agLoginPass->actions());
    menuNumber->addActions(agNumber->actions());
    menuWorkPlace->addActions(agWorkPlace->actions());

    model = new QSqlRelationalTableModel(users);
    nullModel = new QSqlRelationalTableModel();
    proxyModel = new QSortFilterProxyModel(this);
    loginModel = new QSqlRelationalTableModel(loginPassView);
    numberModel = new QSqlRelationalTableModel(numberView);
    modelFilPredDep = new SqlTreeModel(departmentView);
    toUnitModel = new QSqlTableModel(this);
    wpModel = new WorkPlaceModel(wpView);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setFilterKeyColumn(5);
    numberView->setModel(numberModel);
    loginPassView->setModel(loginModel);
    wpView->setModel(wpModel);
    wpModel->sort(3,Qt::AscendingOrder);
    for(int i = 0;i<wpModel->columnCount();i++){
        if(i != 3 && (i < 5 || i > 7))
            wpView->setColumnHidden(i,true);
    }
    wpView->resizeColumnToContents(3);
    wpView->resizeColumnToContents(5);
    modelFilPredDep->setHeaderData(0,Qt::Horizontal,tr("Подразделения"));
    users->setModel(proxyModel);
    populateModel(-1, -1);
    users->setItemDelegate(new QSqlRelationalDelegate(users));
    loginPassView->setItemDelegate(new QSqlRelationalDelegate(loginPassView));
    numberView->setItemDelegate(new QSqlRelationalDelegate(numberView));

    model->relationModel(9)->setFilter("firm = 1");

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(lastName, 2);
    mapper->addMapping(name, 3);
    mapper->addMapping(middleName, 4);
    mapper->addMapping(fioSummary, 5);
    mapper->addMapping(codAD, 1);
    mapper->addMapping(lineEditPost, 6);
    mapper->addMapping(email, 7);
    mapper->addMapping(additionalemail, 8);
    mapper->addMapping(lineEditOrganization, 9);
    mapper->addMapping(note, 10);

    departmentView->setModel(modelFilPredDep);

    populateLoginModel(0);
    populateNumberModel(0);

    connect(findUser, SIGNAL(textEdited(QString)),this,SLOT(setProxyFilter(QString)));
    connect(users->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateMaperIndex(QModelIndex)));
    connect(users->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(loginPass()));
    connect(users->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(numberViewUpdate()));
    connect(users->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(wpViewUpdate()));
    connect(users->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(filPredDep()));

    connect( (QObject*) users->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortClick()));
    connect( (QObject*) loginPassView->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortLoginClick()));
    connect( (QObject*) numberView->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortNumberClick()));
    connect( (QObject*) wpView->header(), SIGNAL (sectionClicked (int)), this, SLOT(sortWPClick()));
    connect(post,SIGNAL(activated(int)),this, SLOT(setUserFilter()));
    connect(organization,SIGNAL(activated(int)),this, SLOT(setUserFilter()));

    connect(loginPassView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onMenu(const QPoint &)));
    connect(numberView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onMenu(const QPoint &)));
    connect(wpView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onMenu(const QPoint &)));

    connect(loginPassView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_buttonEditLoginPass_clicked()));
    connect(numberView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_buttonEditNumber_clicked()));

    if(model->rowCount() > 0)
        users->setCurrentIndex(proxyModel->index(0, 5));
    else{
        populateWPModel(0);
        buttonHistory->setEnabled(false);
    }
    for(int i = 1;i<modelFilPredDep->columnCount();i++)
        departmentView->setColumnHidden(i,true);
    connect(departmentView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(changeDepIndex(QModelIndex)));
    if(readOnly)
        setReadOnly(readOnly);
    else{
        if(model->rowCount() <= 0)
            userModelEmpty(false);
        else
            userModelEmpty(true);
    }

    QList<int> spsize;
    spsize<<users->columnWidth(5)/2;spsize<<30;
    splitter->setSizes(spsize);
}
void Cusers::userModelEmpty(bool ok)
{
    setPrimaryUserButton->setEnabled(ok);
    delUser->setEnabled(ok);
    buttonEdit->setEnabled(ok);
    buttonAddLoginPass->setEnabled(ok);
    actionAddLoginPass->setEnabled(ok);
    buttonAddNumber->setEnabled(ok);
    actionAddNumber->setEnabled(ok);
    groupBoxDep->setEnabled(ok);
    addWPButton->setEnabled(ok);
    actionAddWP->setEnabled(ok);
    buttonHistory->setEnabled(ok);
    if(ok && loginModel->rowCount() > 0){
        buttonDelLoginPass->setEnabled(ok);
        actionDelLoginPass->setEnabled(ok);
        buttonEditLoginPass->setEnabled(ok);
        actionEditLoginPass->setEnabled(ok);
    }else if(ok && loginModel->rowCount() <= 0){
        buttonDelLoginPass->setEnabled(false);
        actionDelLoginPass->setEnabled(false);
        buttonEditLoginPass->setEnabled(false);
        actionEditLoginPass->setEnabled(false);
    }else if(!ok){
        buttonDelLoginPass->setEnabled(ok);
        actionDelLoginPass->setEnabled(ok);
        buttonEditLoginPass->setEnabled(ok);
        actionEditLoginPass->setEnabled(ok);
    }
    if(ok && numberModel->rowCount() > 0){
        buttonDelNumber->setEnabled(ok);
        actionDelNumber->setEnabled(ok);
        buttonEditNumber->setEnabled(ok);
        actionEditNumber->setEnabled(ok);
    }else if(ok && numberModel->rowCount() <= 0){
        buttonDelNumber->setEnabled(false);
        actionDelNumber->setEnabled(false);
        buttonEditNumber->setEnabled(false);
        actionEditNumber->setEnabled(false);
    }else if(!ok){
        buttonDelNumber->setEnabled(ok);
        actionDelNumber->setEnabled(ok);
        buttonEditNumber->setEnabled(ok);
        actionEditNumber->setEnabled(ok);
    }
}
void Cusers::setReadOnly(bool ok)
{
    userModelEmpty(!ok);
    addButton->setDisabled(ok);
    buttonAddUser->setDisabled(ok);
    if(proxyModel->rowCount() > 0)
        buttonHistory->setEnabled(true);
}
void Cusers::setDateOut(const QString &date)
{
    dateOut = date;
}
void Cusers::onMenu(const QPoint &p)
{
    if(sender()->objectName() == "loginPassView")
        menuLoginPass->exec(loginPassView->viewport()->mapToGlobal(p));
    if(sender()->objectName() == "numberView")
        menuNumber->exec(numberView->viewport()->mapToGlobal(p));
    if(sender()->objectName() == "wpView")
        menuWorkPlace->exec(wpView->viewport()->mapToGlobal(p));
}
void Cusers::sortClick()
{
    users->setCurrentIndex(proxyModel->index(0, 5));
}
void Cusers::sortLoginClick()
{
    loginPassView->setCurrentIndex(loginModel->index(0, 2));
}
void Cusers::sortNumberClick()
{
    numberView->setCurrentIndex(numberModel->index(0, 2));
}
void Cusers::sortWPClick()
{
    wpView->setCurrentIndex(wpModel->index(0,3));
}
void Cusers::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
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

void Cusers::populateModel(int postFilter, int orgFilter)
{
    proxyModel->setSourceModel(new QSqlRelationalTableModel());
    model->clear();
    model->setTable("users");
    if(m_wpMode)
        model->setFilter(QString("CodUser IN (SELECT CodUser FROM workplaceandusers WHERE CodWorkerPlace = %1)").arg(m_wpId));
    else{
        if (postFilter != -1 && orgFilter == -1)
            model->setFilter(QString("relTblAl_6.CodPost='%1'").arg(postFilter));
        if (postFilter == -1 && orgFilter != -1)
            model->setFilter(QString("relTblAl_9.id='%1'").arg(orgFilter));
        if (postFilter != -1 && orgFilter != -1)
            model->setFilter(QString("relTblAl_9.id='%1' AND relTblAl_6.CodPost='%2'").arg(orgFilter).arg(postFilter));
    }
    model->setHeaderData(5, Qt::Horizontal,tr("Пользователи"));
    model->setSort(5, Qt::AscendingOrder);
    model->setRelation(6,QSqlRelation("post", "CodPost", "Name"));
    model->setRelation(9,QSqlRelation("departments", "id", "Name"));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    proxyModel->setSourceModel(model);
    for(int i = 0; i<model->columnCount();i++)
        if (i == 5)
            continue;
        else
            users->setColumnHidden(i,true);
}

void Cusers::populateLoginModel(int filter)
{
    loginModel->clear();
    loginModel->setTable("loginpass");
    loginModel->setFilter(QString("CodLoginPass='%1'").arg(filter));
    loginModel->setRelation(2,QSqlRelation("logintype", "CodLoginType", "Name"));
    loginModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    loginModel->setHeaderData(2, Qt::Horizontal,tr("Тип логина"));
    loginModel->setHeaderData(3, Qt::Horizontal,tr("Логин"));
    loginModel->setHeaderData(4, Qt::Horizontal,tr("Пароль"));
    loginModel->setHeaderData(5, Qt::Horizontal,tr("Примечание"));
    loginModel->select();
    loginPassView->setColumnHidden(0,true);
    loginPassView->setColumnHidden(1,true);
}
void Cusers::populateNumberModel(int filter)
{
    numberModel->clear();
    numberModel->setTable("listnumber");
    numberModel->setFilter(QString("CodUser='%1'").arg(filter));
    numberModel->setRelation(2,QSqlRelation("typenumber", "CodTypeNumber", "Name"));
    numberModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    numberModel->setHeaderData(2, Qt::Horizontal,tr("Тип номера"));
    numberModel->setHeaderData(3, Qt::Horizontal,tr("Номер"));
    numberModel->setHeaderData(4, Qt::Horizontal,tr("Примечание"));
    numberModel->select();
    numberView->setColumnHidden(0,true);
    numberView->setColumnHidden(1,true);
}
void Cusers::populateWPModel(int filter)
{
    wpModel->setFilter(QString("CodWorkerPlace IN (SELECT CodWorkerPlace FROM workplaceandusers WHERE CodUser = %1)").arg(filter));
    wpModel->select();
}
void Cusers::populateToUnitModel(int filter)
{
    toUnitModel->clear();
    toUnitModel->setTable("tounits");
    toUnitModel->setFilter(QString("CodUser='%1'").arg(filter));
    toUnitModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    toUnitModel->select();
}

void Cusers::setUserFilter()
{
    QModelIndex curViewIndex = users->currentIndex();
    if(post->currentIndex() > 0 && organization->currentIndex() <= 0){
        populateModel(post->itemData(post->currentIndex()).toInt(), -1);
    }else if(post->currentIndex() <= 0 && organization->currentIndex() > 0){
        populateModel(-1,organization->itemData(organization->currentIndex()).toInt());
    }else if(post->currentIndex() > 0 && organization->currentIndex() > 0){
        populateModel(post->itemData(post->currentIndex()).toInt(),organization->itemData(organization->currentIndex()).toInt());
    }else
        populateModel(-1,-1);
    model->relationModel(9)->setFilter("firm = 1");
    if(model->rowCount() == 0){
        clearUserForm();
        userModelEmpty(false);
    }else{
        userModelEmpty(true);
        if(curViewIndex.isValid())
            users->setCurrentIndex(proxyModel->index(curViewIndex.row(),5));
        else
            users->setCurrentIndex(proxyModel->index(0,5));
    }
}

void Cusers::clearUserForm()
{
    lastName->setText("");
    name->setText("");
    middleName->setText("");
    fioSummary->setText("");
    lineEditOrganization->setText("");
    lineEditPost->setText("");
    email->setText("");
    additionalemail->setText("");
    codAD->setText("");
    note->setText("");
    populateLoginModel(-1);
    populateNumberModel(-1);
    wpViewUpdate();
}

void Cusers::updateMaperIndex(const QModelIndex &index)
{
    if(index.isValid()){
        model->revertAll();
        mapper->setCurrentIndex(realModelIndex(index).row());
    }
}
void Cusers::loginPass()
{
    QModelIndex curViewIndex = users->currentIndex();
    if(curViewIndex.isValid()){
        populateLoginModel(model->data(model->index(realModelIndex(curViewIndex).row(),0)).toInt());
        loginPassView->setCurrentIndex(loginModel->index(0, 2));
    }else
        populateLoginModel(0);
    if(loginModel->rowCount() == 0){
        buttonDelLoginPass->setEnabled(false);
        actionDelLoginPass->setEnabled(false);
        buttonEditLoginPass->setEnabled(false);
        actionEditLoginPass->setEnabled(false);
    }else{
        buttonDelLoginPass->setEnabled(true);
        actionDelLoginPass->setEnabled(true);
        buttonEditLoginPass->setEnabled(true);
        actionEditLoginPass->setEnabled(true);
    }
}
void Cusers::numberViewUpdate()
{
    QModelIndex curViewIndex = users->currentIndex();
    if(curViewIndex.isValid()){
        populateNumberModel(model->data(model->index(realModelIndex(curViewIndex).row(),0)).toInt());
        numberView->setCurrentIndex(numberModel->index(0, 2));
    }else
        populateNumberModel(0);
    if(numberModel->rowCount() <= 0){
        buttonDelNumber->setEnabled(false);
        actionDelNumber->setEnabled(false);
        buttonEditNumber->setEnabled(false);
        actionEditNumber->setEnabled(false);
    }else{
        buttonDelNumber->setEnabled(true);
        actionDelNumber->setEnabled(true);
        buttonEditNumber->setEnabled(true);
        actionEditNumber->setEnabled(true);
    }
}

void Cusers::wpViewUpdate()
{
    if(proxyModel->rowCount() > 0){
        buttonHistory->setEnabled(true);
        if(users->currentIndex().isValid()){
            populateWPModel(model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt());
            wpView->setCurrentIndex(wpModel->index(0,3));
            wpView->resizeColumnToContents(2);
            wpView->resizeColumnToContents(3);
        }else
            populateWPModel(0);
    }else{
        populateWPModel(0);
        buttonHistory->setEnabled(false);
    }
    if(wpModel->rowCount() == 0){
        delWPButton->setEnabled(false);
        actionDelWP->setEnabled(false);
    }else{
        if(!m_readOnly){
            delWPButton->setEnabled(true);
            actionDelWP->setEnabled(true);
        }else{
            delWPButton->setEnabled(false);
            actionDelWP->setEnabled(false);
        }
    }
}

bool Cusers::isLockedOrChanged()
{
    if(!m_readOnly){
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->recordIsLosked(model->index(realModelIndex(users->currentIndex()).row(),0).data().toInt(),
                                          "CodUser","users")){
            if(lockedControl->lastError().type() != QSqlError::NoError){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
                return false;
            }
        }else{
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Запись заблокированна пользователем: %1")
                                     .arg(lockedControl->recordBlockingUser()),
                                     tr("Закрыть"));
            return true;
        }
    }else
        return true;

    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT RV FROM users WHERE CodUser = %1")
                    .arg(model->index(realModelIndex(users->currentIndex()).row(),0).data().toInt()));
    if(ok){
        query.next();
        if(model->index(realModelIndex(users->currentIndex()).row(),13).data().toInt() != query.value(0).toInt()){
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Запись была изменена, будут загружены новые данные записи.\n"
                                        "Повторите свой выбор."),
                                     tr("Закрыть"));
            setUserFilter();
            return true;
        }
    }else
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось получить информацию о версии записи:\n %1\n")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
    return false;
}

void Cusers::on_buttonAddUser_clicked()
{
    QMap<QString,QVariant> data;
    data["firmId"] = m_firmId;
    data["wpId"] = m_wpId;
    AddEditUsers *adduser = new AddEditUsers(this,m_wpMode,data);
    connect(adduser,SIGNAL(userAdded()),this,SLOT(setUserFilter()));
    adduser->exec();
}

void Cusers::on_delUser_clicked()
{
    QSqlQuery query;
    DelUserWithWp *duww = 0;
    if(!m_wpMode){
        int button;
        button = QMessageBox::question(this,tr("Удаление пользователя"),
                                      tr("Вы действительно хотите удалить указанного пользователя?"),
                                      tr("Да"),tr("Нет"),"",1,1);
        if(button == 1)
            return;
        QModelIndex index;
        index = realModelIndex(users->currentIndex());
        model->removeRow(index.row());
        model->submitAll();
        if(model->rowCount() > 0){
            if (index.row() != model->rowCount())
                users->setCurrentIndex(model->index(realViewIndex(index).row(), 5));
            else
                users->setCurrentIndex(model->index(realViewIndex(index).row()-1, 5));
            users->setFocus();
        }else{
            userModelEmpty(false);
            clearUserForm();
        }
    }else{
        int userId = model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt();
        dateOut = "";
        duww = new DelUserWithWp(this);
        connect(duww,SIGNAL(userDateOut(QString)),this,SLOT(setDateOut(QString)));
        if(!duww->exec())
            return;
        query.exec(QString("DELETE FROM workplaceandusers WHERE CodUser = %1 AND CodWorkerPlace = %2")
                   .arg(userId)
                   .arg(m_wpId));
        if(query.lastError().type() != QSqlError::NoError){
            QMessageBox::warning(this, tr("Ошибка !!!"),
                                 tr("Не удалось удалить привязку пользователя к рабочему месту:")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(!dateOut.isEmpty()){
            query.prepare(QString("INSERT INTO historyuseronwp SET CodUser = ?, CodWorkerPlace = ?, DateOut = ?"));
            query.addBindValue(userId);
            query.addBindValue(m_wpId);
            query.addBindValue(dateOut);
            query.exec();
            if (query.lastError().type() != QSqlError::NoError)
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Не удалось записать историю ухода с рабочего места:\n %1")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
            }
        }else
            QMessageBox::information(this, tr("Ошибка"),tr("Отсутствует дата ухода пользователя с раборего места."),tr("Закрыть"));
        setUserFilter();
        query.exec(QString("SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1").arg(m_wpId));
        if(query.lastError().type() != QSqlError::NoError){
            QMessageBox::warning(this, tr("Ошибка !!!"),
                                 tr("Не удалось получить информацию о привязке пользователя:")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(query.size() > 0){
            query.next();
            if(query.value(0).toInt() == userId){
                query.exec(QString("UPDATE workerplace SET PrimaryUser = NULL WHERE CodWorkerPlace = %1").arg(m_wpId));
                if(query.lastError().type() != QSqlError::NoError){
                    QMessageBox::warning(this, tr("Ошибка !!!"),
                                         tr("Не удалось очистить основного пользователя рабочего места:")
                                         .arg(query.lastError().text()),
                                         tr("Закрыть"));
                    return;
                }
            }
        }
        emit updatePrimaryUser();
    }
}

void Cusers::on_buttonEdit_clicked()
{
    if(isLockedOrChanged()) return;
    QModelIndex curViewIndex = realModelIndex(users->currentIndex());
    QMap<QString,QVariant> data;
    data["codUser"] = model->index(curViewIndex.row(),0).data();
    data["firmId"] = organization->itemData(organization->findText(model->index(curViewIndex.row(),9).data().toString()));
    data["wpId"] = m_wpId;
    data["lastName"] = model->index(curViewIndex.row(),2).data();
    data["name"] = model->index(curViewIndex.row(),3).data();
    data["middleName"] = model->index(curViewIndex.row(),4).data();
    data["fioSummary"] = model->index(curViewIndex.row(),5).data();
    data["codAD"] = model->index(curViewIndex.row(),1).data();
    data["codPost"] = post->itemData(post->findText(model->index(curViewIndex.row(),6).data().toString()));
    data["email"] = model->index(curViewIndex.row(),7).data();
    data["additionalemail"] = model->index(curViewIndex.row(),8).data();
    data["note"] = model->index(curViewIndex.row(),10).data();
    data["rv"] = model->index(curViewIndex.row(),13).data();
    AddEditUsers *editUser = new AddEditUsers(this,m_wpMode,data,true);
    editUser->setWindowTitle(tr("Редактирование пользователя"));
    connect(editUser,SIGNAL(userDataChanged()),this,SLOT(setUserFilter()));
    editUser->setAttribute(Qt::WA_DeleteOnClose);
    editUser->exec();
}

void Cusers::on_buttonAddLoginPass_clicked()
{
    QMap<QString,QVariant> data;
    data["userName"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),5)).toString();
    data["userId"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt();
    AddEditLoginPass *addLogin = new AddEditLoginPass(this,data);
    connect(addLogin,SIGNAL(loginAdded()),this,SLOT(loginPass()));
    addLogin->setAttribute(Qt::WA_DeleteOnClose);
    addLogin->exec();
}

void Cusers::on_buttonDelLoginPass_clicked()
{
    int button;
    button = QMessageBox::question(this,tr("Удаление логина и пароля"),
                                  tr("Вы действительно хотите удалить указаный логин и пароль?"),
                                  tr("Да"),tr("Нет"),"",1,1);
    if(button == 1)
        return;
    QModelIndex index;
    index = loginPassView->currentIndex();
    loginModel->removeRow(index.row());
    loginModel->submitAll();
    if (index.row() != loginModel->rowCount())
        loginPassView->setCurrentIndex(loginModel->index(index.row(), 2));
    else
        loginPassView->setCurrentIndex(loginModel->index(index.row()-1, 2));
    loginPassView->setFocus();
    if(loginModel->rowCount() == 0){
        buttonDelLoginPass->setEnabled(false);
        actionDelLoginPass->setEnabled(false);
        buttonEditLoginPass->setEnabled(false);
        actionEditLoginPass->setEnabled(false);
    }
}

void Cusers::on_buttonEditLoginPass_clicked()
{
    if(isLockedOrChanged()) return;
    QSqlQuery query;
    QMap<QString,QVariant> data;
    data["userName"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),5)).toString();
    data["userId"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt();
    data["login"] = loginModel->data(loginModel->index(loginPassView->currentIndex().row(),3)).toString();
    data["pass"] = loginModel->data(loginModel->index(loginPassView->currentIndex().row(),4)).toString();
    data["note"] = loginModel->data(loginModel->index(loginPassView->currentIndex().row(),5)).toString();
    data["rv"] = model->index(realModelIndex(users->currentIndex()).row(),13).data();
    if(!query.exec(QString("SELECT CodLoginType FROM loginpass WHERE `key` = %1")
                   .arg(loginModel->data(loginModel->index(loginPassView->currentIndex().row(),0)).toInt()))){
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось получить код типа логина:\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
        data["codLoginType"] = 0;
    }else{
        if(query.next())
            data["codLoginType"] = query.value(0).toInt();
        else{
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось получить код типа логина.\nЗапрос пуст."),
                                 tr("Закрыть"));
            data["codLoginType"] = 0;
        }
    }
    AddEditLoginPass *editLogin = new AddEditLoginPass(this,data,true,isLockedOrChanged());
    editLogin->setWindowTitle(tr("Редактирование логина и пароля"));
    connect(editLogin,SIGNAL(loginDataChanged()),this,SLOT(setUserFilter()));
    editLogin->setAttribute(Qt::WA_DeleteOnClose);
    editLogin->exec();
}

void Cusers::on_buttonAddNumber_clicked()
{
    QMap<QString,QVariant> data;
    data["userName"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),5)).toString();
    data["userId"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt();
    AddEditNumber *addNumber = new AddEditNumber(this,data);
    connect(addNumber,SIGNAL(numberAdded()),this,SLOT(numberViewUpdate()));
    addNumber->setAttribute(Qt::WA_DeleteOnClose);
    addNumber->exec();
}

void Cusers::on_buttonDelNumber_clicked()
{
    int button;
    button = QMessageBox::question(this,tr("Удаление номера"),
                                  tr("Вы действительно хотите удалить указаный номер?"),
                                  tr("Да"),tr("Нет"),"",1,1);
    if(button == 1)
        return;
    QModelIndex index;
    index = numberView->currentIndex();
    numberModel->removeRow(index.row());
    numberModel->submitAll();
    if (index.row() != numberModel->rowCount())
        numberView->setCurrentIndex(numberModel->index(index.row(), 2));
    else
        numberView->setCurrentIndex(numberModel->index(index.row()-1, 2));
    numberView->setFocus();
    if(numberModel->rowCount() == 0){
        buttonDelNumber->setEnabled(false);
        actionDelNumber->setEnabled(false);
        buttonEditNumber->setEnabled(false);
        actionEditNumber->setEnabled(false);
    }
}

void Cusers::on_buttonEditNumber_clicked()
{
    if(isLockedOrChanged()) return;
    QSqlQuery query;
    QMap<QString,QVariant> data;
    data["userName"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),5)).toString();
    data["userId"] = model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt();
    data["number"] = numberModel->data(numberModel->index(numberView->currentIndex().row(),3)).toString();;
    data["note"] = numberModel->data(numberModel->index(numberView->currentIndex().row(),4)).toString();;
    data["rv"] = model->index(realModelIndex(users->currentIndex()).row(),13).data();
    if(!query.exec(QString("SELECT CodTypeNumber FROM listnumber WHERE `key` = %1")
                   .arg(numberModel->data(numberModel->index(numberView->currentIndex().row(),0)).toInt()))){
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось получить код типа номера:\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
        data["codTypeNumber"] = 0;
    }else{
        if(query.next())
            data["codTypeNumber"] = query.value(0).toInt();
        else{
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось получить код типа номера.\nЗапрос пуст."),
                                 tr("Закрыть"));
            data["codTypeNumber"] = 0;
        }
    }
    AddEditNumber *addNumber = new AddEditNumber(this,data,true,isLockedOrChanged());
    addNumber->setWindowTitle(tr("Редактирование номера"));
    connect(addNumber,SIGNAL(numberDataChanged()),this,SLOT(setUserFilter()));
    addNumber->setAttribute(Qt::WA_DeleteOnClose);
    addNumber->exec();
}

void Cusers::on_closeButton_clicked()
{
    emit closeUserWin(true);
}

QModelIndex Cusers::realModelIndex(const QModelIndex &viewIndex) const
{
    return proxyModel->mapToSource(viewIndex);
}

QModelIndex Cusers::realViewIndex(const QModelIndex &modelIndex) const
{
    return proxyModel->mapFromSource(modelIndex);
}

void Cusers::on_setPrimaryUserButton_clicked()
{
    emit primaryUserSet(model->data(model->index(realModelIndex(users->currentIndex()).row(),5)).toString(),
            model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt());
}

void Cusers::filPredDep()
{
    QSqlQuery query;
    QModelIndex curViewIndex = users->currentIndex();
    if(proxyModel->rowCount() > 0){
        if(curViewIndex.isValid()){
            query.exec(QString("SELECT CodOrganization FROM users WHERE CodUser = %1")
                       .arg(model->data(model->index(realModelIndex(curViewIndex).row(),0)).toInt()));
            populateToUnitModel(model->data(model->index(realModelIndex(curViewIndex).row(),0)).toInt());
        }else{
            query.exec(QString("SELECT CodOrganization FROM users WHERE CodUser = 0"));
            populateToUnitModel(0);
        }
    }else
        query.exec(QString("SELECT CodOrganization FROM users WHERE CodUser = 0"));
    if(query.lastError().type() != QSqlError::NoError){
        qDebug()<<query.lastError().text();
        return;
    }
    modelFilPredDep->setTable("departments");
    if(query.size()>0){
        query.next();
        modelFilPredDep->setFilter(QString("id IN (SELECT c.id FROM "
                                           "departments n, tree t, departments c "
                                           "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id) ")
                                   .arg(query.value(0).toInt()));
    }else{
        modelFilPredDep->setFilter(QString("id IN (SELECT c.id FROM "
                                           "departments n, tree t, departments c "
                                           "WHERE n.id = 0 AND n.id = t.parent_id AND t.id = c.id) "));
    }
    modelFilPredDep->setHideRoot(true);
    modelFilPredDep->select();
    buttonSaveDep->setEnabled(false);
    departmentView->expandAll();

    if(toUnitModel->rowCount() > 0){
        if(!toUnitModel->data(toUnitModel->index(0,2)).isNull()){
            groupBoxDep->setChecked(true);
            depBoxCurState = true;
            depCurIndex = modelFilPredDep->findData(toUnitModel->data(toUnitModel->index(0,2)).toInt());
            departmentView->setCurrentIndex(depCurIndex);
        }else if(!toUnitModel->data(toUnitModel->index(0,1)).isNull()){
            groupBoxDep->setChecked(true);
            depBoxCurState = true;
            depCurIndex = modelFilPredDep->findData(toUnitModel->data(toUnitModel->index(0,1)).toInt());
            departmentView->setCurrentIndex(depCurIndex);
        }else{
            groupBoxDep->setChecked(false);
            depBoxCurState = false;
            depCurIndex = QModelIndex();
        }
    }else{
        groupBoxDep->setChecked(false);
        depBoxCurState = false;
        depCurIndex = QModelIndex();
    }
}

void Cusers::changeDepIndex(QModelIndex ind)
{
    if(depCurIndex == ind)
        buttonSaveDep->setEnabled(false);
    else
        buttonSaveDep->setEnabled(true);
}
void Cusers::setProxyFilter(const QString &filter)
{
    QRegExp regExp(filter,Qt::CaseInsensitive);
    proxyModel->setFilterRegExp(regExp);
    if(proxyModel->rowCount() > 0){
        setReadOnly(false);
        users->setCurrentIndex(proxyModel->index(0,5));
    }else{
        clearUserForm();
        setReadOnly(true);
    }
}
void Cusers::on_groupBoxDep_clicked(bool checked)
{
    if(depBoxCurState != checked)
        buttonSaveDep->setEnabled(true);
    else if(depCurIndex == departmentView->currentIndex()){
        buttonSaveDep->setEnabled(false);
    }else{
        if(!checked)
            buttonSaveDep->setEnabled(false);
    }
}

void Cusers::updateCurUserRowVersion()
{
    QModelIndex curIndex = realModelIndex(users->currentIndex());
    int curRowVersion = model->index(realModelIndex(users->currentIndex()).row(),13).data().toInt();
    if(curRowVersion == 254)
        model->setData(model->index(realModelIndex(users->currentIndex()).row(),13),0);
    else
        model->setData(model->index(realModelIndex(users->currentIndex()).row(),13),curRowVersion+1);
    model->submitAll();
    users->setCurrentIndex(proxyModel->mapFromSource(curIndex));
    if (model->lastError().type() != QSqlError::NoError)
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось обновить версию строки пользователя:\n %1")
                                 .arg(model->lastError().text()),
                                 tr("Закрыть"));
}

void Cusers::on_buttonSaveDep_clicked()
{
    QSqlQuery query;
    if(isLockedOrChanged()) return;
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockRecord(model->index(realModelIndex(users->currentIndex()).row(),0).data().toInt(),"CodUser","users"))
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось заблокировать запись:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    if(groupBoxDep->isChecked()){
        if(modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),1,departmentView->currentIndex().parent())).toInt() <= 0 ||
           modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),3,departmentView->currentIndex().parent())).toInt() == 1){
            QMessageBox::information(this, tr("Внимание"),
                                     tr("Выберите подразделение за которым закреплён пользователь."),
                                     tr("Закрыть"));
            return;
        }
        if(toUnitModel->rowCount() > 0){
            if(modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                            4,
                                                            departmentView->currentIndex().parent())).toInt() == 1){
                toUnitModel->setData(toUnitModel->index(0,2),QVariant());
                toUnitModel->setData(toUnitModel->index(0,1),
                               modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                                            1,
                                                                            departmentView->currentIndex().parent())));
                toUnitModel->submitAll();
                if(toUnitModel->lastError().type() != QSqlError::NoError){
                    toUnitModel->revertAll();
                    QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить изменения:\n%1").arg(toUnitModel->lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
                depBoxCurState = true;
                depCurIndex = departmentView->currentIndex();
                buttonSaveDep->setEnabled(false);
                updateCurUserRowVersion();
            }else{
                query.exec(QString("SELECT p.id FROM departments n, tree t, departments p "
                                   "WHERE n.id = %1 "
                                   "AND n.id = t.id AND t.parent_id = p.id AND t.level > 0 AND p.FP = 1;")
                           .arg(modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),1,departmentView->currentIndex().parent())).toInt()));
                if(query.lastError().type() != QSqlError::NoError){
                    QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось выполнить поиск Филиала/Представительства:\n%1").arg(query.lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
                if(query.size() <= 0){
                    toUnitModel->setData(toUnitModel->index(0,2),
                                   modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                                                1,
                                                                                departmentView->currentIndex().parent())));
                    toUnitModel->setData(toUnitModel->index(0,1),QVariant());
                    toUnitModel->submitAll();
                    if(toUnitModel->lastError().type() != QSqlError::NoError){
                        toUnitModel->revertAll();
                        QMessageBox::warning(this, tr("Ошибка"),
                                                 tr("Не удалось сохранить изменения:\n%1").arg(toUnitModel->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                    depBoxCurState = true;
                    depCurIndex = departmentView->currentIndex();
                    buttonSaveDep->setEnabled(false);
                    updateCurUserRowVersion();
                }else{
                    query.next();
                    toUnitModel->setData(toUnitModel->index(0,2),
                                   modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),1,departmentView->currentIndex().parent())));
                    toUnitModel->setData(toUnitModel->index(0,1),query.value(0));
                    toUnitModel->submitAll();
                    if(toUnitModel->lastError().type() != QSqlError::NoError){
                        toUnitModel->revertAll();
                        QMessageBox::warning(this, tr("Ошибка"),
                                                 tr("Не удалось сохранить изменения:\n%1").arg(toUnitModel->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                    depBoxCurState = true;
                    depCurIndex = departmentView->currentIndex();
                    buttonSaveDep->setEnabled(false);
                    updateCurUserRowVersion();
                }
            }
        }else{
            toUnitModel->insertRow(0);
            toUnitModel->setData(toUnitModel->index(0,0),
                           model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt());
            if(modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                            4,
                                                            departmentView->currentIndex().parent())).toInt() == 1){
                toUnitModel->setData(toUnitModel->index(0,1),
                                     modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                                                  1,
                                                                                  departmentView->currentIndex().parent())));
            }else{
                query.exec(QString("SELECT p.id FROM departments n, tree t, departments p "
                                   "WHERE n.id = %1 "
                                   "AND n.id = t.id AND t.parent_id = p.id AND t.level > 0 AND p.FP = 1;")
                           .arg(modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),
                                                                             1,
                                                                             departmentView->currentIndex().parent())).toInt()));
                if(query.lastError().type() != QSqlError::NoError){
                    QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось выполнить поиск Филиала/Представительства:\n%1").arg(query.lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
                if(query.size() > 0){
                    query.next();
                    toUnitModel->setData(toUnitModel->index(0,1),query.value(0));
                }
                toUnitModel->setData(toUnitModel->index(0,2),
                                     modelFilPredDep->data(modelFilPredDep->index(departmentView->currentIndex().row(),1,departmentView->currentIndex().parent())));
            }
            toUnitModel->submitAll();
            if(toUnitModel->lastError().type() != QSqlError::NoError){
                toUnitModel->revertAll();
                QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось сохранить изменения:\n%1").arg(toUnitModel->lastError().text()),
                                         tr("Закрыть"));
                return;
            }
            depBoxCurState = true;
            depCurIndex = departmentView->currentIndex();
            buttonSaveDep->setEnabled(false);
            updateCurUserRowVersion();
        }
    }else{
        if(toUnitModel->rowCount() > 0){
            toUnitModel->removeRow(0);
            toUnitModel->submitAll();
            if(toUnitModel->lastError().type() != QSqlError::NoError){
                toUnitModel->revertAll();
                QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось удалить привязку к подразделению:\n%1").arg(toUnitModel->lastError().text()),
                                         tr("Закрыть"));
                return;
            }
        }
        depBoxCurState = false;
        depCurIndex = QModelIndex();
        departmentView->setCurrentIndex(QModelIndex());
        buttonSaveDep->setEnabled(false);
        updateCurUserRowVersion();
    }
    if(!lockedControl->unlockRecord(model->index(realModelIndex(users->currentIndex()).row(),0).data().toInt(),"CodUser","users"))
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось разблокировать запись:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
}
void Cusers::on_addWPButton_clicked()
{
    AddWorkplaceOfUsers *awou = new AddWorkplaceOfUsers(this,
                                                        modelFilPredDep,
                                                        model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt());
    connect(awou,SIGNAL(addWorkPlace()),this,SLOT(wpViewUpdate()));
    awou->setAttribute(Qt::WA_DeleteOnClose);
    awou->exec();
}
void Cusers::on_delWPButton_clicked()
{
    dateOut = "";
    DelUserWithWp *duww = new DelUserWithWp(this);
    connect(duww,SIGNAL(userDateOut(QString)),this,SLOT(setDateOut(QString)));
    duww->setAttribute(Qt::WA_DeleteOnClose);
    if(!duww->exec())
        return;
    QSqlQuery query;
    query.exec(QString("DELETE FROM workplaceandusers WHERE CODWORKERPLACE = %1 AND CODUSER = %2")
               .arg(wpModel->data(wpModel->index(wpView->currentIndex().row(),0)).toInt())
               .arg(model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt()));
    if(query.lastError().type() != QSqlError::NoError){
        QMessageBox::warning(this, tr("Ошибка !!!"),
                             tr("Не удалось удалить пользователя с рабочено места:")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }
    if(!dateOut.isEmpty()){
        query.prepare(QString("INSERT INTO historyuseronwp SET CodUser = ?, CodWorkerPlace = ?, DateOut = ?"));
        query.addBindValue(model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt());
        query.addBindValue(wpModel->data(wpModel->index(wpView->currentIndex().row(),0)).toInt());
        query.addBindValue(dateOut);
        query.exec();
        if (query.lastError().type() != QSqlError::NoError)
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Не удалось записать историю ухода с рабочего места:\n %1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
        }
    }else
        QMessageBox::information(this, tr("Ошибка"),tr("Отсутствует дата ухода пользователя с раборего места."),tr("Закрыть"));
    query.exec(QString("SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1")
               .arg(wpModel->data(wpModel->index(wpView->currentIndex().row(),0)).toInt()));
    if(query.lastError().type() != QSqlError::NoError){
        QMessageBox::warning(this, tr("Ошибка !!!"),
                             tr("Не удалось получить информацию о привязке пользователя:")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }
    if(query.size() > 0){
        query.next();
        if(query.value(0).toInt() == model->data(model->index(realModelIndex(users->currentIndex()).row(),0)).toInt()){
            query.exec(QString("UPDATE workerplace SET PrimaryUser = NULL WHERE CodWorkerPlace = %1")
                       .arg(wpModel->data(wpModel->index(wpView->currentIndex().row(),0)).toInt()));
            if(query.lastError().type() != QSqlError::NoError){
                QMessageBox::warning(this, tr("Ошибка !!!"),
                                     tr("Не удалось очистить основного пользователя рабочего места:")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
                return;
            }
        }
    }
    wpViewUpdate();
    wpView->setFocus();
}

void Cusers::on_addButton_clicked()
{
    QList<int> userModelId;
    for(int i = 0; i < model->rowCount(); i++)
        userModelId<<model->record(i).value(0).toInt();
    AddExistingUser *aeu = new AddExistingUser(this,m_firmId,m_wpId,userModelId);
    connect(aeu,SIGNAL(userAdded()),this,SLOT(setUserFilter()));
    aeu->setAttribute(Qt::WA_DeleteOnClose);
    aeu->exec();
}

void Cusers::on_actionAddLoginPass_triggered()
{
    on_buttonAddLoginPass_clicked();
}

void Cusers::on_actionDelLoginPass_triggered()
{
    on_buttonDelLoginPass_clicked();
}

void Cusers::on_actionEditLoginPass_triggered()
{
    on_buttonEditLoginPass_clicked();
}

void Cusers::on_actionAddNumber_triggered()
{
    on_buttonAddNumber_clicked();
}

void Cusers::on_actionDelNumber_triggered()
{
    on_buttonDelNumber_clicked();
}

void Cusers::on_actionEditNumber_triggered()
{
    on_buttonEditNumber_clicked();
}

void Cusers::on_actionAddWP_triggered()
{
    on_addWPButton_clicked();
}

void Cusers::on_actionDelWP_triggered()
{
    on_delWPButton_clicked();
}

void Cusers::on_buttonHistory_clicked()
{
    QDialog *d = new QDialog(this);
    int nWidth = 400;
    int nHeight = 200;
    d->setWindowTitle(tr("История пользователя на рабочих местах"));
    d->setMinimumSize(200,200);
    QVBoxLayout *layout = new QVBoxLayout;
    JournalHistoryUsersOnWP *jhu = new JournalHistoryUsersOnWP(d,model->index(realModelIndex(users->currentIndex()).row(),0).data().toInt());
    jhu->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhu,SIGNAL(closeJhuWin()),d,SLOT(reject()));
    layout->addWidget(jhu);
    d->setLayout(layout);
    d->layout()->setContentsMargins(0,0,0,0);
    jhu->show();
    QPoint parentPos = m_parent->mapToGlobal(m_parent->pos());
    d->setGeometry(parentPos.x() + m_parent->width()/2 - nWidth/2,
                   parentPos.y() + m_parent->height()/2 - nHeight/2,
                   nWidth, nHeight);
    d->exec();
}

void Cusers::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
