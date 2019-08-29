#include "headers/lockdatabaseworker.h"

LockDataBaseWorker::LockDataBaseWorker(int recordId, const QString &recordNameId, const QString &tableName, const QMap<QString, QVariant> &credentials) :
    m_recordId(recordId),
    m_recordNameId(recordNameId),
    m_tableName(tableName),
    m_credentials(credentials)
{
    m_referenceBookName = "";
    m_recordsId = QList<int>();
    running = false;
}

LockDataBaseWorker::LockDataBaseWorker(QList<int> recordsId, const QString &recordNameId, const QString &tableName, const QMap<QString, QVariant> &credentials) :
    m_recordsId(recordsId),
    m_recordNameId(recordNameId),
    m_tableName(tableName),
    m_credentials(credentials)
{
    m_referenceBookName = "";
    m_recordId = 0;
    running = false;
}

LockDataBaseWorker::LockDataBaseWorker(const QString &referenceBookName, const QMap<QString, QVariant> &credentials) :
    m_referenceBookName(referenceBookName),
    m_credentials(credentials)
{
    m_recordId = 0;
    m_recordsId = QList<int>();
    running = false;
}

LockDataBaseWorker::~LockDataBaseWorker()
{
    timer->stop();
    delete timer;
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(m_credentials.value("nameConnect").toString() + "_lockdatabaseworker_connect");
}

void LockDataBaseWorker::process()
{
    running = true;
    db = QSqlDatabase::addDatabase(m_credentials.value("driver").toString(),m_credentials.value("nameConnect").toString() + "_lockdatabaseworker_connect");
    db.setHostName(m_credentials.value("host").toString());
    db.setPort(m_credentials.value("port").toInt());
    db.setUserName(m_credentials.value("login").toString());
    db.setPassword(m_credentials.value("pass").toString());
    db.setDatabaseName(m_credentials.value("databaseName").toString());
    if(!db.open()){
        emit error(m_recordId,tr("Не удалось подключиться к серверу mysql: %1").arg(db.lastError().text()));
        running = false;
        return;
    }
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(lockRec()));
    connect(this,SIGNAL(timerStopNow()),timer,SLOT(stop()));
    connect(this,SIGNAL(timerStopNow()),this,SIGNAL(finished()));
    timer->start(30000);
    lockRec();
}

void LockDataBaseWorker::lockRec()
{
    if(!running){
        timer->stop();
        emit finished();
        return;
    }

    QSqlQuery query(db);
    bool ok;

    if(m_recordId != 0){
        ok = query.exec(QString("UPDATE %1 SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%4' WHERE %2 = %3")
                        .arg(m_tableName)
                        .arg(m_recordNameId)
                        .arg(m_recordId)
                        .arg(m_credentials.value("login").toString()));
        if(!ok){
            emit error(m_recordId,query.lastError().text());
            return;
        }
    }

    if(!m_recordsId.isEmpty()){
        QString recordsForLock = "";
        recordsForLock = QString("%1").arg(m_recordsId.value(0));
        for(int i = 1;i<m_recordsId.count();i++)
            recordsForLock += QString(",%1").arg(m_recordsId.value(i));
        ok = query.exec(QString("UPDATE %1 SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%4' WHERE %2 IN (%3)")
                        .arg(m_tableName)
                        .arg(m_recordNameId)
                        .arg(recordsForLock)
                        .arg(m_credentials.value("login").toString()));
        if(!ok){
            emit error(m_recordId,query.lastError().text());
            return;
        }
    }

    if(!m_referenceBookName.isEmpty()){
        ok = query.exec(QString("SELECT * FROM lockreferencebook WHERE Name = '%1'").arg(m_referenceBookName));
        if(ok){
            if(query.size() > 0){
                ok = query.exec(QString("UPDATE lockreferencebook SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%2'"
                                        " WHERE Name = '%1'").arg(m_referenceBookName).arg(m_credentials.value("login").toString()));
            }else{
                ok = query.exec(QString("INSERT INTO lockreferencebook (Name, LockedUp, LockedBy) VALUES ('%1',"
                                        " (SELECT date_add(current_timestamp (), INTERVAL 1 MINUTE)), '%2');")
                                .arg(m_referenceBookName)
                                .arg(m_credentials.value("login").toString()));
            }
        }
        if(!ok){
            emit errorRef(m_referenceBookName,query.lastError().text());
            return;
        }
    }
}

void LockDataBaseWorker::stop()
{
    emit timerStopNow();
}

int LockDataBaseWorker::curRecordId()
{
    return m_recordId;
}

QList<int> LockDataBaseWorker::curListRecordsId()
{
    return m_recordsId;
}

QString LockDataBaseWorker::curReferenceBookName()
{
    return m_referenceBookName;
}

void LockDataBaseWorker::setRunning(bool run)
{
    running = run;
}
