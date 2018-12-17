#ifndef LICENSEMODELCONTROL_H
#define LICENSEMODELCONTROL_H

#include <QObject>
#include <QTreeView>
#include <QSortFilterProxyModel>

class LicenseModel;

class LicenseModelControl : public QObject
{
    Q_OBJECT
public:
    explicit LicenseModelControl(QObject *parent = 0, QTreeView *view = 0, bool showParentDevice = false, const QString &modelFilter = "");
    void populateLicModel(const QString &filter);
    LicenseModel *model();
    QString filter();
    void clearLicFilter();
    void setLicFilterAndUpdate(const QString &filter);
    void setLicFilterWhithOutUpdate(const QString &filter);
    void setCurrentIndexFirstRow();
    void setShowParentDevice(bool show);
    bool showParentDevice();
    QModelIndex realModelIndex(const QModelIndex &viewIndex) const;
    QModelIndex realViewIndex(const QModelIndex &modelIndex) const;
private:
    LicenseModel *licModel;
    QTreeView *m_view;
    QSortFilterProxyModel *proxyModel;
    bool m_showParentDevice;
    QString licFilter;
signals:
    void licModelUpdated();
public slots:
    void updateLicModel();

};

#endif // LICENSEMODELCONTROL_H
