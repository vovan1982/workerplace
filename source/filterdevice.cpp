#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include "headers/filterdevice.h"
#include "headers/producers.h"
#include "headers/providers.h"
#include "headers/edittable.h"
#include "headers/selectworkplace.h"
#include "headers/selectdepartment.h"

FilterDevice::FilterDevice(QWidget *parent, bool locationIsSet, bool onlyHardwareMode, bool onlyOrgTexMode) :
    QDialog(parent),
    m_locationIsSet(locationIsSet),
    m_onlyHardwareMode(onlyHardwareMode),
    m_onlyOrgTexMode(onlyOrgTexMode)
{
    setupUi(this);
    filterType->addItem(tr("<Выберите вид устройства>"));
    filterType->addItem(tr("Оргтехника"),1);
    filterType->addItem(tr("Комплектующее"),0);
    if(onlyHardwareMode){
        populateCBox("CodTypeDevice","typedevice","Type = 0",tr("<Выберите тип устройства>"),filterTypeDevice);
        filterType->setCurrentIndex(2);
        filterType->setEnabled(false);
    }else if(onlyOrgTexMode){
        populateCBox("CodTypeDevice","typedevice","Type = 1",tr("<Выберите тип устройства>"),filterTypeDevice);
        filterType->setCurrentIndex(1);
        filterType->setEnabled(false);
    }else
        populateCBox("CodTypeDevice","typedevice","",tr("<Выберите тип устройства>"),filterTypeDevice);
    if(locationIsSet){
        groupBox->setVisible(false);
        label->setVisible(false);
        filterWorkPlace->setVisible(false);
    }
    selectDepIsFirm = false;
    items = initItemAdditionalFilter();
    sizeConditionNames = sizeConditionName(items,2,7);
    insertRowInAdditionalFilter(0);
}

void FilterDevice::changeEvent(QEvent *e)
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

void FilterDevice::addButton_clicked()
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

void FilterDevice::delFilterRow()
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
//    if(curRow == 0){
//        QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(0)->layout());
//        QComboBox *cbox = qobject_cast<QComboBox *>(t->itemAtPosition(0,0)->widget());
//        cbox->setEnabled(false);
//        cbox->setCurrentIndex(1);
//    }
}

QList< QList<ItemComboBox> > FilterDevice::initItemAdditionalFilter()
{
    ItemComboBox item;
    QList<ItemComboBox> groupItem;
    QList< QList<ItemComboBox> > initItems;

    // group item index 0
    item.name = tr("или"); item.data = " OR "; groupItem<<item;
    item.name = tr("и"); item.data = " AND "; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group group item index 1
    item.name = tr("Наименование"); item.data = "dev.Name"; groupItem<<item;
    item.name = tr("Инв №"); item.data = "dev.InventoryN"; groupItem<<item;
    item.name = tr("Серийный №"); item.data = "dev.SerialN"; groupItem<<item;
    item.name = tr("Производитель"); item.data = "dev.CodProducer"; groupItem<<item;
    item.name = tr("Поставщик"); item.data = "dev.CodProvider"; groupItem<<item;
    item.name = tr("Дата покупки"); item.data = "dev.DatePurchase"; groupItem<<item;
    item.name = tr("Дата оприходования"); item.data = "dev.DatePosting"; groupItem<<item;
    item.name = tr("Конец гарантии"); item.data = "dev.EndGuarantee"; groupItem<<item;
    item.name = tr("Стоимость"); item.data = "dev.Price"; groupItem<<item;
    item.name = tr("Состояние"); item.data = "dev.CodState"; groupItem<<item;
    item.name = tr("Примечание"); item.data = "dev.Note"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 2
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 3
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Содержит"); item.data = "%1 REGEXP %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group item index 4
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
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
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = ">"; item.data = "%1 > %2"; groupItem<<item;
    item.name = "<"; item.data = "%1 < %2"; groupItem<<item;
    item.name = ">="; item.data = "%1 >= %2"; groupItem<<item;
    item.name = "<="; item.data = "%1 <= %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    return initItems;
}

