#include "headers/populatedevicethread.h"
#include "headers/devicethreadworker.h"
#include "headers/treeitem.h"
#include "headers/devicemodel.h"

PopulateDeviceThread::PopulateDeviceThread(const QString &dbConnectionName, const QSqlDatabase &connectionData, QObject *parent)
    : QThread(parent), m_dbConnectionName(dbConnectionName), m_connectionData(connectionData)
{
}

PopulateDeviceThread::~PopulateDeviceThread()
{
    m_deviceThreadWorker->~DeviceThreadWorker();
    //delete m_deviceThreadWorker;
}

void PopulateDeviceThread::selectData(const QMap<QString, QString> &forQuery, const QVector<QVariant> &rootData)
{
    emit selectDatas(forQuery,rootData);
}

void PopulateDeviceThread::run()
{
    emit ready(false);
    m_deviceThreadWorker = new DeviceThreadWorker(m_dbConnectionName,m_connectionData);
    qRegisterMetaType< QMap<QString,QString> >( "QMap<QString,QString>" );
    qRegisterMetaType< QVector<QVariant> >( "QVector<QVariant>" );
    qRegisterMetaType< QSqlError >( "QSqlError" );
    connect(this,SIGNAL(selectDatas(QMap<QString,QString>,QVector<QVariant>)),
            m_deviceThreadWorker,SLOT(createDeviceTree(QMap<QString,QString>,QVector<QVariant>)));
    connect(m_deviceThreadWorker,SIGNAL(result(TreeItem*)),this,SIGNAL(result(TreeItem*)));
    connect(m_deviceThreadWorker,SIGNAL(lastErrors(QSqlError)),this,SIGNAL(lastErrors(QSqlError)));
    emit ready(true);
    exec();
}
