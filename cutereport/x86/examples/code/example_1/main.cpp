#include <QApplication>
#include <QMessageBox>

#include <CuteReport>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings( QApplication::applicationDirPath()+"/cutereport.ini", QSettings::IniFormat );
    settings.beginGroup( "CuteReport" );
    settings.setValue( "PluginsPath", "D:/Qt/projects/cutereport_1.3.2_dev/Qt5/plugins/debug/core" );
    settings.endGroup();

    CuteReport::ReportCore * reportCore = new CuteReport::ReportCore(0, new QSettings(QApplication::applicationDirPath()+"/cutereport.ini",QSettings::IniFormat), false);
    CuteReport::ReportPreview * preview = new CuteReport::ReportPreview();
    preview->setReportCore(reportCore);
    QString reportUrl;
    reportUrl = "file:/D/Qt/testreport.qtrp";
    CuteReport::ReportInterface * reportObject = reportCore->loadReport(reportUrl);
    if (!reportObject) {
        QMessageBox::critical(0, QObject::tr("CuteReport example"),
                              QObject::tr("Report file loading error"),
                              QMessageBox::Ok);
        exit(1);
    }
    for(int i=0;i<0;i++)
    {
        QString moduleName = reportObject->datasets()[i]->moduleShortName();
        if(moduleName.contains("SQL")){
            reportObject->datasets()[i]->setProperty("dbpasswd",QVariant("B27io64k"));
        }
    }
    preview->connectReport(reportObject);
    preview->show();
    preview->run();

    a.exec();

    delete preview;
    delete reportCore;
    return 0;
}
