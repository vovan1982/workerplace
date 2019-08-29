#ifndef LICENSEMODELWORKER_H
#define LICENSEMODELWORKER_H

#include <QMap>
#include <QtSql>
#include <QObject>
#include <QVariant>

class TreeItem;

class LicenseModelWorker : public QObject
{
    Q_OBJECT
public:
    explicit LicenseModelWorker(const QMap<QString,QVariant> &credentials = QMap<QString,QVariant>(),
                                const QMap<QString,QString> &forQuery = QMap<QString,QString>(),
                                const QVector<QVariant> &rootData = QVector<QVariant>(),
                                const QMap<QString,int> &indexColumn = QMap<QString,int>(),
                                bool showParent = false);
    ~LicenseModelWorker();
    void setFilter(const QString &queryFilter);
    void setShowParent(bool sp);

private:
    QString primaryQuery, filter;
    QMap<QString,QVariant> m_credentials;
    QVector<QVariant> m_rootData;
    QMap<QString,int> m_indexColumn;
    bool m_showParent;

signals:
    void finished();
    void logData(QString text);
    void error(QString errorText);
    void result(TreeItem* licenses);

public slots:
    void process();
};

#endif // LICENSEMODELWORKER_H
