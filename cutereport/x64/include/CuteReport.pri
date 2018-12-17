INCLUDEPATH += cutereport/x64/include/cutereport
LIBS += -L"D:/Qt/projects/workerplace/cutereport/x64"
win32: CONFIG(debug, debug|release): LIBS += -lCuteReportCored -lCuteReportWidgetsd 
else: LIBS += -lCuteReportCore -lCuteReportWidgets 
