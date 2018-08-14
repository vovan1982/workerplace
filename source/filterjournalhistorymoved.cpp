#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QToolButton>
#include "headers/filterjournalhistorymoved.h"
#include "headers/edittable.h"
#include "headers/addexistinguser.h"

FilterJournalHistoryMoved::FilterJournalHistoryMoved(QWidget *parent, bool filterIsEmpty, int organizationId) :
    QDialog(parent),
    m_filterIsEmpty(filterIsEmpty),
    m_organizationId(organizationId)
{
    setupUi(this);
    items = initItemAdditionalFilter();
    sizeConditionNames = sizeConditionName(items,2,7);
    insertRowInAdditionalFilter(0);
}

void FilterJournalHistoryMoved::changeEvent(QEvent *e)
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

void FilterJournalHistoryMoved::addButton_clicked()
{
    if (rowsFilterVerticalLayout->count() >= 20){
        QMessageBox::information(this,"",tr("Достигнут предел правил фильтрации!!!"),1);
        return;
    }
    int curRow;
    curRow = sender()->property("curRow").toInt();
    curRow++;
    insertRowInAdditionalFilter(curRow);

    if(curRow < rowsFilterVerticalLayout->count()-1)
        for(int r = curRow+1;r <= rowsFilterVerticalLayout->count()-1; r++){
            QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(r)->layout());
            for(int c = 1;c<=6;c++){
                if(c != 3){
                    QWidget *w = t->itemAtPosition(0,c)->widget();
                    w->setProperty("curRow",w->property("curRow").toInt()+1);
                }
            }
        }
}

void FilterJournalHistoryMoved::delFilterRow()
{
    if (rowsFilterVerticalLayout->count() <= 1) return;
    int curRow = sender()->property("curRow").toInt();
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(curRow)->layout());
    for(int i = 0;i<=6;i++){
        QWidget *w = t->itemAtPosition(0,i)->widget();
        t->removeWidget(w);
        w->close();
    }
    t->~QGridLayout();
    if(curRow <= rowsFilterVerticalLayout->count()-1)
        for(int r = curRow;r <= rowsFilterVerticalLayout->count()-1; r++){
            QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(r)->layout());
            for(int c = 1;c<=6;c++){
                if(c != 3){
                    QWidget *w = t->itemAtPosition(0,c)->widget();
                    w->setProperty("curRow",w->property("curRow").toInt()-1);
                }
            }
        }
    if(curRow == 0){
        QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(0)->layout());
        QComboBox *cbox = qobject_cast<QComboBox *>(t->itemAtPosition(0,0)->widget());
        cbox->setEnabled(false);
        cbox->setCurrentIndex(1);
    }
}

QList< QList<ItemComboBox> > FilterJournalHistoryMoved::initItemAdditionalFilter()
{
    ItemComboBox item;
    QList<ItemComboBox> groupItem;
    QList< QList<ItemComboBox> > initItems;

    // group item index 0
    item.name = tr("или"); item.data = " OR "; groupItem<<item;
    item.name = tr("и"); item.data = " AND "; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 1
    item.name = tr("Наименование"); item.data = "(device.Name REGEXP %2 OR po.Name REGEXP %2)"; groupItem<<item;
    item.name = tr("Инвентарный №"); item.data = ""; groupItem<<item;
    item.name = tr("Дата"); item.data = "historymoved.DateMoved"; groupItem<<item;
    item.name = tr("Старое место нахождения"); item.data = "historymoved.OldPlace"; groupItem<<item;
    item.name = tr("Новое место нахождения"); item.data = "historymoved.NewPlace"; groupItem<<item;
    item.name = tr("Причина"); item.data = "historymoved.CodCause"; groupItem<<item;
    item.name = tr("Исполнитель"); item.data = "historymoved.CodPerformer"; groupItem<<item;
    item.name = tr("Примечание"); item.data = "historymoved.Note"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 2
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 3
    item.name = tr("Содержит"); item.data = "%1"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 4
    item.name = tr("Содержит"); item.data = "%1 REGEXP %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 5
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Содержит"); item.data = "%1 REGEXP %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 6
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = ">"; item.data = "%1 > %2"; groupItem<<item;
    item.name = "<"; item.data = "%1 < %2"; groupItem<<item;
    item.name = ">="; item.data = "%1 >= %2"; groupItem<<item;
    item.name = "<="; item.data = "%1 <= %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 7
    item.name = "="; item.data = "%1(device.InventoryN = %2 OR licenses.InvN = %2)"; groupItem<<item;
    item.name = tr("Содержит"); item.data = "%1(device.InventoryN REGEXP %2 OR licenses.InvN REGEXP %2)"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "%1"+QString("(((device.InventoryN IS NULL "
                                                           "OR device.InventoryN = '') "
                                                           "AND TypeHistory = 0) "
                                                           "OR ((licenses.InvN IS NULL "
                                                           "OR licenses.InvN = '') "
                                                           "AND TypeHistory = 1))"); groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    return initItems;
}

