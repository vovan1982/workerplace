#include <QtWidgets>
#include <QtSql>
#include <QKeyEvent>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableView>
#include "headers/edittable.h"
#include "headers/cedittableview.h"
#include "headers/lockdatabase.h"

CeditTable::CeditTable(QWidget *parent, QString tName, bool selectMode) :
    QDialog(parent), m_tName(tName), m_selectMode(selectMode)
{
    setupUi(this);

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateLockReferenceBook()));
    readOnly = false;
    selectButton->setVisible(selectMode);
    addButton->setVisible(!selectMode);
    delButton->setVisible(!selectMode);
    submitButton->setVisible(!selectMode);
    revertButton->setVisible(!selectMode);

    tableView = new CeditTableView(this);
    tableView->setObjectName(QString::fromUtf8("tableView"));
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    tableView->setFont(font);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setGridStyle(Qt::DotLine);
    tableView->setSortingEnabled(true);
    tableView->setCornerButtonEnabled(false);
    verticalLayout_2->addWidget(tableView);
    connect((QObject*) tableView->horizontalHeader(), SIGNAL(sectionClicked (int)), tableView, SLOT(resizeRowsToContents()));

    model = new QSqlTableModel(tableView);
    model->setTable(tName);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal,tr("Наименование"));
    tableView->setModel(model);
    tableView->setColumnHidden(0,true);
    tableView->horizontalHeader()->setStretchLastSection(true);
    editMode = false;
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataModelEdit()));
    tableView->setCurrentIndex(model->index(0,1));
    tableView->resizeRowsToContents();

    if(!selectMode){
        LockDataBase *lockedControl = new LockDataBase(this);
        if(lockedControl->referenceBookIsLocked(tName)){
            readOnly = true;
            setWindowTitle(windowTitle()+tr(" - [Только чттение]"));
            addButton->setEnabled(false);
            delButton->setEnabled(false);
            tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
            QMessageBox::information(this,tr("ВНИМАНИЕ!!!"),
                                     tr("Справочник заблокирован пользователем: %1")
                                     .arg(lockedControl->referenceBookBlockingUser()),
                                     tr("Закрыть"));
        }else{
            if(lockedControl->lastError().type() != QSqlError::NoError){
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось получить информацию о блокировке:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
            }else{
                if(!lockedControl->lockReferenceBook(tName))
                    QMessageBox::warning(this,tr("Ошибка!!!"),
                                         tr("Не удалось заблокировать выбранный справочник:\n %1\n")
                                         .arg(lockedControl->lastError().text()),
                                         tr("Закрыть"));
                else
                    timer->start(30000);
            }
        }
    }else{
        readOnly = true;
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}
void CeditTable::changeButton(bool ch)
{
    revertButton->setEnabled(ch);
    submitButton->setEnabled(ch);
    editMode = ch;
}
void CeditTable::dataModelEdit()
{
    changeButton(true);
}
void CeditTable::on_addButton_clicked()
{
    model->insertRow(model->rowCount());
    tableView->selectRow(model->rowCount()-1);
    tableView->setFocus();
    tableView->resizeRowsToContents();
    changeButton(true);
}

void CeditTable::on_delButton_clicked()
{
    QModelIndex index;
    index = tableView->currentIndex();
    model->removeRow(index.row());
    changeButton(true);
}

void CeditTable::on_submitButton_clicked()
{
    model->submitAll();
    if (model->lastError().type() != QSqlError::NoError){
        model->revertAll();
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Ни все изменения удалось применить:\n %1").arg(model->lastError().text()),
                                 "Закрыть");
        model->submitAll();
    }
    changeButton(false);
    tableView->resizeRowsToContents();
}

void CeditTable::on_revertButton_clicked()
{
    model->revertAll();
    model->submitAll();
    tableView->selectRow(0);
    tableView->setFocus();
    changeButton(false);
    tableView->resizeRowsToContents();
}
void CeditTable::on_selectButton_clicked()
{
    QList<QVariant> datas;
    for(int i = 0;i<model->columnCount();i++){
        datas<<model->data(model->index(tableView->currentIndex().row(),i));
    }
    emit selectedRowData(datas);
    this->accept();
}
void CeditTable::updateLockReferenceBook()
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!lockedControl->lockReferenceBook(m_tName)){
        timer->stop();
        QMessageBox::warning(this,tr("Ошибка!!!"),
                             tr("Не удалось продлить блокировку выбраного справочника:\n %1\n")
                             .arg(lockedControl->lastError().text()),
                             tr("Закрыть"));
    }
}
void CeditTable::keyPressEvent(QKeyEvent * e)
{
    if(!readOnly){
        if ((e->key() == Qt::Key_Insert) && !m_selectMode){
            tableView->model()->insertRow(tableView->model()->rowCount());
            tableView->selectRow(tableView->model()->rowCount()-1);
            tableView->setFocus();
            tableView->resizeRowsToContents();
            changeButton(true);
        }
        if(((e->key() == Qt::Key_Escape) && editMode) && !m_selectMode){
            model->revertAll();
            model->submitAll();
            tableView->selectRow(0);
            tableView->setFocus();
            changeButton(false);
        }
        if ((e->key() == Qt::Key_Delete) && !m_selectMode){
            QModelIndex index;
            index = tableView->currentIndex();
            model->removeRow(index.row());
            tableView->closePersistentEditor(index);
            tableView->selectRow(index.row());
            tableView->setFocus();
            changeButton(true);
        }
        if (((e->key() == Qt::Key_S) && (e->modifiers() == Qt::ControlModifier) && editMode) && !m_selectMode)
        {
            model->submitAll();
            if (model->lastError().type() != QSqlError::NoError){
                model->revertAll();
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Ни все изменения удалось применить:\n %1").arg(model->lastError().text()),
                                         tr("Закрыть"));
                model->submitAll();
            }
            changeButton(false);
        }
    }
}
void CeditTable::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
void CeditTable::closeEvent(QCloseEvent *event)
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!readOnly){
        if(!lockedControl->unlockReferenceBook(m_tName))
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать справочник:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
    }
    event->accept();
}
