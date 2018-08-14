#ifndef SELECTWORKPLACE_H
#define SELECTWORKPLACE_H

#include "ui_selectworkplace.h"

class SqlTreeModel;
class WorkPlaceModel;

class SelectWorkPlace : public QDialog, private Ui::SelectWorkPlace {
    Q_OBJECT
public:
    SelectWorkPlace(QWidget *parent = 0, int orgId = 0, bool devMode = false, bool orgTexMode = false, int wpId = 0);
private:
    SqlTreeModel *modelDepart;
    int m_orgId;
    bool m_devMode, m_orgTexMode;
    int m_wpId;
    WorkPlaceModel *wpModel;
    void populateModDep(int organizationId);
    void populateWPModel(const QString &filter = "");
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, QComboBox *cBox);
protected:
    void changeEvent(QEvent *e);
signals:
    void addWorkPlace(int wpId, const QString &wpName, int organizationId);
    void addWorkPlace(QList<QVariant> wpwhdata);
private slots:
    void on_organization_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_wpSubsidiaryDep_clicked();
    void updateWPModel(const QModelIndex &idx = QModelIndex());
    void sortWPClick();
};

#endif // SELECTWORKPLACE_H

