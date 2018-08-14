#include <QDir>
#include <QFile>
#include <QStringListModel>
#include <CuteReport>
#include <QSqlDatabase>
#include "headers/opencutereport.h"
#include "headers/reportfilelistmodel.h"
#include "headers/reportfileitem.h"

OpenCuteReport::OpenCuteReport(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    splitter->setStretchFactor(1,1);

    loadReportFiles();
    reportsList->setModel(model);
    connect(reportsList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,SLOT(updateReportDescription()));
    if(model->rowCount()>0){
        reportsList->setCurrentIndex(model->index(0));
        loadButton->setEnabled(true);
    }
}

OpenCuteReport::~OpenCuteReport()
{
}

void OpenCuteReport::updateReportDescription()
{
    if(reportsList->currentIndex().isValid())
        discriptionReport->setText(model->getDescription(reportsList->currentIndex()).toString());
}

void OpenCuteReport::loadReportFiles()
{
    QVector<ReportFileItem*> files = QVector<ReportFileItem*>();
    QDir directory(QApplication::applicationDirPath()+"/reports");
    QStringList reportFiles = directory.entryList(QStringList() << "*.qtrp" << "*.QTRP",QDir::Files);
    for(int i=0; i< reportFiles.size();++i)
    {
        QString name = "";
        QString description = "";
        bool isFindName = false;
        bool isFindDescription = false;
        QFile inputFile(QApplication::applicationDirPath()+"/reports/"+reportFiles.at(i));
        if (inputFile.open(QIODevice::ReadOnly))
        {
           QTextStream in(&inputFile);
           while (!in.atEnd())
           {
              QString line = in.readLine();
              if(line.contains("name ")){
                  int len = line.size();
                  int end = line.lastIndexOf("</",-1,Qt::CaseInsensitive);
                  int start = line.indexOf(">",0,Qt::CaseInsensitive)+1;
                  name = line.mid(start,len-start-(len-end));
                  isFindName = true;
              }else if(line.contains("description ")){
                  int len = line.size();
                  int end = line.lastIndexOf("</",-1,Qt::CaseInsensitive);
                  int start = line.indexOf(">",0,Qt::CaseInsensitive)+1;
                  description = line.mid(start,len-start-(len-end));
                  isFindDescription = true;
              }if (isFindName && isFindDescription){
                  break;
              }
           }
           inputFile.close();
        }
        files.push_back(new ReportFileItem(name,description,QApplication::applicationDirPath()+"/reports/"+reportFiles.at(i)));
    }
    model = new ReportFileListModel(files,this);
}

void OpenCuteReport::on_loadButton_clicked()
{
    QSettings settings( QApplication::applicationDirPath()+"/cutereport.ini", QSettings::IniFormat );
    settings.beginGroup( "CuteReport" );
#ifdef QT_DEBUG
    settings.setValue( "PluginsPath", QApplication::applicationDirPath()+"/cutereport/Qt5/plugins/debug/core" );
#else
    settings.setValue( "PluginsPath", QApplication::applicationDirPath()+"/cutereport/Qt5/plugins/release/core" );
#endif
    settings.endGroup();

    CuteReport::ReportCore * reportCore = new CuteReport::ReportCore(this, new QSettings(QApplication::applicationDirPath()+"/cutereport.ini",QSettings::IniFormat), false,false);
    CuteReport::ReportPreview * preview = new CuteReport::ReportPreview();
    connect(preview,&CuteReport::ReportPreview::closed,preview,&CuteReport::ReportPreview::deleteLater);
    connect(preview,&CuteReport::ReportPreview::closed,reportCore,&CuteReport::ReportCore::deleteLater);
    preview->setReportCore(reportCore);
    QString reportUrl;
    reportUrl = model->getPath(reportsList->currentIndex()).toString();
    CuteReport::ReportInterface * reportObject = reportCore->loadReport(reportUrl);
    if (!reportObject) {
        QMessageBox::critical(0, QObject::tr("CuteReport"),
                              QObject::tr("Report file loading error"),
                              QMessageBox::Ok);
    }
    for(int i=0;i<reportObject->datasets().size();i++)
    {
        QString moduleName = reportObject->datasets()[i]->moduleShortName();
        if(moduleName.contains("SQL")){
            reportObject->datasets()[i]->setProperty("dbhost",QVariant(QSqlDatabase::database().hostName()));
            reportObject->datasets()[i]->setProperty("dbname",QVariant(QSqlDatabase::database().databaseName()));
            reportObject->datasets()[i]->setProperty("dbuser",QVariant(QSqlDatabase::database().userName()));
            reportObject->datasets()[i]->setProperty("dbpasswd",QVariant(QSqlDatabase::database().password()));
            reportObject->datasets()[i]->setProperty("driver",QVariant(QSqlDatabase::database().driverName()));
        }
    }
    preview->connectReport(reportObject);
    preview->show();
    preview->run();
}

void OpenCuteReport::on_closeButton_clicked()
{
    emit closeCuteReportWin();
}

void OpenCuteReport::on_updateListReportsButton_clicked()
{
    loadReportFiles();
    reportsList->setModel(model);
    connect(reportsList->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this,SLOT(updateReportDescription()));
    if(model->rowCount()>0){
        reportsList->setCurrentIndex(model->index(0));
        loadButton->setEnabled(true);
    }
}
