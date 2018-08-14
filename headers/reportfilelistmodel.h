#ifndef REPORTFILELISTMODEL_H
#define REPORTFILELISTMODEL_H

#include <QtWidgets>
#include <QObject>

class QAbstractListModel;
class ReportFileItem;

class ReportFileListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ReportFileListModel(const QVector<ReportFileItem *> &fileItems = QVector<ReportFileItem*>(), QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant getDescription(const QModelIndex &index) const;
    QVariant getPath(const QModelIndex &index) const;
private:
    QVector<ReportFileItem*> items;
};

#endif // REPORTFILELISTMODEL_H
