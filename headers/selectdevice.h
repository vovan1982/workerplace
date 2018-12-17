#ifndef SELECTDEVICE_H
#define SELECTDEVICE_H

#include <QSqlDatabase>
#include "ui_selectdevice.h"
class QLabel;

class DeviceModelControl;

class SelectDevice : public QDialog, private Ui::SelectDevice {
    Q_OBJECT
public:
    SelectDevice(QWidget *parent = 0, const QString &filter = "", bool multiselections = false,
                 bool onlyHardwareMode = false, bool onlyOrgTexMode = false, bool locationIsSet = false);
    void setViewRootIsDecorated(bool show);
private:
    QSqlDatabase db;
    QString m_filter, curFilter, orgTexFilter;
    bool m_multiselections, m_onlyHardwareMode;
    bool m_onlyOrgTexMode, m_locationIsSet;
    bool filterIsSet;
    QList<QVariant> selectedDev;
    DeviceModelControl *devModel;
    bool loadIndicatorIsShowed, lockSelected;
protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);
signals:
    void selectedDevice(const QList<QVariant> &dev);
    void loadIndicatorShowed();
    void loadIndicatorClosed();
private slots:
    void dataIsLoaded();
    void on_buttonClearFilter_clicked();
    void on_buttonSetFilter_clicked();
    void on_buttonSelect_clicked();
    void setCurIndexIfCollapsed(const QModelIndex &index);
    void setOrgTexFilter(const QString &filter);
    void showLoadIndicator();
    void lockedErrorMessage(const QString &msg, bool updateModel);
};

#endif // SELECTDEVICE_H
