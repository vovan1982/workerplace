#ifndef DEVICETHREADWORKER_H
#define DEVICETHREADWORKER_H

#include <QtSql>
#include <QMap>
#include <QObject>

class TreeItem;

class DeviceThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeviceThreadWorker(const QString &dbConnectionName = "default", const QSqlDatabase &connectionData = QSqlDatabase(), QObject *parent = 0);
    ~DeviceThreadWorker();
private:
    QSqlDatabase db;
    QSqlError lastErr;
    QString m_dbConnectionName;
    TreeItem* search(TreeItem* , int, int);
    TreeItem* searchInRootNode(TreeItem* , int, int);
signals:
    void lastErrors(const QSqlError &error);
    void result(TreeItem* deviceTree);
public slots:
    void createDeviceTree(const QMap<QString,QString> &forQuery, const QVector<QVariant> &rootData);
};

#endif // DEVICETHREADWORKER_H
