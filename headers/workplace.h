#ifndef WORKPLACE_H
#define WORKPLACE_H

#include <QtSql>
#include "ui_workplace.h"

class SqlTreeModel;
class WorkPlaceModel;

class WorkPlace : public QWidget, private Ui::WorkPlace {
    Q_OBJECT
public:
    WorkPlace(QWidget *parent = 0);
private:
    QString tablName;
    SqlTreeModel *modelDepartment;
    WorkPlaceModel *wpModel;
    int moveMode, moveWpMode;
    int newParentId, newDepWpId;
    bool delMode, cancelMove;
    QMenu *menu;
    QMenu *menu2;
    void populateWPModel(const QString &filter = "");
    bool isFirm(const QModelIndex &ind);
    bool isFilPred(const QModelIndex &ind);
    void moveWpDep(bool depMode, bool wpMode);
signals:
    void closeWorkPlaceWin();
protected:
    void changeEvent(QEvent *e);
private slots:
    void wpDataModelUpdate();
    void newWpAdded(const QString &wpName, const QString &wpInternalNum, const QString &wpLocation, bool openEdit, int wh);
    void onWPMenu(const QPoint &p);
    void onDepMenu(const QPoint &p);
    void doubleClickedDepView(const QModelIndex &index);
    void on_wpSubsidiaryDep_clicked();
    void on_actionDelWP_triggered();
    void on_actionMoveWPAll_triggered();
    void on_actionMoveWP_triggered();
    void on_actionAddFilPred_triggered();
    void on_actionAddNewFirm_triggered();
    void on_actionEditWP_triggered();
    void on_actionAddWP_triggered();
    void on_actionMoveDepart_triggered();
    void on_actionEditDepart_triggered();
    void on_actionDelDepart_triggered();
    void on_actionAddDepart_triggered();
    void on_close_clicked();
    void setMoveMode(int newP, int moveM);
    void setMoveWpMode(int newD, int moveWpM);
    void updateWPModel(const QModelIndex &idx = QModelIndex());
    void addDepartment(const QString &name, bool multiMode);
    void editDepartment(const QString &name);
    void addNewFirm(const QString &name, int id);
    void addNewFilPred(const QString &name, int id);
    void sortWPClick();
};

#endif // WORKPLACE_H
