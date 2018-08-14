#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include "headers/deviceimportcomparisonworkplace.h"
#include "headers/devicereportparser.h"
#include "plugins/lineedittwobutton/lineedittwobutton.h"
#include "headers/selectworkplace.h"

DeviceImportComparisonWorkPlace::DeviceImportComparisonWorkPlace(QWidget *parent, const QString &reportFile, int depFilter, const QMap<QString,QVariant> comparisonWP) :
    QDialog(parent),
    m_depFilter(depFilter),
    comparison(comparisonWP)
{
    setupUi(this);
    if(reportFile.isEmpty()){
        QMessageBox::warning(this, tr("Ошибка"), tr("Отсутствует файл отчёта!!!"), "Закрыть");
        reject();
    }
    curRow = 0;

    if(comparison.isEmpty()){
        DeviceReportParser handler(true);
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
            LineEditTwoButton *w2 = new LineEditTwoButton(this);
            w2->setAttribute(Qt::WA_DeleteOnClose);
            w2->setReadOnly(true);
            w2->setProperty("curRow",i+1);
            w2->setVisibleClearButtron(false);
            connect(w2,SIGNAL(runButtonClicked()),this,SLOT(selectWorkPlaceClicked()));
            gridElements->addWidget(w2,i+1,2,1,1);
            w2->show();
        }
    }else{
        bool ok;
        QSqlQuery query;
        QMap<QString,QVariant>::const_iterator  i = comparison.constBegin();
        int row = 1;
        while (i != comparison.constEnd()) {
            QVariant value = i.value();
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
            LineEditTwoButton *w2 = new LineEditTwoButton(this);
            w2->setAttribute(Qt::WA_DeleteOnClose);
            w2->setReadOnly(true);
            w2->setProperty("curRow",row);
            w2->setVisibleClearButtron(false);
            ok = query.exec(QString("SELECT Name FROM  workerplace WHERE CodWorkerPlace = %1").arg(value.toInt()));
            if(!ok){
                QMessageBox::warning(this, tr("Ошибка!!!"),
                                     tr("Не удалось получить имена сопоставленных рабочих мест:\n%1")
                                     .arg(query.lastError().text()),
                                     tr("Закрыть"));
                return;
            }
            query.next();
            w2->setText(query.value(0).toString());
            w2->setData(i.value());
            connect(w2,SIGNAL(runButtonClicked()),this,SLOT(selectWorkPlaceClicked()));
            gridElements->addWidget(w2,row,2,1,1);
            w2->show();
            ++i;
            row++;
        }
    }

}

void DeviceImportComparisonWorkPlace::changeEvent(QEvent *e)
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

void DeviceImportComparisonWorkPlace::selectWorkPlaceClicked()
{
    curRow = sender()->property("curRow").toInt();
    SelectWorkPlace *swp = new SelectWorkPlace(this,m_depFilter);
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(setComparisonWP(int,QString)));
    swp->setAttribute(Qt::WA_DeleteOnClose);
    swp->exec();
}

void DeviceImportComparisonWorkPlace::setComparisonWP(int id, const QString &name)
{
    LineEditTwoButton *letb = qobject_cast<LineEditTwoButton *>(gridElements->itemAtPosition(curRow,2)->widget());
    QLabel *lab = qobject_cast<QLabel *>(gridElements->itemAtPosition(curRow,0)->widget());
    letb->setText(name);
    letb->setData(id);
    lab->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
}

void DeviceImportComparisonWorkPlace::on_buttonAutoComparison_clicked()
{
    bool ok = false;
    QSqlQuery query;
    for(int i = 1; i < gridElements->rowCount(); i++){
        QLineEdit *le = qobject_cast<QLineEdit *>(gridElements->itemAtPosition(i,1)->widget());
        ok = query.exec(QString("SELECT CodWorkerPlace,Name FROM workerplace WHERE Name = '%1' AND CodDepartment IN ("
                           "SELECT id FROM departments WHERE id IN ("
                           "SELECT c.id FROM (SELECT * FROM departments) n, "
                           "(SELECT * FROM tree) t, "
                           "(SELECT * FROM departments) c "
                           "WHERE n.id = %2 AND n.id = t.parent_id "
                           "AND t.id = c.id))")
                   .arg(le->text()).arg(m_depFilter));
        if(!ok){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Не удалось выполнить автосопоставление:\n%1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
            return;
        }
        if(query.size() == 1){
            query.next();
            LineEditTwoButton *letb = qobject_cast<LineEditTwoButton *>(gridElements->itemAtPosition(i,2)->widget());
            QLabel *lab = qobject_cast<QLabel *>(gridElements->itemAtPosition(i,0)->widget());
            letb->setText(query.value(1).toString());
            letb->setData(query.value(0));
            lab->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
        }
    }
}

void DeviceImportComparisonWorkPlace::on_buttonApplay_clicked()
{
    for(int i = 1; i < gridElements->rowCount(); i++){
        QLineEdit *le = qobject_cast<QLineEdit *>(gridElements->itemAtPosition(i,1)->widget());
        LineEditTwoButton *letb = qobject_cast<LineEditTwoButton *>(gridElements->itemAtPosition(i,2)->widget());
        if(letb->text().isNull() || letb->text().isEmpty()){
            QMessageBox::warning(this, tr("Ошибка!!!"),
                                 tr("Ни все сопоставления выполненны!!!"),
                                 tr("Закрыть"));
            comparison.clear();
            return;
        }
        comparison[le->text()] = letb->data();
    }
    emit comparisonResult(comparison);
    accept();
}
