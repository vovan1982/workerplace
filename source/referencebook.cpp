#include <QMdiSubWindow>
#include "headers/referencebook.h"
#include "headers/lockdatabase.h"

CReferenceView::CReferenceView(QWidget *parent) : QTableView(parent)
{
}
void CReferenceView::resizeEvent(QResizeEvent *event)
{
    resizeRowsToContents();
    QTableView::resizeEvent(event);
}

CReferenceBook::CReferenceBook(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    curReferenceBook = "";
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(updateLockReferenceBook()));
    timer->start(30000);
    model = new QSqlRelationalTableModel(this);
    model2 = new QSqlRelationalTableModel(this);
    QStringList refList;
    refList << tr("<Выберите справочник>"); // case default
    refList << tr("Тип логинов"); // case 1
    refList << tr("Должности"); // case 2
    refList << tr("Тип телефонных номеров"); // case 3
    refList << tr("Состояние устройств"); // case 4
    refList << tr("Состояние программного обеспечения"); // case 5
    refList << tr("Тип лицензии программного обеспечения"); // case 6
    refList << tr("Домены\\Рабочие группы"); // case 7
    refList << tr("Причины перемещения устройств"); // case 8
    comboBoxReference->addItems(refList);
    connect(comboBoxReference,SIGNAL(activated(int)),this, SLOT(dataTableChange(int)));

    tableReference = new CReferenceView(this);
    tableReference->setObjectName(QString::fromUtf8("tableReference"));
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    tableReference->setFont(font);
    tableReference->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
    tableReference->setAlternatingRowColors(true);
    tableReference->setSelectionMode(QAbstractItemView::SingleSelection);
    tableReference->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableReference->setGridStyle(Qt::DotLine);
    tableReference->setSortingEnabled(true);
    tableReference->setCornerButtonEnabled(false);
    tableReference->horizontalHeader()->setStretchLastSection(true);
    verticalLayout_2->addWidget(tableReference);
    connect((QObject*) tableReference->horizontalHeader(), SIGNAL(sectionClicked (int)), tableReference, SLOT(resizeRowsToContents()));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(dataModelEdit()));
    modelSet = false;
    editMode = false;
    readOnly = false;
}

void CReferenceBook::dataModelEdit()
{
    changeButton(true);
}

void CReferenceBook::dataTableChange(int index)
{
    switch(index){
        case 1: updateTable("logintype"); break;
        case 2: updateTable("post"); break;
        case 3: updateTable("typenumber"); break;
        case 4: updateTable("statedev"); break;
        case 5: updateTable("statepo"); break;
        case 6: updateTable("typelicense"); break;
        case 7: updateTable("domainwg"); break;
        case 8: updateTable("cause"); break;
        default:
            tableReference->setModel(model2);
            curReferenceBook = "";
            activeButton(false);
            changeButton(false);
            modelSet = false;
    }
}

void CReferenceBook::on_addButton_clicked()
{
    model->insertRow(model->rowCount());
    tableReference->selectRow(model->rowCount()-1);
    tableReference->setFocus();
    changeButton(true);
    tableReference->resizeRowsToContents();
}

void CReferenceBook::on_delButton_clicked()
{
    QModelIndex index;
    index = tableReference->currentIndex();
    model->removeRow(index.row());
    changeButton(true);
}

void CReferenceBook::on_submitButton_clicked()
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
    tableReference->resizeRowsToContents();
}

void CReferenceBook::on_revertButton_clicked()
{
    model->revertAll();
    model->submitAll();
    tableReference->selectRow(0);
    tableReference->setFocus();
    changeButton(false);
    tableReference->resizeRowsToContents();
}

void CReferenceBook::updateTable(QString tableName)
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!readOnly && modelSet){
        if(!lockedControl->unlockReferenceBook(curReferenceBook))
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать справочник:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
    }
    if(lockedControl->referenceBookIsLocked(tableName)){
        readOnly = true;
        addButton->setEnabled(false);
        delButton->setEnabled(false);
        tableReference->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
            return;
        }else{
            readOnly = false;
            addButton->setEnabled(true);
            delButton->setEnabled(true);
            tableReference->setEditTriggers(QAbstractItemView::DoubleClicked |
                                            QAbstractItemView::EditKeyPressed |
                                            QAbstractItemView::AnyKeyPressed);
            if(!lockedControl->lockReferenceBook(tableName))
                QMessageBox::warning(this,tr("Ошибка!!!"),
                                     tr("Не удалось заблокировать выбранный справочник:\n %1\n")
                                     .arg(lockedControl->lastError().text()),
                                     tr("Закрыть"));
        }
    }

    tableReference->setModel(model2);
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    model->setHeaderData(1, Qt::Horizontal,tr("Наименование"));
    tableReference->setModel(model);
    modelSet = true;
    tableReference->setColumnHidden(0,true);
    tableReference->horizontalHeader()->setStretchLastSection(true);
    tableReference->setCurrentIndex(model->index(0,1));
    tableReference->resizeRowsToContents();
    activeButton(true);
    changeButton(false);
    curReferenceBook = tableName;
}
void CReferenceBook::activeButton(bool act)
{
    addButton->setEnabled(act);
    delButton->setEnabled(act);
}
void CReferenceBook::changeButton(bool ch)
{
    revertButton->setEnabled(ch);
    submitButton->setEnabled(ch);
    editMode = ch;
}
void CReferenceBook::updateLockReferenceBook()
{
    if(!curReferenceBook.isEmpty()){
        LockDataBase *lockedControl = new LockDataBase(this);
        if(!lockedControl->lockReferenceBook(curReferenceBook)){
            timer->stop();
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось продлить блокировку выбраного справочника:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
        }
    }
}
void CReferenceBook::keyPressEvent(QKeyEvent * e)
{
    if(!readOnly){
        if ((e->key() == Qt::Key_Insert) && (modelSet)){
            tableReference->model()->insertRow(tableReference->model()->rowCount());
            tableReference->selectRow(tableReference->model()->rowCount()-1);
            tableReference->setFocus();
            changeButton(true);
            tableReference->resizeRowsToContents();
        }
        if((e->key() == Qt::Key_Escape) && (modelSet) && (editMode)){
            model->revertAll();
            model->submitAll();
            tableReference->selectRow(0);
            tableReference->setFocus();
            changeButton(false);
        }
        if ((e->key() == Qt::Key_Delete) && (modelSet)){
            QModelIndex index;
            index = tableReference->currentIndex();
            model->removeRow(index.row());
            tableReference->closePersistentEditor(index);
            tableReference->selectRow(index.row());
            tableReference->setFocus();
            changeButton(true);
        }
        if ((e->key() == Qt::Key_S) && (e->modifiers() == Qt::ControlModifier) && (modelSet) && (editMode))
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
void CReferenceBook::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
void CReferenceBook::closeEvent(QCloseEvent *event)
{
    LockDataBase *lockedControl = new LockDataBase(this);
    if(!readOnly && modelSet){
        if(!lockedControl->unlockReferenceBook(curReferenceBook))
            QMessageBox::warning(this,tr("Ошибка!!!"),
                                 tr("Не удалось разблокировать справочник:\n %1\n")
                                 .arg(lockedControl->lastError().text()),
                                 tr("Закрыть"));
    }
    model->clear();
    model2->clear();
    event->accept();
    QWidget *parent = this->parentWidget();
    while (parent) {
        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow *>(parent);
        if (subWindow) {
            subWindow->close();
            break;
        }
        parent = parent->parentWidget();
    }
}
