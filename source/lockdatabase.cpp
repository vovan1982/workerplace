#include <QMessageBox>
#include "headers/lockdatabase.h"
#include "headers/lockdatabaseworker.h"

LockDataBase::LockDataBase(QObject *parent) :
    QObject(parent),
    m_parent(parent)
{
    recordUserBlocking = "";
    referenceBookUserBlocking = "";
    credentials["host"] = QSqlDatabase::database().hostName();
    credentials["port"] = QSqlDatabase::database().port();
    credentials["driver"] = QSqlDatabase::database().driverName();
    credentials["login"] = QSqlDatabase::database().userName();
    credentials["pass"] = QSqlDatabase::database().password();
    credentials["databaseName"] = QSqlDatabase::database().databaseName();
}
LockDataBase::~LockDataBase()
{
    emit stopAllThread();
    while(!lockedRecInThreads.isEmpty()){
        delete lockedRecInThreads.takeFirst();
    }
    while(!lockedListRecInThreads.isEmpty()){
        delete lockedListRecInThreads.takeFirst();
    }
    while(!lockedReferenceBookInThreads.isEmpty()){
        delete lockedReferenceBookInThreads.takeFirst();
    }
}
void LockDataBase::lockRecordThread(int recordId, const QString &recordNameId, const QString &tableName)
{
    if(recordId <= 0) return;

    credentials["nameConnect"] = QString("thread%1_%2").arg(lockedRecInThreads.count()).arg(recordId);

    LockDataBaseWorker *worker = new LockDataBaseWorker(recordId,recordNameId,tableName,credentials);
    connect(worker,&LockDataBaseWorker::error,this,&LockDataBase::setError);
    connect(this,&LockDataBase::stopAllThread,worker,&LockDataBaseWorker::finished);
    lockedRecInThreads.append(worker);
    QThread* thread = new QThread;
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
void LockDataBase::lockListRecordThread(QList<int> recordsId, const QString &recordNameId, const QString &tableName)
{
    if(recordsId.isEmpty()) return;

    credentials["nameConnect"] = QString("list_record_thread%1_%2").arg(lockedListRecInThreads.count()).arg(recordsId.value(0));

    LockDataBaseWorker *worker = new LockDataBaseWorker(recordsId,recordNameId,tableName,credentials);
    connect(worker,&LockDataBaseWorker::error,this,&LockDataBase::setError);
    connect(this,&LockDataBase::stopAllThread,worker,&LockDataBaseWorker::finished);
    lockedListRecInThreads.append(worker);
    QThread* thread = new QThread;
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
void LockDataBase::lockReferenceBookThread(const QString &referenceBookName)
{
    if(referenceBookName.isEmpty()) return;

    credentials["nameConnect"] = QString("list_record_thread%1_%2").arg(lockedReferenceBookInThreads.count()).arg(referenceBookName);

    LockDataBaseWorker *worker = new LockDataBaseWorker(referenceBookName,credentials);
    connect(worker,&LockDataBaseWorker::errorRef,this,&LockDataBase::setErrorRef);
    connect(this,&LockDataBase::stopAllThread,worker,&LockDataBaseWorker::finished);
    lockedReferenceBookInThreads.append(worker);
    QThread* thread = new QThread;
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
void LockDataBase::stopLockRecordThread(int recordId)
{
    for(int i = 0; i<lockedRecInThreads.size();i++){
        if(lockedRecInThreads.value(i) != nullptr && lockedRecInThreads.value(i)->curRecordId() == recordId){
            lockedRecInThreads.takeAt(i)->stop();
            break;
        }
    }
}
void LockDataBase::stopLockListRecordThread()
{
    while(!lockedListRecInThreads.isEmpty()){
        LockDataBaseWorker *worker = lockedListRecInThreads.takeFirst();
        if(worker != nullptr){
            worker->stop();
        }
    }
}
void LockDataBase::stopLockReferenceBookThread(const QString &referenceBookName)
{
    for(int i = 0; i<lockedReferenceBookInThreads.size();i++){
        if(lockedReferenceBookInThreads.value(i) != nullptr && lockedReferenceBookInThreads.value(i)->curReferenceBookName() == referenceBookName){
            lockedReferenceBookInThreads.takeAt(i)->stop();
            break;
        }
    }
}
bool LockDataBase::lockRecord(int recordId, const QString &recordNameId, const QString &tableName)
{
    QSqlQuery query;
    bool ok;
    QSqlDatabase curDb = QSqlDatabase::database();
    ok = query.exec(QString("UPDATE %1 SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%4' WHERE %2 = %3")
                    .arg(tableName)
                    .arg(recordNameId)
                    .arg(recordId)
                    .arg(curDb.userName()));
    if(!ok)
        error = query.lastError();
    return ok;
}

bool LockDataBase::lockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName)
{
    if(!recordsId.isEmpty()){
        QString recordsForLock = "";
        QSqlQuery query;
        bool ok;
        QSqlDatabase curDb = QSqlDatabase::database();
        recordsForLock = QString("%1").arg(recordsId.value(0));
        for(int i = 1;i<recordsId.count();i++)
            recordsForLock += QString(",%1").arg(recordsId.value(i));
        ok = query.exec(QString("UPDATE %1 SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%4' WHERE %2 IN (%3)")
                        .arg(tableName)
                        .arg(recordNameId)
                        .arg(recordsForLock)
                        .arg(curDb.userName()));
        if(!ok)
            error = query.lastError();
        return ok;
    }else
        return false;
}

bool LockDataBase::unlockRecord(int recordId, const QString &recordNameId, const QString &tableName)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("UPDATE %1 SET LockedUp = NULL , LockedBy = NULL WHERE %2 = %3")
                    .arg(tableName)
                    .arg(recordNameId)
                    .arg(recordId));
    if(!ok)
        error = query.lastError();
    return ok;
}

