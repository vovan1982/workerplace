#ifndef MOVETREEITEM_H
#define MOVETREEITEM_H

#include <QtSql>
#include "ui_movetreeitem.h"

class MoveTreeItem : public QDialog, private Ui::MoveTreeItem {
    Q_OBJECT
public:
    MoveTreeItem(QWidget *parent = 0, const QString &nameItem = "",int itemId = 0, int itemParentId = 0,
                 const QString &tableName = "", int countRows = 0, bool delMode = false, bool filP = false,
                 int filPId = 0, bool moveDep = true, bool moveWp = false);
private:
    QString curFilter, filter1, filter2, tabName, curFilterWp, filterWp;
    QSqlQueryModel *modelDep;
    QSqlQueryModel *modelWp;
    int m_itemRowCount;
    bool m_moveDep, m_moveWp, m_delMode;
    void populateModel(const QString &filter, QSqlQueryModel *model);
protected:
    void changeEvent(QEvent *e);
signals:
    void newParentItem(int newItemId, int moveMode);
    void newParentItemWp(int newDepId, int moveWpMode);
private slots:
    void on_findWpButton_clicked();
    void on_onlyChildrenItem_toggled(bool checked);
    void on_moveButton_clicked();
    void on_findButton_clicked();
};

#endif // MOVETREEITEM_H
