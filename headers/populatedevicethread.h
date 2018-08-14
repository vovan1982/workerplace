#ifndef POPULATEDEVICETHREAD_H
#define POPULATEDEVICETHREAD_H

#include <QSqlDatabase>
#include <QtWidgets>
#include <QMap>
#include <QThread>
#include <QSqlError>

class DeviceThreadWorker;
class TreeItem;

class PopulateDeviceThread : public QThread
{
    Q_OBJECT
public:
    PopulateDeviceThread(const QString &dbConnectionName = "default", const QSqlDatabase &connectionData = QSqlDatabase(), QObject *parent = 0);
    ~PopulateDeviceThread();
    void selectData(const QMap<QString,QString> &forQuery, const QVector<QVariant> &rootData);
private:
    QString m_dbConnectionName;
    QSqlDatabase m_connectionData;
    DeviceThreadWorker* m_deviceThreadWorker;
protected:
    void run();
signals:
    void result(TreeItem* deviceTree);
    void selectDatas(const QMap<QString,QString> &forQuery, const QVector<QVariant> &rootData);
    void ready(bool);
    void lastErrors(const QSqlError &error);
};

#endif // POPULATEDEVICETHREAD_H