bool LockDataBase::unlockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName)
{
    if(!recordsId.isEmpty()){
        QString recordsForUnlock = "";
        QSqlQuery query;
        bool ok;
        recordsForUnlock = QString("%1").arg(recordsId.value(0));
        for(int i = 1;i<recordsId.count();i++)
            recordsForUnlock += QString(",%1").arg(recordsId.value(i));
        ok = query.exec(QString("UPDATE %1 SET LockedUp = NULL, LockedBy = NULL WHERE %2 IN (%3)")
                        .arg(tableName)
                        .arg(recordNameId)
                        .arg(recordsForUnlock));
        if(!ok)
            error = query.lastError();
        return ok;
    }else
        return false;
}

bool LockDataBase::lockReferenceBook(const QString &referenceBookName)
{
    QSqlQuery query;
    bool ok;
    QSqlDatabase curDb = QSqlDatabase::database();
    ok = query.exec(QString("SELECT * FROM lockreferencebook WHERE Name = '%1'").arg(referenceBookName));
    if(ok){
        if(query.size() > 0){
            ok = query.exec(QString("UPDATE lockreferencebook SET LockedUp = (SELECT date_add(current_timestamp(),INTERVAL 1 MINUTE)), LockedBy = '%2'"
                                    " WHERE Name = '%1'").arg(referenceBookName).arg(curDb.userName()));
        }else{
            ok = query.exec(QString("INSERT INTO lockreferencebook (Name, LockedUp, LockedBy) VALUES ('%1',"
                                    " (SELECT date_add(current_timestamp (), INTERVAL 1 MINUTE)), '%2');")
                            .arg(referenceBookName)
                            .arg(curDb.userName()));
        }
    }
    if(!ok)
        error = query.lastError();
    return ok;
}

bool LockDataBase::unlockReferenceBook(const QString &referenceBookName)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("DELETE FROM lockreferencebook WHERE Name = '%1'").arg(referenceBookName));
    if(!ok)
        error = query.lastError();
    return ok;
}

bool LockDataBase::recordIsLosked(int recordId, const QString &recordNameId, const QString &tableName)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT LockedUp, LockedBy FROM %1 WHERE %2 = %3 AND LockedUp > (SELECT current_timestamp ())")
                    .arg(tableName)
                    .arg(recordNameId)
                    .arg(recordId));
    if(!ok){
        error = query.lastError();
        return ok;
    }
    if(query.size() > 0){
        query.next();
        recordUserBlocking = query.value(1).toString();
        return true;
    }else
        return false;
}

bool LockDataBase::recordsIsLosked(QList<int> recordsId, const QString &recordNameId, const QString &tableName)
{
    QSqlQuery query;
    QString recordsForLock = "";
    bool ok;
    recordsForLock = QString("%1").arg(recordsId.value(0));
    for(int i = 1;i<recordsId.count();i++)
        recordsForLock += QString(",%1").arg(recordsId.value(i));
    ok = query.exec(QString("SELECT LockedUp, LockedBy FROM %1 WHERE %2 IN (%3) AND LockedUp > (SELECT current_timestamp ())")
                    .arg(tableName)
                    .arg(recordNameId)
                    .arg(recordsForLock));
    if(!ok){
        error = query.lastError();
        return ok;
    }
    if(query.size() > 0){
        query.next();
        recordUserBlocking = query.value(1).toString();
        return true;
    }else
        return false;
}

bool LockDataBase::referenceBookIsLocked(const QString &referenceBookName)
{
    QSqlQuery query;
    bool ok;
    ok = query.exec(QString("SELECT LockedUp, LockedBy FROM lockreferencebook WHERE Name = '%1' AND LockedUp > (SELECT current_timestamp ())")
                    .arg(referenceBookName));
    if(!ok){
        error = query.lastError();
        return ok;
    }
    if(query.size() > 0){
        query.next();
        referenceBookUserBlocking = query.value(1).toString();
        return true;
    }else
        return false;
}

QString LockDataBase::recordBlockingUser()
{
    return recordUserBlocking;
}

QString LockDataBase::referenceBookBlockingUser()
{
    return referenceBookUserBlocking;
}

QSqlError LockDataBase::lastError()
{
    return error;
}

void LockDataBase::setError(int recordId, const QString &errorText)
{
    error.setDatabaseText(errorText);
    error.setType(QSqlError::UnknownError);
    QWidget *parent = qobject_cast<QWidget *>(m_parent);
    if(recordId != 0)
        stopLockRecordThread(recordId);
    else
        stopLockListRecordThread();
    QMessageBox::warning(parent,tr("Ошибка блокировки!!!"),
                         tr("Не удалось заблокировать запись:\n %1\n").arg(errorText),
                         tr("Закрыть"));
}

void LockDataBase::setErrorRef(const QString &referenceBookName, const QString &errorText)
{
    error.setDatabaseText(errorText);
    error.setType(QSqlError::UnknownError);
    QWidget *parent = qobject_cast<QWidget *>(m_parent);
    stopLockReferenceBookThread(referenceBookName);
    QMessageBox::warning(parent,tr("Ошибка блокировки!!!"),
                         tr("Не удалось заблокировать справочник %2:\n %1\n").arg(errorText).arg(referenceBookName),
                         tr("Закрыть"));
}
