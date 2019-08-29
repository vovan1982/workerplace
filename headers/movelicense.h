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
    explicit MoveLicense(QWidget *parent = 0, bool singlMode = false,const QList<QVariant> &licData = QList<QVariant>());
private:
    QSqlDatabase db;
    bool m_singlMode;
    DeviceModelControl *devModel;
    QStandardItemModel *moveLicModel;
    LicenseModel *licModel;
    LockDataBase *lockedControl;
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
    void setOrgTex(const QList<QVariant> &dev);
    void clearDevModel();
    void addLicForMove(const QList<QVariant> &lic);
    void on_addLicButton_clicked();
    void on_clearLicButton_clicked();
    void on_delLicButton_clicked();
    void on_moveButton_clicked();
    void on_buttonEditCause_clicked();
    void on_performer_runButtonClicked();
    void setPerformer(const QString &performerName, int performerId);
    void on_cause_currentIndexChanged(int);
    void on_note_textChanged();
    void updateLockRecord();
    void on_newOrganization_currentIndexChanged(int index);
    void on_curOrganization_currentIndexChanged(int index);
    void on_selectDeviceButton_clicked();
    void on_removeDeviceButton_clicked();
};

#endif // MOVELICENSE_H
