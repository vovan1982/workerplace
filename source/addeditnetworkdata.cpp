#include <QtSql>
#include <QMenu>
#include <QMessageBox>
#include "headers/addeditnetworkdata.h"
#include "headers/edittable.h"
#include "headers/addeditnetworkinterface.h"
#include "headers/interfacemodel.h"

AddEditNetworkData::AddEditNetworkData(QWidget *parent, int wpId, bool readOnly) :
    QWidget(parent), m_wpId(wpId), m_readOnly(readOnly)
{
    setupUi(this);

    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT CodNetworkData, Name, CodDomainWg FROM networkdata WHERE CodWorkerPlace = %1").arg(wpId));
    if(ok){
        if(query.size() > 0){
            query.next();
            networkDataId = query.value(0).toInt();
            networkName = query.value(1).toString();
            domainWgId = query.value(2).toInt();
            query.clear();
        }else{
            networkDataId = 0;
            networkName = "";
            domainWgId = 0;
            query.clear();
        }
    }else
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось получить данные сетевых настроек:\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
    interfaceModel = new InterfaceModel(interfaceView);
    interfaceModel->setFilter(QString("CodNetworkData = %1").arg(networkDataId));
    interfaceView->setModel(interfaceModel);
    interfaceModel->sort(2,Qt::AscendingOrder);
    interfaceView->setColumnHidden(0,true);
    interfaceView->setColumnHidden(1,true);
    interfaceView->setColumnHidden(8,true);
    interfaceView->setColumnHidden(9,true);
    interfaceView->setColumnHidden(10,true);
    interfaceView->resizeColumnToContents(2);
    interfaceView->resizeColumnToContents(3);
    if(interfaceModel->rowCount() > 0)
        interfaceView->setCurrentIndex(interfaceModel->index(0,2));
    else
        interfaceView->setCurrentIndex(QModelIndex());

    if(!readOnly){
        if(networkDataId == 0){
            buttonAddInterface->setEnabled(false);
            buttonChangeInterface->setEnabled(false);
            buttonDelInterface->setEnabled(false);
            buttonDelNetworkData->setEnabled(false);
            actionAddInterface->setEnabled(false);
            actionChangeInterface->setEnabled(false);
            actionDelInterface->setEnabled(false);
        }
        if(interfaceModel->rowCount() > 0){
            buttonChangeInterface->setEnabled(true);
            buttonDelInterface->setEnabled(true);
            actionChangeInterface->setEnabled(true);
            actionDelInterface->setEnabled(true);
        }else{
            buttonChangeInterface->setEnabled(false);
            buttonDelInterface->setEnabled(false);
            actionChangeInterface->setEnabled(false);
            actionDelInterface->setEnabled(false);
        }
    }else{
        buttonAddInterface->setEnabled(false);
        buttonChangeInterface->setEnabled(false);
        buttonDelInterface->setEnabled(false);
        buttonDelNetworkData->setEnabled(false);
        actionAddInterface->setEnabled(false);
        actionChangeInterface->setEnabled(false);
        actionDelInterface->setEnabled(false);
        name->setReadOnly(true);
        domainWg->setEnabled(false);
        buttonEditDomainWg->setEnabled(false);
    }
    populateCBox("CodDomainWg","domainwg","",tr("<Выберите Домен/Рабочую группу>"),domainWg);
    setDefaultData();
    connect(interfaceModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(updateCurIndexInterface()));
    connect(interfaceView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(on_buttonChangeInterface_clicked()));
    connect(interfaceView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onInterfaceMenu(const QPoint &)));
}

void AddEditNetworkData::changeEvent(QEvent *e)
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

void AddEditNetworkData::onInterfaceMenu(const QPoint &p){
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(actionAddInterface);
    ag->addAction(actionChangeInterface);
    ag->addAction(actionDelInterface);
    QMenu *menu = new QMenu(tr("Интерфейсы"), this);
    menu->addActions(ag->actions());
    menu->exec(interfaceView->viewport()->mapToGlobal(p));
}

