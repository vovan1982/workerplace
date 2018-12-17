# -------------------------------------------------
# Project created by QtCreator 2009-04-15T18:42:10
# -------------------------------------------------
QT += sql xml widgets printsupport core
TARGET = workerplace
TEMPLATE = app
RC_FILE = workerplace.rc
VERSION = 0.9.0.0
SOURCES += main.cpp \
    source/connectdb.cpp \
    source/referencebook.cpp \
    source/users.cpp \
    source/cedittableview.cpp \
    source/workerplace.cpp \
    source/edittable.cpp \
    source/delegats.cpp \
    source/workplace.cpp \
    source/sqltreemodel.cpp \
    source/movetreeitem.cpp \
    source/treeitem.cpp \
    source/addeditworkerplace.cpp \
    source/addeditfirm.cpp \
    source/addeditfilpred.cpp \
    source/addworkplaceofusers.cpp \
    source/addworkerplace.cpp \
    source/addexistinguser.cpp \
    source/workplacemodel.cpp \
    source/selectworkplace.cpp \
    source/addeditdevice.cpp \
    source/addedittypedevice.cpp \
    source/filelistmodel.cpp \
    source/providers.cpp \
    source/editprovider.cpp \
    source/addeditprovider.cpp \
    source/movedevice.cpp \
    source/selectdevice.cpp \
    source/selectdepartment.cpp \
    source/createinventoryreport.cpp \
    source/inventoryreport.cpp \
    source/producers.cpp \
    source/addeditproducer.cpp \
    source/po.cpp \
    source/pomodel.cpp \
    source/editandselectproducer.cpp \
    source/selecticons.cpp \
    source/selectpo.cpp \
    source/licenses.cpp \
    source/licensemodel.cpp \
    source/addeditlicense.cpp \
    source/editpo.cpp \
    source/movelicense.cpp \
    source/selectlicense.cpp \
    source/filterlicense.cpp \
    source/addeditnetworkinterface.cpp \
    source/devicemodel.cpp \
    source/devicemodelcontrol.cpp \
    source/device.cpp \
    source/filterdevice.cpp \
    source/licensemodelcontrol.cpp \
    source/journalhistorymoved.cpp \
    source/filterjournalhistorymoved.cpp \
    source/deviceimport.cpp \
    source/deviceimportcomparisonworkplace.cpp \
    source/devicereportparser.cpp \
    source/deviceimportcomparisontypedevice.cpp \
    source/deviceimportcomparisontypelicense.cpp \
    source/deviceimportcomparisonproducer.cpp \
    source/userimport.cpp \
    source/userimportcomparisonorganization.cpp \
    source/userreportparser.cpp \
    source/populatedevicethread.cpp \
    source/devicethreadworker.cpp \
    source/lockdatabase.cpp \
    source/addeditdepartments.cpp \
    source/addeditusers.cpp \
    source/addeditloginpass.cpp \
    source/addeditnumber.cpp \
    source/journalhistoryusersonwp.cpp \
    source/deluserwithwp.cpp \
    source/inventoryreportworker.cpp \
    source/opencutereport.cpp \
    source/reportfileitem.cpp \
    source/reportfilelistmodel.cpp \
    source/licensetablemodel.cpp \
    source/about.cpp \
    source/loadindicator.cpp
