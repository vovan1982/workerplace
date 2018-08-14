#include <QtSql>
#include <QDebug>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include "headers/devicereportparser.h"
#include "headers/deviceimportcomparisontypelicense.h"

DeviceImportComparisonTypeLicense::DeviceImportComparisonTypeLicense(QWidget *parent, const QString &reportFile, const QMap<QString,QVariant> comparisonTypeLic) :
    QDialog(parent),
    comparison(comparisonTypeLic)
{
    setupUi(this);

    if(reportFile.isEmpty()){
        QMessageBox::warning(this, tr("Ошибка"), tr("Отсутствует файл отчёта!!!"), "Закрыть");
        reject();
    }
    curRow = 0;

    if(comparison.isEmpty()){
        DeviceReportParser handler(false,false,true);
        QFile             file(reportFile);
        QXmlInputSource   source(&file);
        QXmlSimpleReader  reader;

        reader.setContentHandler(&handler);
        reader.parse(source);

        for(int i = 0; i < handler.attrValues().count(); i++){
            QLabel *w0 = new QLabel(this);
            w0->setAttribute(Qt::WA_DeleteOnClose);
            w0->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
            w0->setProperty("curRow",i+1);
            gridElements->addWidget(w0,i+1,0,1,1,Qt::AlignHCenter);
            w0->show();
            QLineEdit *w1 = new QLineEdit(this);
            w1->setAttribute(Qt::WA_DeleteOnClose);
            w1->setReadOnly(true);
            w1->setProperty("curRow",i+1);
            w1->setText(handler.attrValues().value(i));
            gridElements->addWidget(w1,i+1,1,1,1);
            w1->show();
            QComboBox *w2 = new QComboBox(this);
            w2->setAttribute(Qt::WA_DeleteOnClose);
            w2->setProperty("curRow",i+1);
            populateCBox("CodTypeLicense","typelicense","",tr("<Выберите тип лицензии>"),w2);
            connect(w2,SIGNAL(currentIndexChanged(int)),this,SLOT(setComparisonTypeLic(int)));
            gridElements->addWidget(w2,i+1,2,1,1);
            w2->show();
        }
    }else{
        QMap<QString,QVariant>::const_iterator  i = comparison.constBegin();
        int row = 1;
        while (i != comparison.constEnd()) {
            QLabel *w0 = new QLabel(this);
            w0->setAttribute(Qt::WA_DeleteOnClose);
            w0->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
            w0->setProperty("curRow",row);
            gridElements->addWidget(w0,row,0,1,1,Qt::AlignHCenter);
            w0->show();
            QLineEdit *w1 = new QLineEdit(this);
            w1->setAttribute(Qt::WA_DeleteOnClose);
            w1->setReadOnly(true);
            w1->setProperty("curRow",row);
            w1->setText(i.key());
            gridElements->addWidget(w1,row,1,1,1);
            w1->show();
            QComboBox *w2 = new QComboBox(this);
            w2->setAttribute(Qt::WA_DeleteOnClose);
            w2->setProperty("curRow",row);
            populateCBox("CodTypeLicense","typelicense","",tr("<Выберите тип лицензии>"),w2);
            w2->setCurrentIndex(w2->findData(i.value()));
            connect(w2,SIGNAL(currentIndexChanged(int)),this,SLOT(setComparisonTypeLic(int)));
            gridElements->addWidget(w2,row,2,1,1);
            w2->show();
            ++i;
            row++;
        }
    }
}

void DeviceImportComparisonTypeLicense::changeEvent(QEvent *e)
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

void DeviceImportComparisonTypeLicense::populateCBox(const QString &idName, const QString &tableName,
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

void DeviceImportComparisonTypeLicense::setComparisonTypeLic(int index)
{
    curRow = sender()->property("curRow").toInt();
    QLabel *lab = qobject_cast<QLabel *>(gridElements->itemAtPosition(curRow,0)->widget());
    if(index > 0)
        lab->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
    else
        lab->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
}

void DeviceImportComparisonTypeLicense::on_buttonApplay_clicked()
{
    for(int i = 1; i < gridElements->rowCount(); i++){
        QLineEdit *le = qobject_cast<QLineEdit *>(gridElements->itemAtPosition(i,1)->widget());
        QComboBox *cb = qobject_cast<QComboBox *>(gridElements->itemAtPosition(i,2)->widget());
        if(cb->currentIndex() <= 0){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Ни все сопоставления выполненны!!!"),
                                 tr("Закрыть"));
            comparison.clear();
            return;
        }
        comparison[le->text()] = cb->itemData(cb->currentIndex());
    }
    emit comparisonResult(comparison);
    accept();
}
