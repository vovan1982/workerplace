#ifndef INVENTORYREPORTWORKER_H
#define INVENTORYREPORTWORKER_H

#include <QtSql>
//#include <QSqlError>
//#include <QSqlDatabase>
#include <QObject>
#include <QMap>

class CreateInventoryReport;
class DeviceModelControl;

class InventoryReportWorker : public QObject
{
    Q_OBJECT
public:
    explicit InventoryReportWorker(const QMap<QString, QMap<QString, int> > &data = QMap<QString, QMap<QString, int> >(), const QSqlDatabase &connectionData = QSqlDatabase());
    ~InventoryReportWorker();
private:
    QMap<QString, QMap<QString, int> > m_data;
    QSqlDatabase m_connectionData;
    QSqlError lastErr;
    struct departments {
        QString name;
        int id;
    };
    int departmentId,workPlaceId;
    CreateInventoryReport *cInvRep;
    DeviceModelControl *orgTexModel;
    void populateModDep(int organizationId);
    QList<InventoryReportWorker::departments> depList(int parentId, QString depName, const QSqlDatabase &connection, int iteranion = 0);
signals:
    void finished();
    void reportCreated(CreateInventoryReport* report);
    void progresstext(QString text);
public slots:
    void process();
};

#endif // INVENTORYREPORTWORKER_H
