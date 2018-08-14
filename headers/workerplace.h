#ifndef WORKERPLACE_H
#define WORKERPLACE_H

#include <QtSql>
#include "ui_workerplace.h"

class CReferenceBook;
class Cusers;
class Device;
class WorkPlace;
class Providers;
class Producers;
class InventoryReport;
class AddEditTypeDevice;
class Po;
class Licenses;
class JournalHistoryMoved;
class JournalHistoryUsersOnWP;
class DeviceImport;
class UserImport;
class OpenCuteReport;

class workerPlace : public QMainWindow, private Ui::workerPlace {
    Q_OBJECT
    Q_DISABLE_COPY(workerPlace)
public:
    explicit workerPlace(QWidget *parent = 0);
private:
    Device* createDevice();
    CReferenceBook* createReferenceBook();
    Cusers* createUsers();
    WorkPlace* createWorkPlace();
    Providers* createProviders();
    Producers* createProducers();
    InventoryReport* createInventoryReport();
    AddEditTypeDevice* createAddEditTypeDevice();
    Po* createPo();
    Licenses* createLicenses();
    JournalHistoryMoved* createJournalHistoryMoved();
    JournalHistoryUsersOnWP* createJournalHistoryUsersOnWP();
    DeviceImport* createDeviceImport();
    UserImport* createUserImport();
    OpenCuteReport* createOpenCuteReport();
    QMdiSubWindow* findSubWindow(const QString &name);
protected:
    virtual void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_actionLicenses_triggered();
    void on_actionPo_triggered();
    void on_actionTypeDevice_triggered();
    void on_actionInventoryReport_triggered();
    void on_actionPoviders_triggered();
    void on_actionProducers_triggered();
    void on_actionWorkPlace_triggered();
    void on_actionReferenceBookEdit_triggered();
    void on_actionUsers_triggered();
    void on_exit_triggered();
    void on_actionDevice_triggered();
    void on_actionJournalHistoryMoved_triggered();
    void on_actionDeviceImport_triggered();
    void on_actionUserImport_triggered();
    void on_actionOpenCuteReport_triggered();
    void on_actionJournalHistoryUsersOnWP_triggered();
};

#endif // WORKERPLACE_H
