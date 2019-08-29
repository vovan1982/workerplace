#include <QDebug>
#include <QSpinBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include "headers/selectpo.h"
#include "headers/producers.h"
#include "headers/providers.h"
#include "headers/edittable.h"
#include "headers/selectdevice.h"
#include "headers/filterlicense.h"
#include "headers/selectworkplace.h"
#include "headers/selectdepartment.h"

FilterLicense::FilterLicense(QWidget *parent, QMap<int, QString> organizations, int curOrgId) :
    QDialog(parent),
    m_curOrgId(curOrgId)
{
    setupUi(this);
    organization->addItem(tr("<Выберите организацию>"),0);
    QMapIterator<int,QString> organizationsIterator(organizations);
    while(organizationsIterator.hasNext()){
        organizationsIterator.next();
        organization->addItem(organizationsIterator.value(),organizationsIterator.key());
    }
    if(curOrgId > 0){
        organization->setCurrentIndex(organization->findData(curOrgId));
        organization->setEnabled(false);
    }
    items = initItemAdditionalFilter();
    sizeConditionNames = sizeConditionName(items,2,7);
    insertRowInAdditionalFilter(0);
}

void FilterLicense::changeEvent(QEvent *e)
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

void FilterLicense::addButton_clicked()
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

void FilterLicense::delFilterRow()
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
}

