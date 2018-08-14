#include "headers/lockdatabase.h"

LockDataBase::LockDataBase(QObject *parent) :
    QObject(parent)
{
    recordUserBlocking = "";
    referenceBookUserBlocking = "";
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