void FilterDevice::insertRowInAdditionalFilter(int index)
{
    QGridLayout *rg = new QGridLayout();
    rg->setSpacing(6);
    rowsFilterVerticalLayout->insertLayout(index,rg);
    QComboBox *w0 = new QComboBox(scrollAreaWidgetContents);
    w0->setAttribute(Qt::WA_DeleteOnClose);
    populateComboBoxAdditionalFilter(w0,items,0);
    rg->addWidget(w0,0,0,1,1);
    w0->show();
//    if(index == 0){w0->setEnabled(false); w0->setCurrentIndex(1);}
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
    w4->setIcon(QIcon(":/22x22/computers/ico/computer_22x22.png"));
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

void FilterDevice::populateComboBoxAdditionalFilter(QComboBox *cbox, const QList< QList<ItemComboBox> > itemCBox, int groupItemIndex)
{
    if(itemCBox.size()-1<groupItemIndex) return;
    cbox->clear();
    for (int i = 0;i<itemCBox[groupItemIndex].size();i++)
        cbox->addItem(itemCBox[groupItemIndex].value(i).name,itemCBox[groupItemIndex].value(i).data);
}

void FilterDevice::populateCBox(const QString &idName, const QString &tableName,
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

void FilterDevice::fieldAdditionalFilterChanget(int index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QComboBox *cbox = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());

    switch(index){
    case 0:
        populateComboBoxAdditionalFilter(cbox,items,3);
        break;
    case 1: case 2: case 3: case 4:
        populateComboBoxAdditionalFilter(cbox,items,4);
        break;
    case 5: case 6: case 7:
        populateComboBoxAdditionalFilter(cbox,items,7);
        break;
    case 8:
        populateComboBoxAdditionalFilter(cbox,items,6);
        break;
    case 9:
        populateComboBoxAdditionalFilter(cbox,items,2);
        break;
    case 10:
        populateComboBoxAdditionalFilter(cbox,items,5);
        break;
    }
    QWidget *w = t->itemAtPosition(0,3)->widget();
    t->removeWidget(w);
    w->close();
    if((index >= 0 && index <= 4) || index == 9 || index == 10){
        QLineEdit *le = new QLineEdit(scrollAreaWidgetContents);
        le->setAttribute(Qt::WA_DeleteOnClose);
        if(index == 10 || (index >=0 && index <=2))
            le->setReadOnly(false);
        else
            le->setReadOnly(true);
        t->addWidget(le,0,3,1,1);
        le->show();
    }
    if(index == 5 || index == 6 || index == 7){
        QDateEdit *de = new QDateEdit(scrollAreaWidgetContents);
        de->setAttribute(Qt::WA_DeleteOnClose);
        de->setDisplayFormat("dd.MM.yyyy");
        de->setCalendarPopup(true);
        de->setDate(QDate::currentDate());
        t->addWidget(de,0,3,1,1);
        de->show();
    }
    if(index == 8){
        QDoubleSpinBox *dsb = new QDoubleSpinBox(scrollAreaWidgetContents);
        dsb->setAttribute(Qt::WA_DeleteOnClose);
        dsb->setMaximum(99999.99);
        dsb->setSingleStep(0.01);
        t->addWidget(dsb,0,3,1,1);
        dsb->show();
    }
    QToolButton *tb = qobject_cast<QToolButton *>(t->itemAtPosition(0,4)->widget());
    if(index == 3 || index == 4 || index == 9){
        tb->setEnabled(true);
        tb->setProperty("curIndex",index);
        switch(index){
        case 3: tb->setIcon(QIcon(":/128x128/producers/ico/producers_128x128.png")); break;
        case 4: tb->setIcon(QIcon(":/128x128/providers/ico/providers_128x128.png")); break;
        case 9: tb->setIcon(QIcon(":/22x22/book/ico/bookcase_22x22.png")); break;
        }
    }else{
        tb->setEnabled(false);
        tb->setIcon(QIcon(":/22x22/computers/ico/computer_22x22.png"));
    }
}

int FilterDevice::sizeConditionName(QList< QList<ItemComboBox> > itemCondition, int conditionGroupFrom, int conditionGroupTo)
{
    int sizeCondition = 0;
    for(int i = 0;i<itemCondition.size()-1;i++)
        for(int j = conditionGroupFrom;j<=conditionGroupTo;j++){
            if(itemCondition[i].value(j).name.length() > sizeCondition)
                sizeCondition = itemCondition[i].value(j).name.length();
        }
    return sizeCondition;
}

void FilterDevice::additionalButton_clicked()
{
    int index = sender()->property("curIndex").toInt();
    switch(index){
    case 3:{
        QDialog *d = new QDialog(this);
        d->setWindowTitle(tr("Производители"));
        QVBoxLayout *layout = new QVBoxLayout;
        Producers *prod = new Producers(d,false,true);
        prod->setAttribute(Qt::WA_DeleteOnClose);
        prod->setProperty("curRow",sender()->property("curRow").toInt());
        prod->setProperty("isTree",0);
        connect(prod,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        connect(prod,SIGNAL(closeProducerWin()),d,SLOT(reject()));
        layout->addWidget(prod);
        d->setLayout(layout);
        d->layout()->setContentsMargins(0,0,0,0);
        prod->show();
        d->exec();
        d->adjustSize();
    }
        break;
    case 4:{
        QDialog *d = new QDialog(this);
        d->setWindowTitle(tr("Поставщики"));
        QVBoxLayout *layout = new QVBoxLayout;
        Providers *prov = new Providers(d,false,true);
        prov->setAttribute(Qt::WA_DeleteOnClose);
        prov->setProperty("curRow",sender()->property("curRow").toInt());
        prov->setProperty("isTree",0);
        connect(prov,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        connect(prov,SIGNAL(closeProviderWin()),d,SLOT(reject()));
        layout->addWidget(prov);
        d->setLayout(layout);
        d->layout()->setContentsMargins(0,0,0,0);
        prov->show();
        d->exec();
        d->adjustSize();
    }
        break;
    case 8:{
        CeditTable *et = new CeditTable(this,QString("statedev"),true);
        et->setWindowTitle(tr("Cостояния устройства"));
        et->setProperty("curRow",sender()->property("curRow").toInt());
        et->setProperty("isTree",0);
        connect(et,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        et->exec();
    }
        break;
    }
}

void FilterDevice::setSelectedDataAdditionalFilter(const QList<QVariant> datas)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QLineEdit *le = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
    if(sender()->property("isTree").toInt() == 0){
        le->setText(datas.value(1).toString());
        le->setProperty("data",datas.value(0));
    }else{
        le->setText(datas.value(0).toString());
        le->setProperty("data",datas.value(1));
    }
}

void FilterDevice::conditionAdditionalFilterChanget(const QString &index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QWidget *w = t->itemAtPosition(0,3)->widget();
    QWidget *w1 = t->itemAtPosition(0,4)->widget();
    if(index == tr("Не указано")){
        w->setEnabled(false);
        w1->setEnabled(false);
    }else{
        w->setEnabled(true);
        QComboBox *w2 = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
        if(w2->currentIndex() == 3 || w2->currentIndex() == 4)
            w1->setEnabled(true);
    }
}

void FilterDevice::on_groupBox_clicked(bool checked)
{
    filterWorkPlace->setDisabled(checked);
    if(checked){
        filterWorkPlace->clearButton_clicked();
    }else
        filterDepartment->clearButton_clicked();
}

void FilterDevice::on_filterDepartment_runButtonClicked()
{
    SelectDepartment *sd = new SelectDepartment(this);
    sd->setProperty("objectName","filterDepartment");
    connect(sd,SIGNAL(addDepartment(int,QString,int)),this,SLOT(setSelectedData(int,QString,int)));
    sd->exec();
}

void FilterDevice::on_filterWorkPlace_runButtonClicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this,0,true);
    swp->setProperty("objectName","filterWorkPlace");
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(setSelectedData(int,QString,int)));
    swp->exec();
}