HEADERS += headers/connectdb.h \
    headers/referencebook.h \
    headers/edittable.h \
    headers/cedittableview.h \
    headers/workerplace.h \
    headers/delegats.h \
    headers/workplace.h \
    headers/sqltreemodel.h \
    headers/movetreeitem.h \
    headers/treeitem.h \
    headers/addeditworkerplace.h \
    headers/addeditfirm.h \
    headers/addeditfilpred.h \
    headers/addworkplaceofusers.h \
    headers/addworkerplace.h \
    headers/addexistinguser.h \
    headers/workplacemodel.h \
    headers/selectworkplace.h \
    headers/addeditdevice.h \
    headers/addedittypedevice.h \
    headers/filelistmodel.h \
    headers/providers.h \
    headers/editprovider.h \
    headers/addeditprovider.h \
    headers/movedevice.h \
    headers/selectdevice.h \
    headers/selectdepartment.h \
    headers/inventoryreport.h \
    headers/createinventoryreport.h \
    headers/producers.h \
    headers/addeditproducer.h \
    headers/po.h \
    headers/pomodel.h \
    headers/editandselectproducer.h \
    headers/selecticons.h \
    headers/selectpo.h \
    headers/licenses.h \
    headers/licensemodel.h \
    headers/addeditlicense.h \
    headers/editpo.h \
    headers/movelicense.h \
    headers/selectlicense.h \
    headers/filterlicense.h \
    headers/addeditnetworkinterface.h \
    headers/devicemodel.h \
    headers/devicemodelcontrol.h \
    headers/device.h \
    headers/filterdevice.h \
    headers/licensemodelcontrol.h \
    headers/journalhistorymoved.h \
    headers/filterjournalhistorymoved.h \
    headers/deviceimport.h \
    headers/deviceimportcomparisonworkplace.h \
    headers/devicereportparser.h \
    headers/deviceimportcomparisontypedevice.h \
    headers/deviceimportcomparisontypelicense.h \
    headers/deviceimportcomparisonproducer.h \
    headers/userimport.h \
    headers/userimportcomparisonorganization.h \
    headers/userreportparser.h \
    headers/populatedevicethread.h \
    headers/devicethreadworker.h \
    headers/lockdatabase.h \
    headers/addeditdepartments.h \
    headers/addeditusers.h \
    headers/addeditloginpass.h \
    headers/addeditnumber.h \
    headers/users.h \
    headers/journalhistoryusersonwp.h \
    headers/deluserwithwp.h \
    headers/inventoryreportworker.h \
    headers/opencutereport.h \
    headers/reportfileitem.h \
    headers/reportfilelistmodel.h \
    headers/enums.h \
    headers/licensetablemodel.h \
    headers/about.h \
    headers/loadindicator.h
FORMS += ui/workerplace.ui \
    ui/connectdb.ui \
    ui/referencebook.ui \
    ui/users.ui \
    ui/edittable.ui \
    ui/workplace.ui \
    ui/movetreeitem.ui \
    ui/addeditworkerplace.ui \
    ui/addeditfirm.ui \
    ui/addeditfilpred.ui \
    ui/addworkplaceofusers.ui \
    ui/addworkerplace.ui \
    ui/addexistinguser.ui \
    ui/selectworkplace.ui \
    ui/addeditdevice.ui \
    ui/addedittypedevice.ui \
    ui/providers.ui \
    ui/editprovider.ui \
    ui/addeditprovider.ui \
    ui/movedevice.ui \
    ui/selectdevice.ui \
    ui/selectdepartment.ui \
    ui/inventoryreport.ui \
    ui/producers.ui \
    ui/addeditproducer.ui \
    ui/po.ui \
    ui/editandselectproducer.ui \
    ui/selecticons.ui \
    ui/selectpo.ui \
    ui/licenses.ui \
    ui/addeditlicense.ui \
    ui/editpo.ui \
    ui/movelicense.ui \
    ui/selectlicense.ui \
    ui/filterlicense.ui \
    ui/addeditnetworkinterface.ui \
    ui/device.ui \
    ui/filterdevice.ui \
    ui/journalhistorymoved.ui \
    ui/filterjournalhistorymoved.ui \
    ui/deviceimport.ui \
    ui/deviceimportcomparisonworkplace.ui \
    ui/deviceimportcomparisontypedevice.ui \
    ui/deviceimportcomparisontypelicense.ui \
    ui/deviceimportcomparisonproducer.ui \
    ui/userimport.ui \
    ui/userimportcomparisonorganization.ui \
    ui/addeditdepartments.ui \
    ui/addeditusers.ui \
    ui/addeditloginpass.ui \
    ui/addeditnumber.ui \
    ui/journalhistoryusersonwp.ui \
    ui/deluserwithwp.ui \
    ui/opencutereport.ui \
    ui/about.ui
RESOURCES += icons.qrc

include(plugins/lineedittwobutton/lineedittwobutton.pri)
include(cutereport/x64/include/CuteReport.pri)
