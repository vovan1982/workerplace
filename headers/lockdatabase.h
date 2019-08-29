#ifndef LOCKDATABASE_H
#define LOCKDATABASE_H

#include <QtSql>
#include <QObject>

class LockDataBaseWorker;

class LockDataBase : public QObject
{
    Q_OBJECT
public:
    explicit LockDataBase(QObject *parent = 0);
    ~LockDataBase();
    bool recordIsLosked(int recordId, const QString &recordNameId, const QString &tableName);
    bool recordsIsLosked(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool referenceBookIsLocked(const QString &referenceBookName);
    void lockRecordThread(int recordId, const QString &recordNameId, const QString &tableName);
    void lockListRecordThread(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    void lockReferenceBookThread(const QString &referenceBookName);
    void stopLockRecordThread(int recordId);
    void stopLockListRecordThread();
    void stopLockReferenceBookThread(const QString &referenceBookName);
    QString recordBlockingUser();
    QString referenceBookBlockingUser();
    QSqlError lastError();
private:
    QObject *m_parent;
    QString recordUserBlocking, referenceBookUserBlocking;
    QSqlError error;
    QList<LockDataBaseWorker*> lockedRecInThreads, lockedListRecInThreads, lockedReferenceBookInThreads;
    QMap<QString,QVariant> credentials;
signals:
    void stopAllThread();
public slots:
    bool lockRecord(int recordId, const QString &recordNameId, const QString &tableName);
    bool lockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool unlockRecord(int recordId, const QString &recordNameId, const QString &tableName);
    bool unlockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool lockReferenceBook(const QString &referenceBookName);
    bool unlockReferenceBook(const QString &referenceBookName);

private slots:
    void setError(int recordId, const QString &errorText);
    void setErrorRef(const QString &referenceBookName , const QString &errorText);
};

#endif // LOCKDATABASE_H