void FilterLicense::insertRowInAdditionalFilter(int index)
{
    QGridLayout *rg = new QGridLayout();
    rg->setSpacing(6);
    rowsFilterVerticalLayout->insertLayout(index,rg);
    QComboBox *w0 = new QComboBox(scrollAreaWidgetContents);
    w0->setAttribute(Qt::WA_DeleteOnClose);
    populateComboBoxAdditionalFilter(w0,items,0);
    rg->addWidget(w0,0,0,1,1);
    w0->show();
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
    populateComboBoxAdditionalFilter(w2,items,7);
    w2->setMinimumContentsLength(sizeConditionNames);
    connect(w2,SIGNAL(currentIndexChanged(QString)),this,SLOT(conditionAdditionalFilterChanget(QString)));
    rg->addWidget(w2,0,2,1,1);
    w2->show();
    QLineEdit *w3 = new QLineEdit(scrollAreaWidgetContents);
    w3->setAttribute(Qt::WA_DeleteOnClose);
    w3->setReadOnly(true);
    rg->addWidget(w3,0,3,1,1);
    w3->show();
    QToolButton *w4 = new QToolButton(scrollAreaWidgetContents);
    w4->setIcon(QIcon(":/128x128/software/ico/software_128x128.png"));
    w4->setProperty("curRow",index);
    w4->setProperty("curIndex",0);
    w4->setAttribute(Qt::WA_DeleteOnClose);
    connect(w4,SIGNAL(clicked()),this,SLOT(additionalButton_clicked()));
    rg->addWidget(w4,0,4,1,1,Qt::AlignBottom);
    w4->show();
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

QList< QList<ItemComboBox> > FilterLicense::initItemAdditionalFilter()
{
    ItemComboBox item;
    QList<ItemComboBox> groupItem;
    QList< QList<ItemComboBox> > initItems;

    // group 0
    item.name = tr("или"); item.data = " OR "; groupItem<<item;
    item.name = tr("и"); item.data = " AND "; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 1
    item.name = tr("Наименование"); item.data = "l.CodPO"; groupItem<<item;
    item.name = tr("Производитель"); item.data = "producer.CodProducer"; groupItem<<item;
    item.name = tr("Версия"); item.data = "l.VersionN"; groupItem<<item;
    item.name = tr("Инв №"); item.data = "l.InvN"; groupItem<<item;
    item.name = tr("Тип лицензии"); item.data = "l.CodTypeLicense"; groupItem<<item;
    item.name = tr("Состояние"); item.data = "l.CodStatePO"; groupItem<<item;
    item.name = tr("Примечание"); item.data = "l.Note"; groupItem<<item;
    item.name = tr("Рег. Имя"); item.data = "l.RegName"; groupItem<<item;
    item.name = tr("Рег. Ключ"); item.data = "l.RegKey"; groupItem<<item;
    item.name = tr("Рег. E-Mail"); item.data = "l.RegMail"; groupItem<<item;
    item.name = tr("Кол-во лицензий"); item.data = "l.KolLicense"; groupItem<<item;
    item.name = tr("Дата истечения лицензии"); item.data = "l.DateEndLicense"; groupItem<<item;
    item.name = tr("Поставщик"); item.data = "l.CodProvide"; groupItem<<item;
    item.name = tr("Дата покупки"); item.data = "l.DatePurchase"; groupItem<<item;
    item.name = tr("Стоимость"); item.data = "l.Price"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 2
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 3
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 4
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Содержит"); item.data = "%1 REGEXP %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 5
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = ">"; item.data = "%1 > %2"; groupItem<<item;
    item.name = "<"; item.data = "%1 < %2"; groupItem<<item;
    item.name = ">="; item.data = "%1 >= %2"; groupItem<<item;
    item.name = "<="; item.data = "%1 <= %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 6
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = ">"; item.data = "%1 > %2"; groupItem<<item;
    item.name = "<"; item.data = "%1 < %2"; groupItem<<item;
    item.name = ">="; item.data = "%1 >= %2"; groupItem<<item;
    item.name = "<="; item.data = "%1 <= %2"; groupItem<<item;
    item.name = tr("Не указано"); item.data = "(%1 IS NULL OR %1 = '')"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    // group 7
    item.name = "="; item.data = "%1 = %2"; groupItem<<item;
    item.name = tr("Содержит"); item.data = "%1 REGEXP %2"; groupItem<<item;
    initItems<<groupItem; groupItem.clear();

    return initItems;
}

void FilterLicense::populateComboBoxAdditionalFilter(QComboBox *cbox, const QList< QList<ItemComboBox> > itemCBox, int groupItemIndex)
{
    if(itemCBox.size()-1<groupItemIndex) return;
    cbox->clear();
    for (int i = 0;i<itemCBox[groupItemIndex].size();i++)
        cbox->addItem(itemCBox[groupItemIndex].value(i).name,itemCBox[groupItemIndex].value(i).data);
}

void FilterLicense::fieldAdditionalFilterChanget(int index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QComboBox *cbox = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());

    QWidget *w = t->itemAtPosition(0,3)->widget();
    t->removeWidget(w);
    w->close();
    if((index >= 0 && index <= 9) || index == 12){
        QLineEdit *le = new QLineEdit(scrollAreaWidgetContents);
        le->setAttribute(Qt::WA_DeleteOnClose);
        if(index == 2 || index == 3 || (index >=6 && index <=9))
            le->setReadOnly(false);
        else
            le->setReadOnly(true);
        t->addWidget(le,0,3,1,1);
        le->show();
    }
    if(index == 10){
        QSpinBox *sb = new QSpinBox(scrollAreaWidgetContents);
        sb->setAttribute(Qt::WA_DeleteOnClose);
        sb->setMaximum(999);
        t->addWidget(sb,0,3,1,1);
        sb->show();
    }
    if(index == 11 || index == 13){
        QDateEdit *de = new QDateEdit(scrollAreaWidgetContents);
        de->setAttribute(Qt::WA_DeleteOnClose);
        de->setDisplayFormat("dd.MM.yyyy");
        de->setCalendarPopup(true);
        de->setDate(QDate::currentDate());
        t->addWidget(de,0,3,1,1);
        de->show();
    }
    if(index == 14){
        QDoubleSpinBox *dsb = new QDoubleSpinBox(scrollAreaWidgetContents);
        dsb->setAttribute(Qt::WA_DeleteOnClose);
        dsb->setMaximum(99999.99);
        dsb->setSingleStep(0.01);
        t->addWidget(dsb,0,3,1,1);
        dsb->show();
    }
    QToolButton *tb = qobject_cast<QToolButton *>(t->itemAtPosition(0,4)->widget());
    if(index == 0 || index == 1 || index == 4 || index == 5 || index == 12){
        tb->setEnabled(true);
        tb->setProperty("curIndex",index);
        switch(index){
        case 0: tb->setIcon(QIcon(":/128x128/software/ico/software_128x128.png")); break;
        case 1: tb->setIcon(QIcon(":/128x128/producers/ico/producers_128x128.png")); break;
        case 4: tb->setIcon(QIcon(":/22x22/book/ico/bookcase_22x22.png")); break;
        case 5: tb->setIcon(QIcon(":/22x22/book/ico/bookcase_22x22.png")); break;
        case 12: tb->setIcon(QIcon(":/128x128/providers/ico/providers_128x128.png")); break;
        }
    }else{
        tb->setEnabled(false);
        tb->setIcon(QIcon(":/64x64/licence/ico/licence_64x64.png"));
    }
    switch(index){
    case 0:
        populateComboBoxAdditionalFilter(cbox,items,7);
        break;
    case 1: case 4: case 5: case 10:
        populateComboBoxAdditionalFilter(cbox,items,2);
        break;
    case 2: case 3: case 12:
        populateComboBoxAdditionalFilter(cbox,items,3);
        break;
    case 6: case 7: case 8: case 9:
        populateComboBoxAdditionalFilter(cbox,items,4);
        break;
    case 14:
        populateComboBoxAdditionalFilter(cbox,items,5);
        break;
    case 11: case 13:
        populateComboBoxAdditionalFilter(cbox,items,6);
        break;
    }
}