void FilterDevice::on_filterType_currentIndexChanged(int index)
{
    if(index == 1)
        populateCBox("CodTypeDevice","typedevice","Type = 1",tr("<Выберите тип устройства>"),filterTypeDevice);
    else if(index == 2)
        populateCBox("CodTypeDevice","typedevice","Type = 0",tr("<Выберите тип устройства>"),filterTypeDevice);
    else
        populateCBox("CodTypeDevice","typedevice","",tr("<Выберите тип устройства>"),filterTypeDevice);
}

void FilterDevice::setSelectedData(int id, const QString &name, int orgId)
{
    if(sender()->property("objectName").toString() == "filterDepartment"){
        filterDepartment->setText(name);
        filterDepartment->setData(id);
        if(id == orgId)
            selectDepIsFirm = true;
        else
            selectDepIsFirm = false;
    }
    if(sender()->property("objectName").toString() == "filterWorkPlace"){
        filterWorkPlace->setText(name);
        filterWorkPlace->setData(id);
    }
}

void FilterDevice::on_setButton_clicked()
{
    QString filter = "";
    QString primaryFilter = "";
    if(groupBox->isChecked()){
        if(!filterDepartment->text().isNull() && !filterDepartment->text().isEmpty()){
            if(!dontShowChild->isChecked()){
                primaryFilter = QString("dev.CodWorkerPlace IN (SELECT CodWorkerPlace FROM workerplace WHERE CodDepartment IN ("
                                 "SELECT c.id FROM (SELECT * FROM departments) n, (SELECT * FROM tree) t, "
                                 "(SELECT * FROM departments) c WHERE n.id = %1 "
                                 "AND n.id = t.parent_id AND t.id = c.id))")
                        .arg(filterDepartment->data().toInt());
            }else
                primaryFilter = QString("dev.CodWorkerPlace IN (SELECT CodWorkerPlace FROM workerplace WHERE CodDepartment = %1)")
                        .arg(filterDepartment->data().toInt());
        }
    }else if (!m_locationIsSet && !filterWorkPlace->text().isNull() && !filterWorkPlace->text().isEmpty())
        primaryFilter = QString("dev.CodWorkerPlace = %1").arg(filterWorkPlace->data().toInt());

    if(filterType->currentIndex() > 0){
        if(m_locationIsSet)
            filter += "(";
        filter += QString("dev.Type = %1").arg(filterType->itemData(filterType->currentIndex()).toInt());
    }
    if(filterTypeDevice->currentIndex() > 0){
        if(!filter.isEmpty())
            filter += " AND ";
        else if(m_locationIsSet)
            filter += "(";
        filter += QString("dev.CodTypeDevice = %1").arg(filterTypeDevice->itemData(filterTypeDevice->currentIndex()).toInt());
    }

    if(additionalFilteringRules->isChecked()){
        for(int i = 0; i < rowsFilterVerticalLayout->count();i++){
            QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(i)->layout());
            QComboBox *filterField = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
            QComboBox *condition = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());
            if(filterField->currentIndex() != 5 && filterField->currentIndex() != 6 && filterField->currentIndex() != 7 && filterField->currentIndex() != 8){
                if(condition->currentText() != tr("Не указано")){
                    QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
                    if(valueFilterField->text().isEmpty() || valueFilterField->text().isNull())
                        continue;
                }
            }
            if(!filter.isEmpty()){
                QComboBox *operators = qobject_cast<QComboBox *>(t->itemAtPosition(0,0)->widget());
                filter += operators->itemData(operators->currentIndex()).toString();
            }else if(i == 0 && m_locationIsSet)
                filter += "(";
            switch(filterField->currentIndex()){
            case 5: case 6: case 7:{
                QDateEdit *valueFilterField = qobject_cast<QDateEdit *>(t->itemAtPosition(0,3)->widget());
                if(condition->currentText() != tr("Не указано"))
                    filter += QString(condition->itemData(condition->currentIndex()).toString())
                            .arg(filterField->itemData(filterField->currentIndex()).toString())
                            .arg("'"+valueFilterField->date().toString("yyyy.MM.dd")+"'");
                else
                    filter += QString(condition->itemData(condition->currentIndex()).toString())
                            .arg(filterField->itemData(filterField->currentIndex()).toString());
            }
            break;
            case 8:{
                QDoubleSpinBox *valueFilterField = qobject_cast<QDoubleSpinBox *>(t->itemAtPosition(0,3)->widget());
                filter += QString(condition->itemData(condition->currentIndex()).toString())
                        .arg(filterField->itemData(filterField->currentIndex()).toString())
                        .arg(valueFilterField->value());
            }
            break;
            case 3: case 4: case 9:{
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
    }
    if(m_locationIsSet && !filter.isEmpty())
        filter += ")";

    if(!filter.isNull() && !filter.isEmpty()){
        if(showDeviceComposition->isChecked() && showDeviceParent->isChecked()){
            if (!primaryFilter.isNull() && !primaryFilter.isEmpty())
                primaryFilter.prepend(" AND ");
            primaryFilter.prepend("dev.id IN "
                                  "(SELECT c.id FROM device n, devtree t, device c WHERE "
                                  "n.id IN (SELECT p.id FROM device n, devtree t, device p WHERE "
                                  "n.id IN (SELECT id FROM device dev WHERE "+filter+") "
                                  "AND n.id = t.id AND t.parent_id = p.id GROUP BY p.id) "
                                  "AND n.id = t.parent_id AND t.id = c.id)");
        }
        if(showDeviceComposition->isChecked() && !showDeviceParent->isChecked()){
            if (!primaryFilter.isNull() && !primaryFilter.isEmpty())
                primaryFilter.prepend(" AND ");
            primaryFilter.prepend("dev.id IN ( SELECT c.id FROM device n, devtree t, device c WHERE "
                                  "n.id IN  (SELECT id FROM device dev WHERE "+filter+") "
                                  "AND n.id = t.parent_id AND t.id = c.id )");
        }
        if(!showDeviceComposition->isChecked() && showDeviceParent->isChecked()){
            if (!primaryFilter.isNull() && !primaryFilter.isEmpty())
                primaryFilter.prepend(" AND ");
            primaryFilter.prepend("dev.id IN "
                                  "(SELECT p.id FROM device n, devtree t, device p WHERE "
                                  "n.id IN (SELECT id FROM device dev WHERE "+filter+") "
                                  "AND n.id = t.id AND t.parent_id = p.id GROUP BY p.id)");
        }
        if(!showDeviceComposition->isChecked() && !showDeviceParent->isChecked()){
            if (!primaryFilter.isNull() && !primaryFilter.isEmpty())
                primaryFilter.append(" AND ");
            primaryFilter.append(filter);
        }
    }
    if (!primaryFilter.isNull() && !primaryFilter.isEmpty())
        emit setFilter(primaryFilter);
    this->accept();
}

void FilterDevice::on_additionalFilteringRules_clicked(bool checked)
{
    if(!checked){
        if(!m_onlyHardwareMode && !m_onlyOrgTexMode)
            filterType->setCurrentIndex(0);
        filterTypeDevice->setCurrentIndex(0);
    }
}
