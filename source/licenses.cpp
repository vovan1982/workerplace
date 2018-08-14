#include <QMenu>
#include <QMessageBox>
#include <QClipboard>
#include "headers/licenses.h"
#include "headers/licensemodel.h"
#include "headers/licensemodelcontrol.h"
#include "headers/workplacemodel.h"
#include "headers/addeditlicense.h"
#include "headers/movelicense.h"
#include "headers/filterlicense.h"
#include "headers/journalhistorymoved.h"
#include "headers/lockdatabase.h"

Licenses::Licenses(QWidget *parent, bool wpMode, int wpId, bool whMode, bool readOnly) :
    QWidget(parent), m_wpMode(wpMode), m_wpId(wpId), m_readOnly(readOnly)
{
    setupUi(this);
    if(m_readOnly) setWindowTitle(windowTitle()+tr(" - [Только чттение]"));
    filterIsSet = false;
    licenseFilter = "";
    wpLicenseFilter = "";

    QActionGroup *ag = new QActionGroup(this);
    ag->setExclusive(true);

    if(wpMode){
        groupBox->setVisible(false);
        groupBox_4->setVisible(false);
        groupBox_5->setVisible(false);
        closeButton->setVisible(false);
        horizontalLayout->removeItem(horizontalSpacer);
        if(!whMode){
            setFilterButton->setVisible(false);
            clearFilterButton->setVisible(false);
        }
        wpLicenseFilter = QString("l.CodWorkerPlace = %1").arg(wpId);
        licenseFilter = wpLicenseFilter;
        ag->addAction(actionAddExistingLicense);
    }

    ag->addAction(actionAddLicense);
    ag->addAction(actionDelLicense);
    ag->addAction(actionEditLicense);
    ag->addAction(actionMoveLicense);
    ag->addAction(actionHistoryMoved);
    ag->addAction(actionCopyLicenseInBufer);
    menu = new QMenu(tr("Лицензии"), this);
    menu->addActions(ag->actions());
    licenseButton->setMenu(menu);

    lModel = new LicenseModelControl(licenseView,licenseView,showParentDevice->isChecked(),licenseFilter);
    connect(licenseView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setAccessToActions(QModelIndex)));
    connect(licenseView, SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(onLicMenu(const QPoint &)));
    connect(licenseView, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(doubleClickedLicenseView()));
    connect(buttonUpdate,SIGNAL(clicked()),lModel,SLOT(updateLicModel()));
    lModel->setCurrentIndexFirstRow();

    setAccessToActions();

    if(!wpMode){
        showParentDevice->setVisible(false);
        wpModel = new WorkPlaceModel(this);
        updateWpModel(licenseView->currentIndex());
        connect(licenseView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(updateWpModel(QModelIndex)));
    }
}
void Licenses::onLicMenu(const QPoint &p){
   menu->exec(licenseView->viewport()->mapToGlobal(p));
}
void Licenses::doubleClickedLicenseView(){
    if(lModel->model()->data(lModel->model()->index(licenseView->currentIndex().row(),
                                                    lModel->model()->cIndex.isLicense,
                                                    licenseView->currentIndex().parent())).toInt() == 1)
        on_actionEditLicense_triggered();
}
void Licenses::populateWpModel(int filter)
{
    wpModel->setFilter(QString("CodWorkerPlace = %1").arg(filter));
    wpModel->select();
    if (wpModel->lastError().type() != QSqlError::NoError){
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка обновления рабочего места:\n %1").arg(wpModel->lastError().text()),
                                 "Закрыть");
    }
}
void Licenses::clearWpForm()
{
    WPName->setText("");
    WPUser->setText("");
    WPDivision->setText("");
}
void Licenses::updateWpModel(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    QSqlQuery query;
    bool ok;
    filial->clear();
    organization->clear();
    ok = query.exec(QString("select p.id, p.name, p.Firm from departments n, tree t, departments p where n.id =  "
                       "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                       "and n.id = t.id and t.parent_id = p.id and (p.FP = 1 or p.Firm = 1);")
               .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка получения данных о фирме:\n %1").arg(query.lastError().text()),
                                 "Закрыть");
    else{
        if(query.size() > 0){
            while(query.next()){
                if(query.value(2).toInt())
                    organization->setText(query.value(1).toString());
                else
                    filial->setText(query.value(1).toString());
            }
        }
    }
    populateWpModel(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt());
    if(wpModel->rowCount() == 0)
        clearWpForm();
    else{
        if(wpModel->index(0,8).data().toInt() != 1 && wpModel->index(0,9).data().toInt() != 1){
            WPName->setText(wpModel->index(0,3).data().toString());
            WPUser->setText(wpModel->index(0,5).data().toString());
            WPDivision->setText(wpModel->index(0,2).data().toString());
        }else{
            WPName->setText(wpModel->index(0,3).data().toString());
            WPUser->setText("");
            WPDivision->setText("");
        }
    }
    device->setText(lModel->model()->index(index.row(),lModel->model()->cIndex.nameDevice,index.parent()).data().toString());

    ok = query.exec(QString("SELECT typedevice.Name FROM device "
                            "INNER JOIN typedevice ON device.CodTypeDevice = typedevice.CodTypeDevice "
                            "WHERE device.id = %1")
                    .arg(abs(lModel->model()->index(index.row(),lModel->model()->cIndex.codDevice,index.parent()).data().toInt())));
    if(!ok)
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка получения типа устройства:\n %1").arg(query.lastError().text()),
                                 "Закрыть");
    else{
        if(query.size() > 0){
            query.next();
            typeDevice->setText(query.value(0).toString());
        }else
            typeDevice->setText("");
    }
}
void Licenses::updateLicenseModel()
{
    lModel->updateLicModel();
    setAccessToActions(licenseView->currentIndex());
    if(showParentDevice->isChecked())
        licenseView->expandAll();
}
void Licenses::updateLicenseRow()
{
    lModel->model()->updateRow(licenseView->currentIndex().row(),licenseView->currentIndex().parent());
    if(!m_wpMode) updateWpModel(licenseView->currentIndex());
    if(showParentDevice->isChecked()) updateLicenseModel();
}
void Licenses::setAccessToActions(const QModelIndex &index)
{
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
        if(index.isValid()){
            if(!m_readOnly){
                moveButton->setEnabled(true);
                setFilterButton->setEnabled(true);
                clearFilterButton->setEnabled(filterIsSet);
                if(lModel->model()->index(index.row(),lModel->model()->cIndex.isLicense,index.parent()).data().toInt() == 1){
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
                actionAddExistingLicense->setEnabled(false);
                actionAddLicense->setEnabled(false);
                if(lModel->model()->index(index.row(),lModel->model()->cIndex.isLicense,index.parent()).data().toInt() == 1){
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
    AddEditLicense *ael = new AddEditLicense(this,m_wpMode,m_wpId);
    connect(ael,SIGNAL(licenseAdded()),this,SLOT(updateLicenseModel()));
    ael->setAttribute(Qt::WA_DeleteOnClose);
    ael->exec();
}
void Licenses::on_actionEditLicense_triggered()
{
    QList<QVariant> r;
    bool readOnly = false;
    QModelIndex curViewIndex = licenseView->currentIndex();
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
    }
    for(int i=0; i<lModel->model()->columnCount(licenseView->currentIndex());i++)
        r.append(lModel->model()->data(lModel->model()->index(licenseView->currentIndex().row(),i,
                                            licenseView->currentIndex().parent())));
    AddEditLicense *ael = new AddEditLicense(this,m_wpMode,m_wpId,true,r,readOnly);
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
    lModel->model()->sqlRemoveRow(licenseView->currentIndex().row(),licenseView->currentIndex().parent());
    if(lModel->model()->rowCount() <= 0){
        if(!m_wpMode){
            organization->setText("");
            filial->setText("");
            clearWpForm();
            device->setText("");
            typeDevice->setText("");
        }
        setAccessToActions();
    }
}
void Licenses::on_moveButton_clicked()
{
    MoveLicense *ml;
    if(!m_wpMode)
        ml = new MoveLicense(this);
    else{
        QSqlQuery query;
        bool ok;
        QList<QVariant> wpData;
        if(lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.isLicense,licenseView->currentIndex().parent()).data().toInt() == 1){
            wpData << "wpwh";
            wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.nameWP,licenseView->currentIndex().parent()).data().toString();
            wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.codWorkerPlace,licenseView->currentIndex().parent())
                      .data().toInt();

            ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                    "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                    "and n.id = t.id and t.parent_id = p.id;")
                            .arg(lModel->model()->index(licenseView->currentIndex().row(),
                                                        lModel->model()->cIndex.codWorkerPlace,
                                                        licenseView->currentIndex().parent())
                                 .data().toInt()));
        }else{
            ok = query.exec(QString("SELECT Name FROM workerplace WHERE CodWorkerPlace = %1;").arg(m_wpId));
            if(!ok){
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Ошибка получения данных о рабочем месте:\n %1")
                                         .arg(query.lastError().text()),
                                         "Закрыть");
                return;
            }
            query.next();
            wpData << "wpwh";
            wpData << query.value(0).toString();
            wpData << m_wpId;
            ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                    "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                    "and n.id = t.id and t.parent_id = p.id;")
                            .arg(m_wpId));
        }
        if(!ok)
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Ошибка получения данных о фирме, филиале и подразделении:\n %1")
                                     .arg(query.lastError().text()),
                                     "Закрыть");
        else{
            if(query.size() > 0){
                while(query.next()){
                    if(query.value(2).toInt() == 1){
                        wpData << "org";
                        wpData << query.value(1).toString();
                        wpData << query.value(0).toInt();
                        wpData << "dep";wpData << "";wpData << 0;
                        wpData << "fp";wpData << "";wpData << 0;
                    }else if(query.value(3).toInt() == 1){
                        wpData << "fp";
                        wpData << query.value(1).toString();
                        wpData << query.value(0).toInt();
                        wpData << "dep";wpData << "";wpData << 0;
                    }else{
                        wpData << "dep";
                        wpData << query.value(1).toString();
                        wpData << query.value(0).toInt();
                    }
                }
            }
        }
        ml = new MoveLicense(this,false,wpData,QList<QVariant>(),false,true);
    }
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
    lModel->setShowParentDevice(checked);
    updateLicenseModel();
}
void Licenses::on_setFilterButton_clicked()
{
    FilterLicense *fl;
    if(m_wpMode)
        fl = new FilterLicense(this,true,m_wpId);
    else
        fl = new FilterLicense(this);
    connect(fl,SIGNAL(setFilter(QString)),SLOT(setFilter(QString)));
    fl->exec();
}
void Licenses::setFilter(const QString &filter)
{
    if(!wpLicenseFilter.isEmpty() && !filter.isEmpty())
        licenseFilter = wpLicenseFilter+" AND "+filter;
    if(!wpLicenseFilter.isEmpty() && filter.isEmpty())
        licenseFilter = wpLicenseFilter;
    if(wpLicenseFilter.isEmpty() && !filter.isEmpty())
        licenseFilter = filter;
    if(wpLicenseFilter.isEmpty() && filter.isEmpty())
        licenseFilter = "";
    if(filter.isNull() || filter.isEmpty()){
        filterIsSet = false;
    }else{
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
    QSqlQuery query;
    bool ok;
    QList<QVariant> wpData;
    QList<QVariant> licData;
    for(int i = 0;i<lModel->model()->columnCount();i++){
        licData<<lModel->model()->data(lModel->model()->index(licenseView->currentIndex().row(),i,licenseView->currentIndex().parent()));
    }
    if(lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.isLicense,licenseView->currentIndex().parent()).data().toInt() == 1){
        wpData << "wpwh";
        wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.nameWP,licenseView->currentIndex().parent()).data().toString();
        wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.codWorkerPlace,licenseView->currentIndex().parent())
                  .data().toInt();

        ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id;")
                        .arg(lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.codWorkerPlace,licenseView->currentIndex().parent())
                             .data().toInt()));
    }else{
        ok = query.exec(QString("SELECT Name FROM workerplace WHERE CodWorkerPlace = %1;").arg(m_wpId));
        if(!ok){
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Ошибка получения данных о рабочем месте:\n %1")
                                     .arg(query.lastError().text()),
                                     "Закрыть");
            return;
        }
        query.next();
        wpData << "wpwh";
        wpData << query.value(0).toString();
        wpData << m_wpId;
        ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id;")
                        .arg(m_wpId));
    }
    if(!ok){
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка получения данных о фирме, филиале и подразделении:\n %1")
                                 .arg(query.lastError().text()),
                                 "Закрыть");
        return;
    }else{
        if(query.size() > 0){
            while(query.next()){
                if(query.value(2).toInt() == 1){
                    wpData << "org";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                    wpData << "dep";wpData << "";wpData << 0;
                    wpData << "fp";wpData << "";wpData << 0;
                }else if(query.value(3).toInt() == 1){
                    wpData << "fp";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                    wpData << "dep";wpData << "";wpData << 0;
                }else{
                    wpData << "dep";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                }
            }
        }
    }

    MoveLicense *ml = new MoveLicense(this,true,wpData,licData);
    connect(ml,SIGNAL(licIsMoved()),SLOT(updateLicenseModel()));
    if(ml->exec())
        QMessageBox::information(this," ",tr("Перемещение выполненно успешно."),tr("Закрыть"));
}
void Licenses::on_actionAddExistingLicense_triggered()
{
    QSqlQuery query;
    bool ok = false;
    QList<QVariant> wpData;
    if(lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.isLicense,licenseView->currentIndex().parent()).data().toInt() == 1){
        wpData << "wpwh";
        wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.nameWP,licenseView->currentIndex().parent()).data().toString();
        wpData << lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.codWorkerPlace,licenseView->currentIndex().parent())
                  .data().toInt();

        ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id;")
                        .arg(lModel->model()->index(licenseView->currentIndex().row(),lModel->model()->cIndex.codWorkerPlace,licenseView->currentIndex().parent())
                             .data().toInt()));
    }else{
        ok = query.exec(QString("SELECT Name FROM workerplace WHERE CodWorkerPlace = %1;").arg(m_wpId));
        if(!ok){
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Ошибка получения данных о рабочем месте:\n %1")
                                     .arg(query.lastError().text()),
                                     "Закрыть");
            return;
        }
        query.next();
        wpData << "wpwh";
        wpData << query.value(0).toString();
        wpData << m_wpId;
        ok = query.exec(QString("select p.id, p.name, p.Firm, p.FP from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id;")
                        .arg(m_wpId));
    }
    if(!ok)
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ошибка получения данных о фирме, филиале и подразделении:\n %1")
                                 .arg(query.lastError().text()),
                                 "Закрыть");
    else{
        if(query.size() > 0){
            while(query.next()){
                if(query.value(2).toInt() == 1){
                    wpData << "org";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                    wpData << "dep";wpData << "";wpData << 0;
                    wpData << "fp";wpData << "";wpData << 0;
                }else if(query.value(3).toInt() == 1){
                    wpData << "fp";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                    wpData << "dep";wpData << "";wpData << 0;
                }else{
                    wpData << "dep";
                    wpData << query.value(1).toString();
                    wpData << query.value(0).toInt();
                }
            }
        }
    }

    MoveLicense *ml = new MoveLicense(this,false,wpData,QList<QVariant>(),true);
    connect(ml,SIGNAL(licIsMoved()),SLOT(updateLicenseModel()));
    if(ml->exec())
        QMessageBox::information(this," ",tr("Лицензии успешно добавлены."),tr("Закрыть"));
}
void Licenses::on_actionHistoryMoved_triggered()
{
    QDialog *d = new QDialog(this);
    int nWidth = 400;
    int nHeight = 200;
    d->setWindowTitle(tr("История перемещений"));
    QVBoxLayout *layout = new QVBoxLayout;
    JournalHistoryMoved *jhm = new JournalHistoryMoved(d,
                                                       lModel->model()->index(licenseView->currentIndex().row(),
                                                                                lModel->model()->cIndex.key,
                                                                                licenseView->currentIndex().parent())
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
    bool ok;
    QSqlQuery query;
    QString str = "";
    QModelIndex index = licenseView->currentIndex();
    // шапка
    str += tr("Фирма") + "\t";
    str += tr("Филиал/Представительство") + "\t";
    str += tr("Подразделение") + "\t";
    str += tr("Рабочее место/склад") + "\t";
    str += tr("Пользователь") + "\t";
    str += tr("Тип устройства") + "\t";
    str += tr("Инветарный № устройства") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("№ версии") + "\t";
    str += tr("Тип лицензии") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Регистрационный код") + "\n";
    // данные
    ok = query.exec(QString("select p.name, p.Firm from departments n, tree t, departments p where n.id =  "
                       "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                       "and n.id = t.id and t.parent_id = p.id and p.Firm = 1;")
               .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";

    if(!m_wpMode){
        str += filial->text() + "\t";
        str += WPDivision->text() + "\t";
    }else{
        ok = query.exec(QString("select p.name from departments n, tree t, departments p where n.id =  "
                                "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                                "and n.id = t.id and t.parent_id = p.id and p.FP = 1;")
                        .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
        ok = query.exec(QString("SELECT Name FROM departments WHERE id = (SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace = %1)")
                        .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
    }
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameWP,index.parent()).data().toString() + "\t";
    if(!m_wpMode)
        str += WPUser->text() + "\t";
    else{
        QSqlQuery query;
        bool ok;
        ok = query.exec(QString("SELECT FIOSummary FROM users WHERE CodUser = (SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1)")
                        .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
        if(!ok)
            qDebug()<<query.lastError().text();
        if(query.size() > 0){
            query.next();
            str += query.value(0).toString() + "\t";
        }else
            str += "\t";
    }
    ok = query.exec(QString("SELECT typedevice.Name ,device.InventoryN FROM device "
                            "INNER JOIN typedevice ON device.CodTypeDevice = typedevice.CodTypeDevice "
                            "WHERE device.id = %1")
                    .arg(abs(lModel->model()->index(index.row(),lModel->model()->cIndex.codDevice,index.parent()).data().toInt())));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
        str += query.value(1).toString() + "\t";
    }else{
        str += "\t";
        str += "\t";
    }
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.namePO,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameProd,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.invN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.versionN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameLic,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameState,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.regKey,index.parent()).data().toString();
    QApplication::clipboard()->setText(str);
    QMessageBox::information(this, tr("Копирование"),tr("Данные скопированны в буфер обмена."),tr("Закрыть"));
}
QString Licenses::readDataModel(const QModelIndex &index){
    bool ok;
    QSqlQuery query;
    QString str = "";
    ok = query.exec(QString("select p.name, p.Firm from departments n, tree t, departments p where n.id =  "
                            "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                            "and n.id = t.id and t.parent_id = p.id and p.Firm = 1;")
                    .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    ok = query.exec(QString("select p.name from departments n, tree t, departments p where n.id =  "
                            "(SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace  = %1) "
                            "and n.id = t.id and t.parent_id = p.id and p.FP = 1;")
                    .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    ok = query.exec(QString("SELECT Name FROM departments WHERE id = (SELECT CodDepartment FROM workerplace WHERE CodWorkerPlace = %1)")
                    .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameWP,index.parent()).data().toString() + "\t";
    ok = query.exec(QString("SELECT FIOSummary FROM users WHERE CodUser = (SELECT PrimaryUser FROM workerplace WHERE CodWorkerPlace = %1)")
                    .arg(lModel->model()->index(index.row(),lModel->model()->cIndex.codWorkerPlace,index.parent()).data().toInt()));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
    }else
        str += "\t";
    ok = query.exec(QString("SELECT typedevice.Name ,device.InventoryN FROM device "
                            "INNER JOIN typedevice ON device.CodTypeDevice = typedevice.CodTypeDevice "
                            "WHERE device.id = %1")
                    .arg(abs(lModel->model()->index(index.row(),lModel->model()->cIndex.codDevice,index.parent()).data().toInt())));
    if(!ok)
        qDebug()<<query.lastError().text();
    if(query.size() > 0){
        query.next();
        str += query.value(0).toString() + "\t";
        str += query.value(1).toString() + "\t";
    }else{
        str += "\t";
        str += "\t";
    }
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.namePO,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameProd,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.invN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.versionN,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameLic,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.nameState,index.parent()).data().toString() + "\t";
    str += lModel->model()->index(index.row(),lModel->model()->cIndex.regKey,index.parent()).data().toString() + "\n";
    return str;
}
void Licenses::on_buttonCopyAllInBufer_clicked()
{
    QString str = "";
    QModelIndex parent, children;
    // шапка
    str += tr("Фирма") + "\t";
    str += tr("Филиал/Представительство") + "\t";
    str += tr("Подразделение") + "\t";
    str += tr("Рабочее место/склад") + "\t";
    str += tr("Пользователь") + "\t";
    str += tr("Тип устройства") + "\t";
    str += tr("Инветарный № устройства") + "\t";
    str += tr("Наименование") + "\t";
    str += tr("Производитель") + "\t";
    str += tr("Инветарный №") + "\t";
    str += tr("№ версии") + "\t";
    str += tr("Тип лицензии") + "\t";
    str += tr("Состояние") + "\t";
    str += tr("Регистрационный код") + "\n";
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
