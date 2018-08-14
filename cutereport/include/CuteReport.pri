INCLUDEPATH += cutereport/include/cutereport
LIBS += -L"D:/Qt/projects/workerplace/cutereport"
win32: CONFIG(debug, debug|release): LIBS += -lCuteReportCored -lCuteReportWidgetsd 
else: LIBS += -lCuteReportCore -lCuteReportWidgets 
