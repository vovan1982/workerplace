#ifndef CUSERS_H
#define CUSERS_H

#include "ui_users.h"

class SqlTreeModel;
class WorkPlaceModel;
class QSqlTableModel;
class QSqlRelationalTableModel;
class QDataWidgetMapper;
class QSortFilterProxyModel;

class Cusers : public QWidget, private Ui::Cusers {
    Q_OBJECT
    Q_DISABLE_COPY(Cusers)
public:
    explicit Cusers(QWidget *parent = 0, bool wpMode = false, int firmId = 0, int wpId = 0, bool readOnly = false);
private:
    QWidget *m_parent;
    bool depBoxCurState;
    QString dateOut;
    QModelIndex depCurIndex;
    QMenu *menuLoginPass;
    QMenu *menuNumber;
    QMenu *menuWorkPlace;
    SqlTreeModel *modelFilPredDep;
    QSqlRelationalTableModel *model;
    QSqlRelationalTableModel *nullModel;
    QSqlRelationalTableModel *loginModel;
    QSqlRelationalTableModel *numberModel;
    WorkPlaceModel *wpModel;
    QSqlTableModel *toUnitModel;
    QDataWidgetMapper *mapper;
    QSortFilterProxyModel *proxyModel;
    bool m_wpMode;
    int m_firmId, m_wpId;
    bool m_readOnly;
    void populateModel(int postFilter, int orgFilter);
    void populateLoginModel(int filter);
    void populateNumberModel(int filter);
    void populateWPModel(int filter);
    void populateToUnitModel(int filter);
    void clearUserForm();
    void userModelEmpty(bool ok);
    QModelIndex realModelIndex(const QModelIndex &viewIndex) const;
    QModelIndex realViewIndex(const QModelIndex &modelIndex) const;
    void setReadOnly(bool ok);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    void updateCurUserRowVersion();
    bool isLockedOrChanged();
signals:
    void updatePrimaryUser();
    void closeUserWin(bool);
    void primaryUserSet(const QString &name, int id);
private slots:
    void onMenu(const QPoint &p);
    void filPredDep();
    void changeDepIndex(QModelIndex ind);
    void sortClick();
    void setUserFilter();
    void updateMaperIndex(const QModelIndex &index);
    void loginPass();
    void numberViewUpdate();
    void wpViewUpdate();
    void sortLoginClick();
    void sortNumberClick();
    void sortWPClick();
    void setProxyFilter(const QString &filter);
    void setDateOut(const QString &date);
    void on_groupBoxDep_clicked(bool checked);
    void on_addButton_clicked();
    void on_delWPButton_clicked();
    void on_addWPButton_clicked();
    void on_closeButton_clicked();
    void on_delUser_clicked();
    void on_setPrimaryUserButton_clicked();
    void on_buttonAddUser_clicked();
    void on_buttonAddLoginPass_clicked();
    void on_buttonDelLoginPass_clicked();
    void on_buttonEditLoginPass_clicked();
    void on_buttonAddNumber_clicked();
    void on_buttonDelNumber_clicked();
    void on_buttonEditNumber_clicked();
    void on_buttonSaveDep_clicked();
    void on_actionAddLoginPass_triggered();
    void on_actionDelLoginPass_triggered();
    void on_actionEditLoginPass_triggered();
    void on_actionAddNumber_triggered();
    void on_actionDelNumber_triggered();
    void on_actionEditNumber_triggered();
    void on_actionAddWP_triggered();
    void on_actionDelWP_triggered();
    void on_buttonEdit_clicked();
    void on_buttonHistory_clicked();

protected:
    virtual void changeEvent(QEvent *e);
};

#endif // CUSERS_H
