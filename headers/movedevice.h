#ifndef MOVEDEVICE_H
#define MOVEDEVICE_H

#include <QSqlDatabase>
#include "ui_movedevice.h"

class QStandardItemModel;
class DeviceModel;
class LockDataBase;

class MoveDevice : public QDialog, private Ui::MoveDevice {
    Q_OBJECT
public:
    MoveDevice(QWidget *parent = 0, bool singlMode = false,
               const QList<QVariant> &curWPData = QList<QVariant>(),
               const QList<QVariant> &devData = QList<QVariant>(),
               bool addWPMode = false, bool moveFromWp = false);
private:
    QSqlDatabase db;
    bool m_singlMode,m_addWPMode,m_moveFromWp;
    int curOrgId, curFilPredId, curDepId, curWPId;
    int newOrgId, newFilPredId, newDepId, newWPId;
    int orgTexId, typeDevForMove;
    QStandardItemModel *moveDevModel;
    DeviceModel *devModel;
    LockDataBase *lockedControl, *lockedControlLicense;
    QList<int> lockedLicenseId;
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    bool dataForMoveEntered();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void devIsMoved();
    void devIsMoved(int type, int orgTId);
    void lockedError(const QString &msg, bool updateModel);
private slots:
    void on_moveDev_clicked();
    void on_delDeviceForMove_clicked();
    void on_cancel_clicked();
    void on_addDeviceForMove_clicked();
    void on_selectNew_clicked();
    void on_selectCur_clicked();
    void fillCurPlace(int wpId, const QString &wpName, int orgId);
    void fillNewPlace(int wpId, const QString &wpName, int orgId);
    void setOrgTex(const QList<QVariant> &orgTex);
    void setPerformer(const QString &performerName, int performerId);
    void addDevForMove(const QList<QVariant> &dev);
    void on_newOrgTex_runButtonClicked();
    void on_note_textChanged();
    void on_buttonEditCause_clicked();
    void on_performer_runButtonClicked();
    void on_cause_currentIndexChanged(int);
    void updateLockRecord();
//    void updateLockLicenseRecord();
    void on_closeButton_clicked();
};

#endif // MOVEDEVICE_H
