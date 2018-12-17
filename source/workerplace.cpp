#include <QtWidgets>
#include <QMdiSubWindow>
#include "headers/about.h"
#include "headers/po.h"
#include "headers/workerplace.h"
#include "headers/referencebook.h"
#include "headers/users.h"
#include "headers/device.h"
#include "headers/workplace.h"
#include "headers/providers.h"
#include "headers/producers.h"
#include "headers/inventoryreport.h"
#include "headers/addedittypedevice.h"
#include "headers/licenses.h"
#include "headers/journalhistorymoved.h"
#include "headers/deviceimport.h"
#include "headers/userimport.h"
#include "headers/opencutereport.h"
#include "headers/journalhistoryusersonwp.h"

workerPlace::workerPlace(QWidget *parent) :
    QMainWindow(parent){
    setupUi(this);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);
}
void workerPlace::on_actionReferenceBookEdit_triggered()
{
    QMdiSubWindow *sw = findSubWindow("ReferenceBooks");
    if(sw == 0)
        createReferenceBook()->show();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionUsers_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Cusers");
    if(sw == 0)
        createUsers()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionDevice_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Device");
    if(sw == 0)
        createDevice()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionWorkPlace_triggered()
{
    QMdiSubWindow *sw = findSubWindow("WorkPlace");
    if(sw == 0)
        createWorkPlace()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionPoviders_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Providers");
    if(sw == 0)
        createProviders()->show();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionProducers_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Producers");
    if(sw == 0)
        createProducers()->show();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionInventoryReport_triggered()
{
    QMdiSubWindow *sw = findSubWindow("InventoryReport");
    if(sw == 0)
        createInventoryReport()->show();
    else
        mdiArea->setActiveSubWindow(sw);

}
void workerPlace::on_actionTypeDevice_triggered()
{
    QMdiSubWindow *sw = findSubWindow("AddEditTypeDevice");
    if(sw == 0)
        createAddEditTypeDevice()->show();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionPo_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Po");
    if(sw == 0)
        createPo()->show();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionLicenses_triggered()
{
    QMdiSubWindow *sw = findSubWindow("Licenses");
    if(sw == 0)
        createLicenses()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionJournalHistoryMoved_triggered()
{
    QMdiSubWindow *sw = findSubWindow("JournalHistoryMoved");
    if(sw == 0)
        createJournalHistoryMoved()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionJournalHistoryUsersOnWP_triggered()
{
    QMdiSubWindow *sw = findSubWindow("JournalHistoryUsersOnWP");
    if(sw == 0)
        createJournalHistoryUsersOnWP()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionDeviceImport_triggered()
{
    QMdiSubWindow *sw = findSubWindow("DeviceImport");
    if(sw == 0)
        createDeviceImport()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionUserImport_triggered()
{
    QMdiSubWindow *sw = findSubWindow("UserImport");
    if(sw == 0)
        createUserImport()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
void workerPlace::on_actionOpenCuteReport_triggered()
{
    QMdiSubWindow *sw = findSubWindow("OpenCuteReport");
    if(sw == 0)
        createOpenCuteReport()->showMaximized();
    else
        mdiArea->setActiveSubWindow(sw);
}
CReferenceBook* workerPlace::createReferenceBook()
{
    CReferenceBook* prb = new CReferenceBook;
    mdiArea->addSubWindow(prb);
    prb->setAttribute(Qt::WA_DeleteOnClose);
    return prb;
}
Cusers* workerPlace::createUsers()
{
    Cusers* pu =new Cusers;
    mdiArea->addSubWindow(pu);
    pu->setAttribute(Qt::WA_DeleteOnClose);
    connect(pu,SIGNAL(closeUserWin(bool)),mdiArea,SLOT(closeActiveSubWindow()));
    return pu;
}
Device* workerPlace::createDevice()
{
   Device* dev = new Device;
   mdiArea->addSubWindow(dev);
   dev->setAttribute(Qt::WA_DeleteOnClose);
   connect(dev,SIGNAL(closeDeviceWin()),mdiArea,SLOT(closeActiveSubWindow()));
   return dev;
}
WorkPlace* workerPlace::createWorkPlace()
{
    WorkPlace* wp = new WorkPlace;
    mdiArea->addSubWindow(wp);
    wp->setAttribute(Qt::WA_DeleteOnClose);
    connect(wp,SIGNAL(closeWorkPlaceWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return wp;
}
Providers* workerPlace::createProviders()
{
    Providers* pr = new Providers;
    mdiArea->addSubWindow(pr);
    pr->setAttribute(Qt::WA_DeleteOnClose);
    connect(pr,SIGNAL(closeProviderWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return pr;
}
Producers* workerPlace::createProducers()
{
    Producers* pr = new Producers;
    mdiArea->addSubWindow(pr);
    pr->setAttribute(Qt::WA_DeleteOnClose);
    connect(pr,SIGNAL(closeProducerWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return pr;
}
InventoryReport* workerPlace::createInventoryReport()
{
    InventoryReport *ir = new InventoryReport;
    mdiArea->addSubWindow(ir);
    ir->setAttribute(Qt::WA_DeleteOnClose);
    connect(ir,SIGNAL(closeInventoryReportWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return ir;
}
AddEditTypeDevice* workerPlace::createAddEditTypeDevice()
{
    AddEditTypeDevice *aetd = new AddEditTypeDevice(0,"typedevice",1,true);
    mdiArea->addSubWindow(aetd);
    aetd->setAttribute(Qt::WA_DeleteOnClose);
    connect(aetd,SIGNAL(closeTypeDeviceWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return aetd;
}
Po* workerPlace::createPo()
{
    Po *soft = new Po;
    mdiArea->addSubWindow(soft);
    soft->setAttribute(Qt::WA_DeleteOnClose);
    connect(soft,SIGNAL(closePoWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return soft;
}
Licenses* workerPlace::createLicenses()
{
    Licenses *lic = new Licenses;
    mdiArea->addSubWindow(lic);
    lic->setAttribute(Qt::WA_DeleteOnClose);
    connect(lic,SIGNAL(closeLicWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return lic;
}
JournalHistoryMoved* workerPlace::createJournalHistoryMoved()
{
    JournalHistoryMoved *jhm = new JournalHistoryMoved;
    mdiArea->addSubWindow(jhm);
    jhm->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhm,SIGNAL(closeJhmWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return jhm;
}
JournalHistoryUsersOnWP* workerPlace::createJournalHistoryUsersOnWP()
{
    JournalHistoryUsersOnWP *jhu = new JournalHistoryUsersOnWP;
    mdiArea->addSubWindow(jhu);
    jhu->setAttribute(Qt::WA_DeleteOnClose);
    connect(jhu,SIGNAL(closeJhuWin()),mdiArea,SLOT(closeActiveSubWindow()));
    return jhu;
}
DeviceImport *workerPlace::createDeviceImport()
{
    DeviceImport *id = new DeviceImport;
    mdiArea->addSubWindow(id);
    id->setAttribute(Qt::WA_DeleteOnClose);
    return id;
}
UserImport *workerPlace::createUserImport()
{
    UserImport *ui = new UserImport;
    mdiArea->addSubWindow(ui);
    ui->setAttribute(Qt::WA_DeleteOnClose);
    return ui;
}
OpenCuteReport *workerPlace::createOpenCuteReport()
{
    OpenCuteReport *ocr = new OpenCuteReport;
    mdiArea->addSubWindow(ocr);
    ocr->setAttribute(Qt::WA_DeleteOnClose);
    connect(ocr,&OpenCuteReport::closeCuteReportWin,mdiArea,&QMdiArea::closeActiveSubWindow);
    return ocr;
}
void workerPlace::on_exit_triggered()
{
    workerPlace::close();
}
void workerPlace::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
QMdiSubWindow* workerPlace::findSubWindow(const QString &name)
{
    QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for(int i = 0; i < subWindows.count(); i++)
        if(subWindows.at(i)->widget()->objectName() == name)
            return subWindows.at(i);
    return 0;
}

void workerPlace::closeEvent(QCloseEvent *event){
    mdiArea->closeAllSubWindows();
    QSqlDatabase db = QSqlDatabase::database();
    if(db.isOpen())
        db.close();
    event->accept();
}

void workerPlace::on_actionAbout_triggered()
{
    About * ab = new About(this);
    ab->setAttribute(Qt::WA_DeleteOnClose);
    ab->setFixedSize(ab->size());
    ab->exec();
}