void FilterJournalHistoryMoved::insertRowInAdditionalFilter(int index)
{
    QGridLayout *rg = new QGridLayout();
    rg->setSpacing(6);
    rowsFilterVerticalLayout->insertLayout(index,rg);
    QComboBox *w0 = new QComboBox(scrollAreaWidgetContents);
    w0->setAttribute(Qt::WA_DeleteOnClose);
    populateComboBoxAdditionalFilter(w0,items,0);
    rg->addWidget(w0,0,0,1,1);
    w0->show();
    if(index == 0){w0->setEnabled(false); w0->setCurrentIndex(1);}
    QComboBox *w1 = new QComboBox(scrollAreaWidgetContents);
    w1->setAttribute(Qt::WA_DeleteOnClose);
    w1->setProperty("curRow",index);
    populateComboBoxAdditionalFilter(w1,items,1);
    connect(w1,SIGNAL(currentIndexChanged(int)),this,SLOT(fieldAdditionalFilterChanget(int)));
    rg->addWidget(w1,0,1,1,1);
    w1->show();
    QComboBox *w2 = new QComboBox(scrollAreaWidgetContents);
    w2->setAttribute(Qt::WA_DeleteOnClose);
    w2->setProperty("curRow",index);
    populateComboBoxAdditionalFilter(w2,items,3);
    w2->setMinimumContentsLength(sizeConditionNames);
    connect(w2,SIGNAL(currentIndexChanged(QString)),this,SLOT(conditionAdditionalFilterChanget(QString)));
    rg->addWidget(w2,0,2,1,1);
    w2->show();
    QLineEdit *w3 = new QLineEdit(scrollAreaWidgetContents);
    w3->setAttribute(Qt::WA_DeleteOnClose);
    w3->setReadOnly(false);
    rg->addWidget(w3,0,3,1,1);
    w3->show();
    QToolButton *w4 = new QToolButton(scrollAreaWidgetContents);
    w4->setIcon(QIcon(":/32x32/journal/ico/journal_32x32.png"));
    w4->setProperty("curRow",index);
    w4->setAttribute(Qt::WA_DeleteOnClose);
    connect(w4,SIGNAL(clicked()),this,SLOT(additionalButton_clicked()));
    rg->addWidget(w4,0,4,1,1,Qt::AlignBottom);
    w4->show();
    w4->setEnabled(false);
    QToolButton *w5 = new QToolButton(scrollAreaWidgetContents);
    w5->setIcon(QIcon(":/16x16/add/ico/edit_add_16x16.png"));
    w5->setProperty("curRow",index);
    w5->setAttribute(Qt::WA_DeleteOnClose);
    connect(w5,SIGNAL(clicked()),this,SLOT(addButton_clicked()));
    rg->addWidget(w5,0,5,1,1);
    w5->show();
    QToolButton *w6 = new QToolButton(scrollArea);
    w6->setIcon(QIcon(":/16x16/cancelDel/ico/del_16x16.png"));
    w6->setProperty("curRow",index);
    w6->setAttribute(Qt::WA_DeleteOnClose);
    connect(w6,SIGNAL(clicked()),this,SLOT(delFilterRow()));
    rg->addWidget(w6,0,6,1,1);
    w6->show();
}

void FilterJournalHistoryMoved::populateComboBoxAdditionalFilter(QComboBox *cbox, const QList< QList<ItemComboBox> > itemCBox, int groupItemIndex)
{
    if(itemCBox.size()-1<groupItemIndex) return;
    cbox->clear();
    for (int i = 0;i<itemCBox[groupItemIndex].size();i++)
        cbox->addItem(itemCBox[groupItemIndex].value(i).name,itemCBox[groupItemIndex].value(i).data);
}

void FilterJournalHistoryMoved::populateCBox(const QString &idName, const QString &tableName,
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
    cBox->setMaxCount(query.size()+1);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

void FilterJournalHistoryMoved::fieldAdditionalFilterChanget(int index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QComboBox *cbox = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());

    switch(index){
    case 0:
        populateComboBoxAdditionalFilter(cbox,items,3);
        break;
    case 1:
        populateComboBoxAdditionalFilter(cbox,items,7);
        break;
    case 2:
        populateComboBoxAdditionalFilter(cbox,items,6);
        break;
    case 3: case 4:
        populateComboBoxAdditionalFilter(cbox,items,4);
        break;
    case 5: case 6:
        populateComboBoxAdditionalFilter(cbox,items,2);
        break;
    case 7:
        populateComboBoxAdditionalFilter(cbox,items,5);
        break;
    }
    QWidget *w = t->itemAtPosition(0,3)->widget();
    t->removeWidget(w);
    w->close();
    if(index == 0 || index == 1 || (index >= 3 && index <= 7)){
        QLineEdit *le = new QLineEdit(scrollAreaWidgetContents);
        le->setAttribute(Qt::WA_DeleteOnClose);
        if(index == 0 || index == 1 || index == 3 || index == 4 || index == 7)
            le->setReadOnly(false);
        else
            le->setReadOnly(true);
        t->addWidget(le,0,3,1,1);
        le->show();
    }
    if(index == 2){
        QDateTimeEdit *de = new QDateTimeEdit(scrollAreaWidgetContents);
        de->setAttribute(Qt::WA_DeleteOnClose);
        de->setCalendarPopup(true);
        de->setDateTime(QDateTime::currentDateTime());
        t->addWidget(de,0,3,1,1);
        de->show();
    }
    QToolButton *tb = qobject_cast<QToolButton *>(t->itemAtPosition(0,4)->widget());
    if(index == 5 || index == 6){
        tb->setEnabled(true);
        tb->setProperty("curIndex",index);
        switch(index){
        case 5: tb->setIcon(QIcon(":/22x22/book/ico/bookcase_22x22.png")); break;
        case 6: tb->setIcon(QIcon(":/22x22/users/ico/system-users_22x22.png")); break;
        }
    }else{
        tb->setEnabled(false);
        tb->setIcon(QIcon(":/32x32/journal/ico/journal_32x32.png"));
    }
}

