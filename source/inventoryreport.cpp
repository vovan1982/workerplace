#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include "headers/inventoryreport.h"
#include "headers/selectworkplace.h"
#include "headers/selectdepartment.h"
#include "headers/sqltreemodel.h"
#include "headers/workplacemodel.h"
#include "headers/createinventoryreport.h"
#include "headers/inventoryreportworker.h"

InventoryReport::InventoryReport(QWidget *parent) :
    QWidget(parent){
    setupUi(this);
    departmentId = 0;
    workPlaceId = 0;
    populateCBox("id","departments","firm = 1","<Выберите фирму>",firm);
    cInvRep = new CreateInventoryReport(this);
}
void InventoryReport::on_firm_currentIndexChanged(int index)
{
    if(index > 0)
        createButton->setEnabled(true);
    else
        createButton->setEnabled(false);
}

void InventoryReport::on_selectDepButton_clicked()
{
    SelectDepartment *sd = new SelectDepartment(this);
    connect(sd,SIGNAL(addDepartment(int,QString,int)),this,SLOT(setDepartment(int,QString)));
    sd->exec();
}
void InventoryReport::on_selectWPButton_clicked()
{
    SelectWorkPlace *swp = new SelectWorkPlace(this,0,true);
    connect(swp,SIGNAL(addWorkPlace(int,QString,int)),this,SLOT(setWorkPlase(int,QString)));
    swp->exec();
}
void InventoryReport::setWorkPlase(int wpId, const QString &wpNm)
{
    workPlaceId = wpId;
    wpName->setText(wpNm);
    createButton->setEnabled(true);
}

void InventoryReport::setDepartment(int depId, const QString &depNm)
{
    departmentId = depId;
    depName->setText(depNm);
    createButton->setEnabled(true);
}

