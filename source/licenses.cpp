#include <QtSql>
#include <QMenu>
#include <QClipboard>
#include <QMessageBox>
#include "headers/enums.h"
#include "headers/licenses.h"
#include "headers/movelicense.h"
#include "headers/licensemodel.h"
#include "headers/lockdatabase.h"
#include "headers/filterlicense.h"
#include "headers/loadindicator.h"
#include "headers/workplacemodel.h"
#include "headers/addeditlicense.h"
#include "headers/licensemodelcontrol.h"
#include "headers/journalhistorymoved.h"

Licenses::Licenses(QWidget *parent, bool readOnly) :
    QWidget(parent), m_readOnly(readOnly)
{
    setupUi(this);
    dontShowLoadindicator = false;
    if(m_readOnly) setWindowTitle(windowTitle()+tr(" - [Только чттение]"));
    filterIsSet = false;
    licenseFilter = "";

    QActionGroup *ag = new QActionGroup(this);
    ag->setExclusive(true);
    ag->addAction(actionAddLicense);
    ag->addAction(actionDelLicense);
    ag->addAction(actionEditLicense);
    ag->addAction(actionMoveLicense);
    ag->addAction(actionHistoryMoved);
    ag->addAction(actionCopyLicenseInBufer);
    menu = new QMenu(tr("Лицензии"), this);
    menu->addActions(ag->actions());
    licenseButton->setMenu(menu);

    actionAddLicense->setEnabled(false);
    buttonUpdate->setEnabled(false);
    buttonCopyAllInBufer->setEnabled(false);

    li = new LoadIndicator(licenseView,tr("Подождите идёт загрузка..."));

    lModel = new LicenseModelControl(licenseView,licenseView,showParentDevice->isChecked(),licenseFilter,true);

    connect(lModel,SIGNAL(dataIsPopulated()),this,SLOT(dataIsLoaded()));
    connect(licenseView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setAccessToActions(QModelIndex)));
    connect(licenseView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onLicMenu(const QPoint &)));
    connect(licenseView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(doubleClickedLicenseView()));
    connect(buttonUpdate,SIGNAL(clicked()),this,SLOT(updateLicenseModel()));
}
void Licenses::showEvent(QShowEvent *e)
{
    QWidget::showEvent( e );
    if(!dontShowLoadindicator){
        dontShowLoadindicator = true;
        showLoadIndicator();
    }
}
void Licenses::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    li->updatePosition();
}
void Licenses::dataIsLoaded()
{
    li->stop();
    if(lModel->model()->lastError().type() == QSqlError::NoError){
        actionAddLicense->setEnabled(true);
        buttonUpdate->setEnabled(true);
        buttonCopyAllInBufer->setEnabled(true);
        if(lModel->model()->rowCount() == 0)
            setAccessToActions();
        else{
            showParentDevice->setEnabled(true);
            setAccessToActions(licenseView->currentIndex());
        }
        if(showParentDevice->isChecked())
            licenseView->collapseAll();
    }else{
        QMessageBox::warning(this, tr("Ошибка"),
                                 tr("Ошибка получения данных:\n %1").arg(lModel->model()->lastError().text()),
                                 tr("Закрыть"));
    }
}
void Licenses::onLicMenu(const QPoint &p){
   menu->exec(licenseView->viewport()->mapToGlobal(p));
}
void Licenses::doubleClickedLicenseView(){
    if(lModel->model()->data(lModel->model()->index(lModel->realModelIndex(licenseView->currentIndex()).row(),
                                                    lModel->model()->cIndex.isLicense,
                                                    lModel->realModelIndex(licenseView->currentIndex()).parent())).toInt() == 1)
        on_actionEditLicense_triggered();
}
void Licenses::updateLicenseModel()
{
    showLoadIndicator();
    lModel->updateLicModel();
}
void Licenses::updateLicenseRow()
{
    lModel->model()->updateRow(lModel->realModelIndex(licenseView->currentIndex()).row(),lModel->realModelIndex(licenseView->currentIndex()).parent());
    if(showParentDevice->isChecked()) updateLicenseModel();
}
void Licenses::setAccessToActions(const QModelIndex &index)
{
    QModelIndex curIndex = lModel->realModelIndex(index);
    if(lModel->model()->rowCount() == 0){
        actionDelLicense->setEnabled(false);
        actionEditLicense->setEnabled(false);
        actionMoveLicense->setEnabled(false);
        actionHistoryMoved->setEnabled(false);
        actionCopyLicenseInBufer->setEnabled(false);
        moveButton->setEnabled(false);
        setFilterButton->setEnabled(filterIsSet);
        clearFilterButton->setEnabled(filterIsSet);
    }else{
        if(curIndex.isValid()){
            organization->setText(lModel->model()->index(curIndex.row(),lModel->model()->cIndex.nameOrg,curIndex.parent()).data().toString());
            if(!m_readOnly){
                moveButton->setEnabled(true);
                setFilterButton->setEnabled(true);
                clearFilterButton->setEnabled(filterIsSet);
                if(lModel->model()->index(curIndex.row(),lModel->model()->cIndex.isLicense,curIndex.parent()).data().toInt() == 1){
                    actionDelLicense->setEnabled(true);
                    actionEditLicense->setEnabled(true);
                    actionMoveLicense->setEnabled(true);
                    actionHistoryMoved->setEnabled(true);
                    actionCopyLicenseInBufer->setEnabled(true);
                }else{
                    actionDelLicense->setEnabled(false);
                    actionEditLicense->setEnabled(false);
                    actionMoveLicense->setEnabled(false);
                    actionHistoryMoved->setEnabled(false);
                    actionCopyLicenseInBufer->setEnabled(false);
                }
            }else{
                moveButton->setEnabled(false);
                setFilterButton->setEnabled(true);
                clearFilterButton->setEnabled(filterIsSet);
                actionDelLicense->setEnabled(false);
                actionMoveLicense->setEnabled(false);
                actionAddLicense->setEnabled(false);
                if(lModel->model()->index(curIndex.row(),lModel->model()->cIndex.isLicense,curIndex.parent()).data().toInt() == 1){
                    actionHistoryMoved->setEnabled(true);
                    actionCopyLicenseInBufer->setEnabled(true);
                    actionEditLicense->setEnabled(true);
                }else{
                    actionHistoryMoved->setEnabled(false);
                    actionCopyLicenseInBufer->setEnabled(false);
                    actionEditLicense->setEnabled(false);
                }
            }
        }
    }
}
void Licenses::on_actionAddLicense_triggered()
{
    AddEditLicense *ael = new AddEditLicense(this,Enums::Standart);
    connect(ael,SIGNAL(licenseAdded()),this,SLOT(updateLicenseModel()));
    ael->setAttribute(Qt::WA_DeleteOnClose);
    ael->exec();
}
void Licenses::on_actionEditLicense_triggered()
{
    QList<QVariant> r;
    bool readOnly = false;
    QModelIndex curViewIndex = lModel->realModelIndex(licenseView->currentIndex());
    if(m_readOnly)
        readOnly = m_readOnly;
    else{
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->recordIsLosked(lModel->model()->data(lModel->model()->index(curViewIndex.row(),
                                                                                           lModel->model()->cIndex.key,
                                                                                           curViewIndex.parent())).toInt(),
                                          "`"+lModel->model()->colTabName.key+"`",
                                          lModel->model()->nameModelTable())){
            if(lockedControl->lastError().type() != QSqlError::NoError)
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
        }else{
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Запись заблокированна пользователем: %1")
                                     .arg(lockedControl->recordBlockingUser()),
                                     tr("Закрыть"));
            readOnly = true;
        }
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("SELECT %1, %2 FROM %3 WHERE `%4` = %5")
                        .arg(lModel->model()->colTabName.rv)
                        .arg(lModel->model()->colTabName.codOrganization)
                        .arg(lModel->model()->nameModelTable())
                        .arg(lModel->model()->colTabName.key)
                        .arg(lModel->model()->data(lModel->model()->index(curViewIndex.row(),
                                                                          lModel->model()->cIndex.key,
                                                                          curViewIndex.parent())).toInt()));
        if(ok){
            query.next();
            if(lModel->model()->index(curViewIndex.row(),
                                      lModel->model()->cIndex.rv,
                                      curViewIndex.parent()).data().toInt() != query.value(0).toInt()){
                if(query.value(1).toInt() != lModel->model()->index(curViewIndex.row(),
                                                                    lModel->model()->cIndex.codOrganization,
                                                                    curViewIndex.parent()).data().toInt()){
                    QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                             tr("Лицензия была перемещена в другую организацию,\n"
                                                "повторите выбор лицензии."),
                                             tr("Закрыть"));
                    updateLicenseModel();
                    return;
                }else{
                    QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                             tr("Запись была изменена, будут загружены новые данные записи"),
                                             tr("Закрыть"));
                    if(!lModel->model()->updateRow(curViewIndex.row())){
                        QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                                 tr("Не удалось загрузить новые данные:\n%1").arg(lModel->model()->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                    setAccessToActions(licenseView->currentIndex());
                }
            }
        }else{
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось получить информацию о версии записи:\n %1\n")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }

    }
    for(int i=0; i<lModel->model()->columnCount(lModel->realModelIndex(licenseView->currentIndex()));i++)
        r.append(lModel->model()->data(lModel->model()->index(lModel->realModelIndex(licenseView->currentIndex()).row(),i,
                                            lModel->realModelIndex(licenseView->currentIndex()).parent())));
    AddEditLicense *ael = new AddEditLicense(this,Enums::Standart,true,r,readOnly);
    if(readOnly)
        ael->setWindowTitle(tr("Редактирование лицензии - [Только чтение]"));
    else
        ael->setWindowTitle(tr("Редактирование лицензии"));
    connect(ael,SIGNAL(licenseDataChanged()),this,SLOT(updateLicenseRow()));
    ael->setAttribute(Qt::WA_DeleteOnClose);
    ael->exec();
}
void Licenses::on_actionDelLicense_triggered()
{
    int button = QMessageBox::question(this,tr("Удаление лицензии"),
                                       tr("Вы уверены что хотите удалить данную лицензию?"),
                                       tr("Да"),tr("Нет"),"",1,1);
    if (button == 1)
        return;
    lModel->model()->sqlRemoveRow(lModel->realModelIndex(licenseView->currentIndex()).row(),lModel->realModelIndex(licenseView->currentIndex()).parent());
    if(lModel->model()->rowCount() <= 0)
        setAccessToActions();
}
void Licenses::on_moveButton_clicked()
{
    MoveLicense *ml = new MoveLicense(this);
    connect(ml,SIGNAL(licIsMoved()),SLOT(updateLicenseModel()));
    if(ml->exec())
        QMessageBox::information(this," ",tr("Перемещение выполненно успешно."),tr("Закрыть"));
}
void Licenses::on_closeButton_clicked()
{
    emit closeLicWin();
}
void Licenses::on_showParentDevice_clicked(bool checked)
{
    showLoadIndicator();
    lModel->setShowParentDevice(checked);
    updateLicenseModel();
}
void Licenses::on_setFilterButton_clicked()
{
    QMap<int,QString> organizationList;
    QSqlQuery query;
    query.exec(QString("SELECT id,name FROM departments WHERE firm = 1;"));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось получить список организаций:\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
        return;
    }else{
        while(query.next())
            organizationList[query.value(0).toInt()] = query.value(1).toString();
    }
    FilterLicense *fl = new FilterLicense(this,organizationList);
    connect(fl,SIGNAL(setFilter(QString)),SLOT(setFilter(QString)));
    fl->exec();
}
void Licenses::setFilter(const QString &filter)
{
    if(filter.isNull() || filter.isEmpty()){
        licenseFilter = "";
        filterIsSet = false;
    }else{
        licenseFilter = filter;
        filterIsSet = true;
    }
    lModel->setLicFilterWhithOutUpdate(licenseFilter);
    updateLicenseModel();
}
void Licenses::on_clearFilterButton_clicked()
{
    setFilter("");
}
void Licenses::on_actionMoveLicense_triggered()
{
    QList<QVariant> licData;
    QModelIndex curIndex = lModel->realModelIndex(licenseView->currentIndex());
    for(int i = 0;i<lModel->model()->columnCount();i++){
        licData<<lModel->model()->data(lModel->model()->index(curIndex.row(),i,curIndex.parent()));
    }
    MoveLicense *ml = new MoveLicense(this,true,licData);
    connect(ml,SIGNAL(licIsMoved()),SLOT(updateLicenseModel()));
    if(ml->exec())
        QMessageBox::information(this," ",tr("Перемещение выполненно успешно."),tr("Закрыть"));
}
void Licenses::on_actionHistoryMoved_triggered()
{
    QDialog *d = new QDialog(this);
    int nWidth = 400;
    int nHeight = 200;
    d->setWindowTitle(tr("История перемещений"));
    QVBoxLayout *layout = new QVBoxLayout;
    JournalHistoryMoved *jhm = new JournalHistoryMoved(d,
                                                       lModel->model()->index(lModel->realModelIndex(licenseView->currentIndex()).row(),
                                                                              lModel->model()->cIndex.key,
                                                                              lModel->realModelIndex(licenseView->currentIndex()).parent())
                                                       .data().toInt(),
                                                       1);
    jhm->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhm,SIGNAL(closeJhmWin()),d,SLOT(reject()));
    layout->addWidget(jhm);
    d->setLayout(layout);
    d->layout()->setContentsMargins(0,0,0,0);
    jhm->show();
    QPoint parentPos = this->mapToGlobal(this->pos());
    d->setGeometry(parentPos.x() + this->width()/2 - nWidth/2,
                   parentPos.y() + this->height()/2 - nHeight/2,
                   nWidth, nHeight);
    d->exec();
    d->adjustSize();
}
void Licenses::on_actionCopyLicenseInBufer_triggered()
{
    QString str = "";
    QModelIndex index = lModel->realModelIndex(licenseView->currentIndex());
    // шапка
    str += tr("Организация") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("№ версии") + "\t";
    str += tr("Тип лицензии") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Регистрационный код") + "\t";
    str += tr("Количество лицензий") + "\t";
    str += tr("Примечание") + "\n";
    // данные
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameOrg,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.namePO,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameProd,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.invN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.versionN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameLic,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameState,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.regKey,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.kolLicense,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.note,index.parent()).data().toString();
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, tr("Копирование"),tr("Данные скопированны в буфер обмена."),tr("Закрыть"));
}
QString Licenses::readDataModel(const QModelIndex &index){
    QString str = "";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameOrg,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.namePO,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameProd,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.invN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.versionN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameLic,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameState,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.regKey,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.kolLicense,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.note,index.parent()).data().toString().replace("\n"," ") + "\n";
    return str;
}
void Licenses::on_buttonCopyAllInBufer_clicked()
{
    QString str = "";
    QModelIndex parent, children;
    // шапка
    str += tr("Организация") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("№ версии") + "\t";
    str += tr("Тип лицензии") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Регистрационный код") + "\t";
    str += tr("Количество лицензий") + "\t";
    str += tr("Примечание") + "\n";
    // данные
    for(int i = 0; i < lModel->model()->rowCount(); i++){
        if(lModel->showParentDevice()){
            parent = lModel->model()->index(i,0);
            if(lModel->model()->index(i,lModel->model()->cIndex.isLicense).data().toInt() == 0)
                for(int j = 0; j < lModel->model()->rowCount(parent); j++){
                    children = lModel->model()->index(j,0,parent);
                    if(lModel->model()->index(j,lModel->model()->cIndex.isLicense,parent).data().toInt() == 0)
                        str += readDataModel(children.child(0,0));
                    else
                        str += readDataModel(children);
                }
            else
                str += readDataModel(parent);
        }else{
            str += readDataModel(lModel->model()->index(i,0));
        }
    }
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, tr("Копирование"),tr("Данные скопированны в буфер обмена."),tr("Закрыть"));
}
void Licenses::changeEvent(QEvent *e)
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
void Licenses::showLoadIndicator()
{
    actionAddLicense->setEnabled(false);
    actionDelLicense->setEnabled(false);
    actionEditLicense->setEnabled(false);
    actionMoveLicense->setEnabled(false);
    actionHistoryMoved->setEnabled(false);
    actionCopyLicenseInBufer->setEnabled(false);
    moveButton->setEnabled(false);
    setFilterButton->setEnabled(false);
    clearFilterButton->setEnabled(false);
    buttonUpdate->setEnabled(false);
    buttonCopyAllInBufer->setEnabled(false);
    showParentDevice->setEnabled(false);
    li->start();
}
