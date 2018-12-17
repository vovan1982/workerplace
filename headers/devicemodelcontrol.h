#ifndef DEVICEMODELCONTROL_H
#define DEVICEMODELCONTROL_H

#include <QObject>
#include <QTreeView>
#include <QSqlDatabase>
#include <QSortFilterProxyModel>

class DeviceModel;
class PopulateDeviceThread;

class DeviceModelControl : public QObject
{
    Q_OBJECT
public:
    explicit DeviceModelControl(QObject *parent = 0, const QString &dbConnectionName = "default",
                                const QSqlDatabase &connectionData = QSqlDatabase(),const QString &modelFilter = "",
                                bool populateModelnInit = true, bool inThread = true);
    explicit DeviceModelControl(QObject *parent = 0, QTreeView *view = 0, const QString &dbConnectionName = "default",
                                const QSqlDatabase &connectionData = QSqlDatabase(),const QString &modelFilter = "",
                                bool populateModelnInit = true, bool inThread = true);
    explicit DeviceModelControl(QObject *parent = 0, QTreeView *view = 0, const QSqlDatabase &connectionData = QSqlDatabase(),
                                const QString &dbConnectionName = "default");

    ~DeviceModelControl();
    void populateDevModel(const QString &filter);
    DeviceModel *model();
    QString filter();
    void clearDevFilter();
    void setDevFilter(const QString &filter);
    void setCurrentIndexFirstRow();
    void setHeaderData();
    QModelIndex realModelIndex(const QModelIndex &viewIndex) const;
    QModelIndex realViewIndex(const QModelIndex &modelIndex) const;
    bool dataIsLoad;
private:
    DeviceModel *devModel;
    QSortFilterProxyModel *proxyModel;
    QTreeView *m_view;
    QString devFilter;
signals:
    void dataIsPopulated();
    void devModelUpdated();
private slots:
    void dataIsLoaded();
public slots:
    void updateDevModel();
    
};

#endif // DEVICEMODELCONTROL_H
