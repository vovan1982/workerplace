#ifndef DEVICE_H
#define DEVICE_H

#include <QSqlDatabase>
#include "ui_device.h"

class WorkPlaceModel;
class DeviceModelControl;
class LoadIndicator;

class Device : public QWidget, private Ui::Device
{
    Q_OBJECT
    
public:
    explicit Device(QWidget *parent = 0, bool wpMode = false, int wpId = 0, int wpFirmId = 0, bool readOnly = false);
private:
    QSqlDatabase db;
    bool m_wpMode, filterIsSet;
    int m_wpId, m_wpFirmId;
    bool m_readOnly;
    QString deviceFilter, wpDeviceFilter, wpName;
    DeviceModelControl *devModel;
    WorkPlaceModel *wpModel;
    QMenu *menu;
    LoadIndicator *li;
    void populateWpModel(int filter);
    void clearWpForm();
    QString readDataModel(const QModelIndex &index);
    bool loadIndicatorIsShowed;
protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *event);
signals:
    void closeDeviceWin();
    void devModelUpdated();
private slots:
    void showLoadIndicator();
    void dataIsLoaded();
    void onDevMenu(const QPoint &p);
    void doubleClickedDeviceView(const QModelIndex &index);
    void on_actionAddNewDevice_triggered();
    void on_actionAddDevice_triggered();
    void on_actionCreateCopy_triggered();
    void on_actionDelDevice_triggered();
    void on_actionMoveDevice_triggered();
    void on_actionAddNewDiviceInComposition_triggered();
    void on_actionEditDevice_triggered();
    void on_buttonMove_clicked();
    void on_buttonDeviceFilter_clicked();
    void on_buttonClearDeviceFilter_clicked();
    void on_buttonClose_clicked();
    void updateDevModel();
    void updateDevRow();
    void setDeviceFilter(const QString &filter);
    void convertIndexForUpdateWp(const QModelIndex &index);
    void updateWpModel(const QModelIndex &index);
    void setCurIndexIfCollapsed(const QModelIndex &index);
    void setAccessToActions(const QModelIndex &index = QModelIndex());
    void insertNewRowOfDB(int id);
    void insertNewRowOfDB(int id, int parent_id);
    void afterMove();
    void afterMove(int type, int orgTexId);
    void on_actionHistoryMoved_triggered();
    void on_actionCopyDeviceInBufer_triggered();
    void on_buttonCopyAllInBufer_clicked();
};

#endif // DEVICE_H
