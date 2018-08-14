#ifndef OPENCUTEREPORT_H
#define OPENCUTEREPORT_H

#include "ui_opencutereport.h"

class ReportFileListModel;

class OpenCuteReport : public QWidget, private Ui::OpenCuteReport
{
    Q_OBJECT
public:
    explicit OpenCuteReport(QWidget *parent = 0);
    ~OpenCuteReport();
private:
    void loadReportFiles();
    ReportFileListModel *model;
signals:
    void closeCuteReportWin();
private slots:
    void updateReportDescription();
    void on_loadButton_clicked();
    void on_closeButton_clicked();
    void on_updateListReportsButton_clicked();
};

#endif // OPENCUTEREPORT_H
