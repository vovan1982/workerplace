#ifndef MOVELICENSE_H
#define MOVELICENSE_H

#include <QSqlDatabase>
#include "ui_movelicense.h"

class DeviceModelControl;
class QStandardItemModel;
class LicenseModel;
class LockDataBase;

class MoveLicense : public QDialog, private Ui::MoveLicense
{
    Q_OBJECT

public:
    explicit MoveLicense(QWidget *parent = 0, bool singlMode = false,
                         const QList<QVariant> &curWPData = QList<QVariant>(),
                         const QList<QVariant> &licData = QList<QVariant>(),
                         bool addWPMode = false, bool moveFromWp = false);
private:
    QSqlDatabase db;
    bool m_singlMode,m_addWPMode,m_moveFromWp;
    DeviceModelControl *devModel;
    QStandardItemModel *moveLicModel;
    LicenseModel *licModel;
    LockDataBase *lockedControl;
    QTimer* timer;
    int curOrgId, curFilPredId, curDepId, curWPWHId;
    int newOrgId, newFilPredId, newDepId, newWPWHId;
    QString curFilter, orgTexFilter;
    bool filterIsSet;
    bool accessForMove();
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void licIsMoved();
    void lockedError(const QString &msg, bool updateModel);
private slots:
    void fillCurPlace(QList<QVariant> curPlaceData);
    void fillNewPlace(QList<QVariant> newPlaceData);
    void setOrgTexFilter(const QString &filter);
    void addLicForMove(const QList<QVariant> &lic);
    void on_selectCurButton_clicked();
    void on_selectNewButton_clicked();
    void on_groupBoxDev_clicked(bool checked);
    void on_addFiltrButton_clicked();
    void on_clearFilterButton_clicked();
    void on_addLicButton_clicked();
    void on_clearLicButton_clicked();
    void on_delLicButton_clicked();
    void on_moveButton_clicked();
    void on_buttonEditCause_clicked();
    void on_performer_runButtonClicked();
    void setPerformer(const QString &performerName, int performerId);
    void on_cause_currentIndexChanged(int);
    void on_note_textChanged();
    void deviceModelIsPopulated();
    void updateLockRecord();
};

#endif // MOVELICENSE_H
