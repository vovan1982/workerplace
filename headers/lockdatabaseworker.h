#ifndef LOCKDATABASEWORKER_H
#define LOCKDATABASEWORKER_H

#include <QMap>
#include <QtSql>
#include <QObject>
#include <QVariant>

class LockDataBaseWorker : public QObject
{
    Q_OBJECT
public:
    explicit LockDataBaseWorker(int recordId = 0, const QString &recordNameId = "", const QString &tableName = "", const QMap<QString,QVariant> &credentials = QMap<QString,QVariant>());
    explicit LockDataBaseWorker(QList<int> recordsId = QList<int>(), const QString &recordNameId = "", const QString &tableName = "", const QMap<QString,QVariant> &credentials = QMap<QString,QVariant>());
    explicit LockDataBaseWorker(const QString &referenceBookName = "", const QMap<QString,QVariant> &credentials = QMap<QString,QVariant>());
    ~LockDataBaseWorker();
    int curRecordId();
    QList<int> curListRecordsId();
    QString curReferenceBookName();
    void setRunning(bool run);

private:
    QSqlDatabase db;
    int m_recordId;
    QList<int> m_recordsId;
    QString m_recordNameId, m_tableName, m_referenceBookName;
    QMap<QString,QVariant> m_credentials;
    QTimer* timer;
    bool running;

signals:
    void timerStopNow();
    void finished();
    void error(int recordId, const QString &errorText);
    void errorRef(const QString &referenceBookName, const QString &errorText);

public slots:
    void process();
    void stop();

private slots:
    void lockRec();
};

#endif // LOCKDATABASEWORKER_H
