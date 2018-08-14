#ifndef INVENTORYREPORT_H
#define INVENTORYREPORT_H

#include <QSqlDatabase>
#include <QtPrintSupport>
#include "ui_inventoryreport.h"

class CreateInventoryReport;
class DeviceModelControl;
class SqlTreeModel;
class WorkPlaceModel;

class InventoryReport : public QWidget, private Ui::InventoryReport {
    Q_OBJECT
public:
    InventoryReport(QWidget *parent = 0);
private:
    QSqlDatabase db;
    struct departments {
        QString name;
        int id;
    };
    int departmentId,workPlaceId;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    CreateInventoryReport *cInvRep;
    DeviceModelControl *orgTexModel;
    SqlTreeModel *modelDepart;
    WorkPlaceModel *wpModel;
    void populateModDep(int organizationId);
    void populateWPModel(const QString &filter = "");
    QList<InventoryReport::departments> depList(const QModelIndex &parent, QString depName);
    void enabledForm(bool enable);
protected:
    void changeEvent(QEvent *e);
signals:
    void closeInventoryReportWin();
private slots:
    void on_selectFileForSave_clicked();
    void on_firm_currentIndexChanged(int index);
    void on_isWP_clicked(bool checked);
    void on_isDep_clicked(bool checked);
    void on_isFirm_clicked(bool checked);
    void on_createButton_clicked();
    void on_closeButton_clicked();
    void on_selectWPButton_clicked();
    void on_selectDepButton_clicked();
    void setWorkPlase(int wpId, const QString &wpNm);
    void setDepartment(int depId, const QString &depNm);
    void printPreview(QPrinter *printer);
    void printReport(CreateInventoryReport* report);
    void disableProgressBar();
};

#endif // INVENTORYREPORT_H
