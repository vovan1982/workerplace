INCLUDEPATH += "cutereport/x86/include/cutereport"
LIBS += -L"D:/Qt/projects/workerplace/cutereport/x86"
win32: CONFIG(debug, debug|release): LIBS += -lCuteReportCored -lCuteReportWidgetsd 
else: LIBS += -lCuteReportCore -lCuteReportWidgets 