void AddEditNetworkData::populateCBox(const QString &idName, const QString &tableName,
                  const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    if(!nullStr.isNull() && !nullStr.isEmpty())
        cBox->addItem(nullStr);
    QSqlQuery query;
    if(filter.isNull() || filter.isEmpty())
        query.exec(QString("SELECT %2,name FROM %1;").arg(tableName).arg(idName));
    else
        query.exec(QString("SELECT %2,name FROM %1 WHERE %3;").arg(tableName).arg(idName).arg(filter));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(this, tr("Ошибка"),
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

void AddEditNetworkData::setDefaultData()
{
    if(m_wpId <= 0) QMessageBox::warning(this, tr("Ошибка"), tr("Отсутствует код рабочего места"), tr("Закрыть"));
    else{
        if(networkDataId == 0){
           name->clear();
           domainWg->setCurrentIndex(0);
        }else{
            name->setText(networkName);
            domainWg->setCurrentIndex(domainWg->findData(domainWgId));
        }
    }

}

bool AddEditNetworkData::dataChanged()
{
    if(networkName != name->text() || domainWgId != domainWg->itemData(domainWg->currentIndex()).toInt())
        return true;
    else
        return false;
}

bool AddEditNetworkData::dataIsEntered()
{
    if(!name->text().isNull() && !name->text().isEmpty() && domainWg->currentIndex() > 0 && m_wpId > 0)
        return true;
    else
        return false;
}

void AddEditNetworkData::updateCurIndexInterface()
{
    if(interfaceModel->rowCount() > 0)
        interfaceView->setCurrentIndex(interfaceModel->index(0,2));
    else
        interfaceView->setCurrentIndex(QModelIndex());
}

void AddEditNetworkData::on_buttonEditDomainWg_clicked()
{
    CeditTable edittable(this,QString("domainwg"));
    edittable.setWindowTitle(tr("Редактирование доменов/рабочих групп"));
    edittable.exec();
    domainWg->clear();
    populateCBox("CodDomainWg","domainwg","",tr("<Выберите Домен/Рабочую группу>"),domainWg);
}

void AddEditNetworkData::on_name_textEdited()
{
    if(dataChanged()){
        buttonRevertNetworkData->setEnabled(true);
        buttonSaveNetworkData->setEnabled(dataIsEntered());
    }else{
        buttonRevertNetworkData->setEnabled(false);
        buttonSaveNetworkData->setEnabled(false);
    }
}

void AddEditNetworkData::on_domainWg_currentIndexChanged(int)
{
    if(dataChanged()){
        buttonRevertNetworkData->setEnabled(true);
        buttonSaveNetworkData->setEnabled(dataIsEntered());
    }else{
        buttonRevertNetworkData->setEnabled(false);
        buttonSaveNetworkData->setEnabled(false);
    }
}

void AddEditNetworkData::on_buttonRevertNetworkData_clicked()
{
    setDefaultData();
}

void AddEditNetworkData::on_buttonSaveNetworkData_clicked()
{
    QSqlQuery addquery;
    QString queryStr;
    if(networkDataId == 0){
        QQueue<QVariant> bindval;
        QString field, val;
        field = "("; val = "(";

        field += "CodWorkerPlace,Name,CodDomainWg"; val += "?,?,?";
        bindval.enqueue(m_wpId);
        bindval.enqueue(name->text());
        bindval.enqueue(domainWg->itemData(domainWg->currentIndex()).toInt());

        field += ")"; val += ")";
        queryStr = "INSERT INTO networkdata "+field+" VALUES "+val;
        addquery.prepare(queryStr);
        while(!bindval.empty()){
            addquery.addBindValue(bindval.dequeue());
        }
        addquery.exec();
        if (addquery.lastError().type() != QSqlError::NoError){
            QMessageBox::warning(this, tr("Ошибка"),
                                     tr("Не удалось сохранить сетевые настройки:\n %1")
                                     .arg(addquery.lastError().text()),
                                     tr("Закрыть"));
            return;
        }else{
            buttonRevertNetworkData->setEnabled(false);
            buttonSaveNetworkData->setEnabled(false);
            buttonDelNetworkData->setEnabled(true);

            networkDataId = addquery.lastInsertId().toInt();
            networkName = name->text();
            domainWgId = domainWg->itemData(domainWg->currentIndex()).toInt();
            buttonAddInterface->setEnabled(true);
            actionAddInterface->setEnabled(true);
            interfaceModel->setFilter(QString("CodNetworkData = %1").arg(networkDataId));
            QMessageBox::information(this, tr("Сохранение"),
                                     tr("Настройки успешно сохранены"),
                                     tr("Закрыть"));
        }
    }else{
        bool prev = false;
        queryStr = QString("UPDATE networkdata SET ");
        if(networkName != name->text()){
            queryStr += QString("Name = '%1'").arg(name->text());
            prev = true;
        }
        if(domainWgId != domainWg->itemData(domainWg->currentIndex()).toInt()){
            if(prev) queryStr += QString(", "); else prev = true;
            queryStr += QString("CodDomainWg = %1").arg(domainWg->itemData(domainWg->currentIndex()).toInt());
        }

        queryStr += QString(" WHERE CodWorkerPlace = %1").arg(m_wpId);
        addquery.exec(queryStr);
        if (addquery.lastError().type() != QSqlError::NoError){
            QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Не удалось сохранить сетевые настройки:\n %1")
                                 .arg(addquery.lastError().text()),
                                 tr("Закрыть"));
            return;
        }else{
            buttonRevertNetworkData->setEnabled(false);
            buttonSaveNetworkData->setEnabled(false);

            networkName = name->text();
            domainWgId = domainWg->itemData(domainWg->currentIndex()).toInt();
            QMessageBox::information(this, tr("Сохранение"),
                                     tr("Настройки успешно сохранены!"),
                                     tr("Закрыть"));
        }
    }
}

void AddEditNetworkData::on_buttonDelNetworkData_clicked()
{
    int button;
    button = QMessageBox::question(this,tr("Удаление сетевых настроек"),
                                   tr("Вы уверены что хотите удалить сетевые настройки?"),
                                   tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    QSqlQuery query;
    query.exec(QString("DELETE FROM %1 WHERE CodNetworkData = %2").arg("networkdata").arg(networkDataId));

    if (query.lastError().type() != QSqlError::NoError){
        QMessageBox::warning(this, tr("Ошибка"),
                             tr("Не удалось удалить сетевые настройки:\n %1")
                             .arg(query.lastError().text()),
                             tr("Закрыть"));
        return;
    }else{
        networkDataId = 0;
        networkName = "";
        domainWgId = 0;
        setDefaultData();
        buttonAddInterface->setEnabled(false);
        buttonChangeInterface->setEnabled(false);
        buttonDelInterface->setEnabled(false);
        buttonDelNetworkData->setEnabled(false);
        actionAddInterface->setEnabled(false);
        actionChangeInterface->setEnabled(false);
        actionDelInterface->setEnabled(false);
        interfaceModel->select();
    }
}

void AddEditNetworkData::on_buttonAddInterface_clicked()
{
    AddEditNetworkInterface *aeni = new AddEditNetworkInterface(this, networkDataId, interfaceModel);
    if(aeni->exec()){
        if(interfaceModel->rowCount() > 0){
            buttonChangeInterface->setEnabled(true);
            buttonDelInterface->setEnabled(true);
            actionChangeInterface->setEnabled(true);
            actionDelInterface->setEnabled(true);
        }
    }
}

void AddEditNetworkData::on_buttonChangeInterface_clicked()
{
    if(m_readOnly) return;
    AddEditNetworkInterface *aeni = new AddEditNetworkInterface(this, networkDataId, interfaceModel, true,interfaceView->currentIndex());
    aeni->exec();
}

void AddEditNetworkData::on_buttonDelInterface_clicked()
{
    int button;
    button = QMessageBox::question(this,tr("Удаление сетевого интерфейса"),
                                   tr("Вы уверены что хотите удалить данный сетевой интерфейс?"),
                                   tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    interfaceModel->removeRecord(interfaceModel->data(interfaceModel->index(interfaceView->currentIndex().row(),0)).toInt());
    if(interfaceModel->rowCount() <= 0){
        buttonChangeInterface->setEnabled(false);
        buttonDelInterface->setEnabled(false);
        actionChangeInterface->setEnabled(false);
        actionDelInterface->setEnabled(false);
    }
}

void AddEditNetworkData::on_actionAddInterface_triggered()
{
    on_buttonAddInterface_clicked();
}

void AddEditNetworkData::on_actionChangeInterface_triggered()
{
    on_buttonChangeInterface_clicked();
}

void AddEditNetworkData::on_actionDelInterface_triggered()
{
    on_buttonDelInterface_clicked();
}
