#ifndef DEVICEMODELWORKER_H
#define DEVICEMODELWORKER_H

#include <QMap>
#include <QtSql>
#include <QObject>
#include <QVariant>

class TreeItem;

class DeviceModelWorker : public QObject
{
    Q_OBJECT
public:
    explicit DeviceModelWorker(const QMap<QString,QVariant> &credentials = QMap<QString,QVariant>(),
                               const QMap<QString,QString> &forQuery = QMap<QString,QString>(),
                               const QVector<QVariant> &rootData = QVector<QVariant>());
    ~DeviceModelWorker();
    void setFilter(const QString &queryFilter);
private:
    QString primaryQuery, filter, tabName, aliasTable;
    int colId, colParent_Id;
    QMap<QString,QVariant> m_credentials;
    QVector<QVariant> m_rootData;
    TreeItem* search(TreeItem* , int, int);
    TreeItem* searchInRootNode(TreeItem* , int, int);

signals:
    void finished();
    void logData(QString text);
    void error(QString errorText);
    void result(TreeItem* licenses);

public slots:
    void process();
};

#endif // DEVICEMODELWORKER_H
