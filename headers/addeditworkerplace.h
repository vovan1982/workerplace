#ifndef ADDEDITWORKERPLACE_H
#define ADDEDITWORKERPLACE_H

#include <QtSql>
#include "ui_addeditworkerplace.h"

class Cusers;
class Device;
class Licenses;
class LockDataBase;

class AddEditWorkerPlace : public QDialog, private Ui::AddEditWorkerPlace {
    Q_OBJECT
public:
    AddEditWorkerPlace(QWidget *parent = 0, int wpFirmId = 0, const QMap<int,QVariant> record = QMap<int,QVariant>(),
                       bool readOnly = false, const QString &tableName = "");
private:
    int m_wpId, m_wpDepId;
    QString m_wpName;
    int m_wpPUserId, tempWpPUserId;
    QString m_wpPUser, m_wpIntNum, m_wpLocation;
    int m_wpWareHouse, m_wpRV;
    bool m_readOnly;
    QString m_tableName;
    LockDataBase *lockedControl;
    Cusers *wUsers;
    Device *wDevice;
    Licenses *wLicenses;
    bool formIsEdited();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
signals:
    void wpDataChange();
private slots:
    void on_delPUserButton_clicked();
    void on_saveButton_clicked();
    void on_location_textEdited(QString text);
    void on_internalNumber_textEdited(QString text);
    void on_primaryUser_textChanged(QString text);
    void on_cancelButton_clicked();
    void on_name_textEdited(QString text);
    void setPrimaryUser(const QString &fio, int id);
    void updatePrimaryUser();
    void on_closeButton_clicked();
    void on_buttonHistoryUsers_clicked();
    void on_buttonHistoryDevice_clicked();
};

#endif // ADDEDITWORKERPLACE_H