int FilterJournalHistoryMoved::sizeConditionName(QList< QList<ItemComboBox> > itemCondition, int conditionGroupFrom, int conditionGroupTo)
{
    int sizeCondition = 0;
    for(int i = 0;i<itemCondition.size()-1;i++)
        for(int j = conditionGroupFrom;j<=conditionGroupTo;j++){
            if(itemCondition[i].value(j).name.length() > sizeCondition)
                sizeCondition = itemCondition[i].value(j).name.length();
        }
    return sizeCondition;
}

void FilterJournalHistoryMoved::additionalButton_clicked()
{
    int index = sender()->property("curIndex").toInt();
    switch(index){
    case 5:{
        CeditTable *et = new CeditTable(this,QString("cause"),true);
        et->setWindowTitle(tr("Причина"));
        et->setProperty("curRow",sender()->property("curRow").toInt());
        connect(et,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        et->exec();
    }
        break;
    case 6:{
        AddExistingUser *aeu = new AddExistingUser(this,m_organizationId);
        aeu->setWindowTitle(tr("Выбор исполнителя"));
        aeu->setAddButtonName(tr("Выбрать"));
        aeu->setProperty("curRow",sender()->property("curRow").toInt());
        connect(aeu,SIGNAL(userAdded(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        aeu->exec();
    }
        break;
    }
}

void FilterJournalHistoryMoved::setSelectedDataAdditionalFilter(const QList<QVariant> datas)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QLineEdit *le = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
    le->setText(datas.value(1).toString());
    le->setProperty("data",datas.value(0));
}

void FilterJournalHistoryMoved::conditionAdditionalFilterChanget(const QString &index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QWidget *w = t->itemAtPosition(0,3)->widget();
    QWidget *w1 = t->itemAtPosition(0,4)->widget();
    if(index == tr("Не указано")){
        w->setEnabled(false);
        w1->setEnabled(false);
    }else
        w->setEnabled(true);
}

void FilterJournalHistoryMoved::on_setButton_clicked()
{
    QString filter = "";
    for(int i = 0; i < rowsFilterVerticalLayout->count();i++){
        QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(i)->layout());
        QComboBox *filterField = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
        QComboBox *condition = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());
        if(filterField->currentIndex() != 2){
            if(condition->currentText() != tr("Не указано")){
                QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
                if(valueFilterField->text().isEmpty() || valueFilterField->text().isNull())
                    continue;
            }
        }
        if(!filter.isEmpty()){
            QComboBox *operators = qobject_cast<QComboBox *>(t->itemAtPosition(0,0)->widget());
            filter += operators->itemData(operators->currentIndex()).toString();
        }else if(i == 0 && !m_filterIsEmpty)
            filter += "(";
        switch(filterField->currentIndex()){
        case 2:{
            QDateTimeEdit *valueFilterField = qobject_cast<QDateTimeEdit *>(t->itemAtPosition(0,3)->widget());
            filter += QString(condition->itemData(condition->currentIndex()).toString())
                    .arg(filterField->itemData(filterField->currentIndex()).toString())
                    .arg("'"+valueFilterField->dateTime().toString("yyyy.MM.dd hh:mm:ss")+"'");
        }
            break;
        case 5: case 6:{
            QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
            filter += QString(condition->itemData(condition->currentIndex()).toString())
                    .arg(filterField->itemData(filterField->currentIndex()).toString())
                    .arg(valueFilterField->property("data").toInt());
        }
            break;
        case -1: break;
        default:
            QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
            if(condition->currentText() != tr("Не указано"))
                filter += QString(condition->itemData(condition->currentIndex()).toString())
                        .arg(filterField->itemData(filterField->currentIndex()).toString())
                        .arg("'"+valueFilterField->text()+"'");
            else
                filter += QString(condition->itemData(condition->currentIndex()).toString())
                        .arg(filterField->itemData(filterField->currentIndex()).toString());
        }
    }
    if(!m_filterIsEmpty && !filter.isEmpty())
        filter += ")";
    emit setFilter(filter);
    this->accept();
}
