#ifndef LICENSEMODELCONTROL_H
#define LICENSEMODELCONTROL_H

#include <QObject>
#include <QTreeView>

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
private:
    LicenseModel *licModel;
    QTreeView *m_view;
    bool m_showParentDevice;
    QString licFilter;
signals:
    void licModelUpdated();
public slots:
    void updateLicModel();

};

#endif // LICENSEMODELCONTROL_H