void InventoryReport::populateCBox(const QString &idName, const QString &tableName,
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
void InventoryReport::populateModDep(int organizationId)
{
    modelDepart->setTable("departments");
    modelDepart->setFilter(QString("id IN (SELECT c.id FROM "
                                   "(SELECT * FROM departments) n, "
                                   "(SELECT * FROM tree) t, "
                                   "(SELECT * FROM departments) c "
                                   "WHERE n.id = %1 AND n.id = t.parent_id AND t.id = c.id) ")
                           .arg(organizationId));
    modelDepart->setHideRoot(true);
    modelDepart->select();
}
void InventoryReport::populateWPModel(const QString &filter)
{
    wpModel->setFilter(filter);
    wpModel->select();
}

QList<InventoryReport::departments> InventoryReport::depList(const QModelIndex &parent, QString depName)
{
    QList<departments> dep;
    departments depitem;
    QString curDepName = "";
    for(int i=0;i<modelDepart->rowCount(parent);i++){
        depitem.id = modelDepart->index(i,1,parent).data().toInt();
        curDepName = modelDepart->index(i,0,parent).data().toString();
        depitem.name = depName+curDepName;
        dep.append(depitem);
        if(modelDepart->rowCount(modelDepart->index(i,0,parent))>0){
            dep.append(depList(modelDepart->index(i,0,parent),depName+curDepName+"/"));
        }
    }
    return dep;
}

void InventoryReport::on_createButton_clicked()
{
    QMap<QString, QMap<QString,int> > dataForWorker;
    progressBar->setMaximum(0);
    enabledForm(false);
    if(isFirm->isChecked()){
        QMap<QString,int> data;
        data[firm->currentText()] = firm->itemData(firm->currentIndex()).toInt();
        dataForWorker["Firm"] = data;
    }
    if(isDep->isChecked()){
        QMap<QString,int> data;
        data[depName->text()] = departmentId;
        dataForWorker["Dep"] = data;
    }
    if(isWP->isChecked()){
        QMap<QString,int> data;
        data[wpName->text()] = workPlaceId;
        dataForWorker["WP"] = data;
    }
    if(hideComposition->isChecked()){
        dataForWorker["hideComposition"] = QMap<QString,int>();
    }
    if(givenSubDep->isChecked()){
        dataForWorker["givenSubDep"] = QMap<QString,int>();
    }

    db = QSqlDatabase::database("qt_sql_default_connection");
    db.setHostName(QSqlDatabase::database().hostName());
    db.setDatabaseName(QSqlDatabase::database().databaseName());
    db.setPort(QSqlDatabase::database().port());
    db.setUserName(QSqlDatabase::database().userName());
    db.setPassword(QSqlDatabase::database().password());

    InventoryReportWorker* worker = new InventoryReportWorker(dataForWorker,db);
    QThread* thread = new QThread;
    worker->moveToThread(thread);
    connect(worker, SIGNAL(reportCreated(CreateInventoryReport*)), this, SLOT(printReport(CreateInventoryReport*)));
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), this, SLOT(disableProgressBar()));
    connect(worker, SIGNAL(progresstext(QString)), progressText, SLOT(setText(QString)));
    connect(this, SIGNAL(closeInventoryReportWin()), worker, SIGNAL(finished()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
    return;
}
void InventoryReport::printReport(CreateInventoryReport *report)
{
    progressBar->setMaximum(100);
    enabledForm(true);
    cInvRep = report->clone();
    if(!groupBox_2->isChecked()){
        int nWidth = 1000;
        int nHeight = 600;
        QPrinter printer(QPrinter::HighResolution);
        QPrintPreviewDialog preview(&printer, this);
        connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
        preview.setWindowFlags(Qt::Dialog | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
        QPoint parentPos = this->mapToGlobal(this->pos());
        preview.setGeometry(parentPos.x() + this->width()/2 - nWidth/2,
                            parentPos.y() + this->height()/2 - nHeight/2,
                            nWidth, nHeight);
        preview.exec();
        cInvRep->clear();
    }else{
        if(fileName->text().isEmpty())
            on_selectFileForSave_clicked();
        if(fileName->text().isEmpty())
            return;
        QString fileN = fileName->text();
        if(!fileN.endsWith(".pdf", Qt::CaseInsensitive)){
            if (! (fileN.endsWith(".odt", Qt::CaseInsensitive) ||
                   fileN.endsWith(".htm", Qt::CaseInsensitive) ||
                   fileN.endsWith(".html", Qt::CaseInsensitive)) )
                     fileN += ".odt";
            cInvRep->write(fileN);
        }else{
            QPrinter *printer = new QPrinter(QPrinter::HighResolution);
            printer->setOrientation(QPrinter::Landscape);
            printer->setOutputFormat(QPrinter::PdfFormat);
            printer->setOutputFileName(fileN);
            cInvRep->print(printer);
            delete printer;
        }
        progressBar->setMaximum(100);
        QMessageBox::information(this, tr("Вывод в файл"),tr("Вывод в файл завершён."),tr("Закрыть"));
        cInvRep->clear();
    }
}
void InventoryReport::printPreview(QPrinter *printer)
{
    cInvRep->print(printer);
}
void InventoryReport::disableProgressBar()
{
    progressBar->setMaximum(100);
    progressText->setText("");
}
void InventoryReport::on_isFirm_clicked(bool checked)
{
    if(checked){
        workPlaceId = 0;
        wpName->setText("");
        departmentId = 0;
        depName->setText("");
        createButton->setEnabled(false);
    }
}
void InventoryReport::on_isDep_clicked(bool checked)
{
    if(checked){
        firm->setCurrentIndex(0);
        workPlaceId = 0;
        wpName->setText("");
        createButton->setEnabled(false);
    }
}
void InventoryReport::on_isWP_clicked(bool checked)
{
    if(checked){
        firm->setCurrentIndex(0);
        departmentId = 0;
        depName->setText("");
        createButton->setEnabled(false);
    }
}
void InventoryReport::on_selectFileForSave_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Укажите файл"),
                                              QString(), tr("ODF files (*.odt);;HTML-Files (*.htm *.html);;PDF-Files (*.pdf)"));
    if (fn.isEmpty()) return;
    if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) || fn.endsWith(".html", Qt::CaseInsensitive) ||
           fn.endsWith(".pdf", Qt::CaseInsensitive)) )
             fn += ".odt";
    fileName->setText(fn);
}

void InventoryReport::on_closeButton_clicked()
{
    emit closeInventoryReportWin();
}

void InventoryReport::enabledForm(bool enable)
{
    groupBox->setEnabled(enable);
    groupBox_2->setEnabled(enable);
    hideComposition->setEnabled(enable);
    createButton->setEnabled(enable);
}

void InventoryReport::changeEvent(QEvent *e)
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