int FilterLicense::sizeConditionName(QList< QList<ItemComboBox> > itemCondition, int conditionGroupFrom, int conditionGroupTo)
{
    int sizeCondition = 0;
    for(int i = 0;i<itemCondition.size()-1;i++)
        for(int j = conditionGroupFrom;j<=conditionGroupTo;j++){
            if(itemCondition[i].value(j).name.length() > sizeCondition)
                sizeCondition = itemCondition[i].value(j).name.length();
        }
    return sizeCondition;
}

void FilterLicense::additionalButton_clicked()
{
    int index = sender()->property("curIndex").toInt();
    switch(index){
    case 0:{
        SelectPo *sp = new SelectPo(this,"",true);
        sp->setProperty("curRow",sender()->property("curRow").toInt());
        sp->setProperty("isTree",1);
        connect(sp,SIGNAL(itemSelected(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        sp->exec();
    }
        break;
    case 1:{
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
        CeditTable *et = new CeditTable(this,QString("typelicense"),true);
        et->setWindowTitle(tr("Типы лицензий"));
        et->setProperty("curRow",sender()->property("curRow").toInt());
        et->setProperty("isTree",0);
        connect(et,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        et->exec();
    }
        break;
    case 5:{
        CeditTable *et = new CeditTable(this,QString("statepo"),true);
        et->setWindowTitle(tr("Cостояния лицензии"));
        et->setProperty("curRow",sender()->property("curRow").toInt());
        et->setProperty("isTree",0);
        connect(et,SIGNAL(selectedRowData(QList<QVariant>)),this,SLOT(setSelectedDataAdditionalFilter(QList<QVariant>)));
        et->exec();
    }
        break;
    case 12:{
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
    }
}

void FilterLicense::setSelectedDataAdditionalFilter(const QList<QVariant> datas)
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

void FilterLicense::conditionAdditionalFilterChanget(const QString &index)
{
    QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(sender()->property("curRow").toInt())->layout());
    QWidget *w = t->itemAtPosition(0,3)->widget();
    QWidget *w1 = t->itemAtPosition(0,4)->widget();
    if(index == tr("Не указано")){
        w->setEnabled(false);
        w1->setEnabled(false);
    }else if(index == tr("Содержит")){
        QComboBox *w2 = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
        if(w2->currentIndex() == 0){
            QLineEdit *le = qobject_cast<QLineEdit *>(w);
            le->setReadOnly(false);
            w1->setEnabled(false);
        }
    }else{
        QComboBox *w2 = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
        if(w2->currentIndex() == 12 || w2->currentIndex() == 0){
            w1->setEnabled(true);
            QLineEdit *le = qobject_cast<QLineEdit *>(w);
            le->setEnabled(true);
            le->setText("");
            le->setReadOnly(true);
        }else{
            w->setEnabled(true);
        }
    }
}

void FilterLicense::on_organization_currentIndexChanged(int)
{
    filterOaH->setText("");
    filterOaH->setData(QVariant());
}

void FilterLicense::on_noBindingOaH_clicked(bool checked)
{
    filterOaH->setDisabled(checked);
    if(checked)
        filterOaH->clearButton_clicked();
}

void FilterLicense::on_filterOaH_runButtonClicked()
{
    SelectDevice *sd;
    if(organization->currentIndex() > 0)
        sd = new SelectDevice(this,QString("dev.CodOrganization = %1 AND typedevice.Type = 1").arg(organization->itemData(organization->currentIndex()).toInt()));
    else
        sd = new SelectDevice(this,QString("typedevice.Type = 1"));
    sd->setViewRootIsDecorated(false);
    sd->setAttribute(Qt::WA_DeleteOnClose);
    connect(sd,SIGNAL(selectedDevice(QList<QVariant>)),this,SLOT(setSelectedData(QList<QVariant>)));
    sd->exec();
}

void FilterLicense::setSelectedData(QList<QVariant> datas)
{
    QString deviceDisplayName = "";
    if(datas.value(9).toString().isNull() || datas.value(9).toString().isEmpty())
        deviceDisplayName = datas.value(0).toString()+"/"+datas.value(8).toString();
    else
        deviceDisplayName = datas.value(0).toString()+"/"+datas.value(9).toString()+"/"+datas.value(8).toString();
    filterOaH->setText(deviceDisplayName);
    filterOaH->setCursorPosition(0);
    filterOaH->setData(datas.value(1));
}

void FilterLicense::on_setButton_clicked()
{
    QString filter = "";
    if(organization->currentIndex() > 0 && m_curOrgId <= 0){
        filter = QString("l.CodOrganization = %1").arg(organization->itemData(organization->currentIndex()).toInt());
    }
    if(noBindingOaH->isChecked()){
        if(!filter.isEmpty())
            filter += " AND ";
        filter += QString("l.`key` NOT IN (SELECT CodLicense FROM licenseanddevice)");
    }else if(!filterOaH->text().isNull() && !filterOaH->text().isEmpty()){
        if(!filter.isEmpty())
            filter += " AND ";
        filter += QString("l.`key` IN (SELECT CodLicense FROM licenseanddevice WHERE CodDevice = %1)").arg(filterOaH->data().toInt());
    }
    if(additionalFilteringRules->isChecked()){
        for(int i = 0; i < rowsFilterVerticalLayout->count();i++){
            QGridLayout *t = qobject_cast<QGridLayout *>(rowsFilterVerticalLayout->itemAt(i)->layout());
            QComboBox *filterField = qobject_cast<QComboBox *>(t->itemAtPosition(0,1)->widget());
            QComboBox *condition = qobject_cast<QComboBox *>(t->itemAtPosition(0,2)->widget());
            if(filterField->currentIndex() != 10 && filterField->currentIndex() != 11 && filterField->currentIndex() != 13 &&
                    filterField->currentIndex() != 14){
                if(condition->currentText() != tr("Не указано")){
                    QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
                    if(valueFilterField->text().isEmpty() || valueFilterField->text().isNull())
                        continue;
                }
            }
            if(!filter.isEmpty()){
                QComboBox *operators = qobject_cast<QComboBox *>(t->itemAtPosition(0,0)->widget());
                filter += operators->itemData(operators->currentIndex()).toString();
            }
            switch(filterField->currentIndex()){
            case 11: case 13:{
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
            case 10:{
                QSpinBox *valueFilterField = qobject_cast<QSpinBox *>(t->itemAtPosition(0,3)->widget());
                filter += QString(condition->itemData(condition->currentIndex()).toString())
                        .arg(filterField->itemData(filterField->currentIndex()).toString())
                        .arg(valueFilterField->value());
            }
            break;
            case 14:{
                QDoubleSpinBox *valueFilterField = qobject_cast<QDoubleSpinBox *>(t->itemAtPosition(0,3)->widget());
                filter += QString(condition->itemData(condition->currentIndex()).toString())
                        .arg(filterField->itemData(filterField->currentIndex()).toString())
                        .arg(valueFilterField->value());
            }
            break;
            case 0:{
                QLineEdit *valueFilterField = qobject_cast<QLineEdit *>(t->itemAtPosition(0,3)->widget());
                if(condition->currentText() == tr("Содержит")){
                    filter += QString(condition->itemData(condition->currentIndex()).toString())
                            .arg("po.Name")
                            .arg("'"+valueFilterField->text()+"'");
                }else{
                    filter += QString(condition->itemData(condition->currentIndex()).toString())
                            .arg(filterField->itemData(filterField->currentIndex()).toString())
                            .arg(valueFilterField->property("data").toInt());
                }
            }
                break;
            case 1: case 4: case 5: case 12:{
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
    emit setFilter(filter);
    this->accept();
}
