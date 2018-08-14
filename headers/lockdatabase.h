#ifndef LOCKDATABASE_H
#define LOCKDATABASE_H

#include <QtSql>
#include <QObject>

class LockDataBase : public QObject
{
    Q_OBJECT
public:
    explicit LockDataBase(QObject *parent = 0);
    bool recordIsLosked(int recordId, const QString &recordNameId, const QString &tableName);
    bool recordsIsLosked(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool referenceBookIsLocked(const QString &referenceBookName);
    QString recordBlockingUser();
    QString referenceBookBlockingUser();
    QSqlError lastError();
private:
    QString recordUserBlocking, referenceBookUserBlocking;
    QSqlError error;
signals:
    
public slots:
    bool lockRecord(int recordId, const QString &recordNameId, const QString &tableName);
    bool lockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool unlockRecord(int recordId, const QString &recordNameId, const QString &tableName);
    bool unlockListRecord(QList<int> recordsId, const QString &recordNameId, const QString &tableName);
    bool lockReferenceBook(const QString &referenceBookName);
    bool unlockReferenceBook(const QString &referenceBookName);
};

#endif // LOCKDATABASE_H
