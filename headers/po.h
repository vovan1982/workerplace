#ifndef PO_H
#define PO_H

#include "ui_po.h"

class PoModel;

class Po : public QWidget, private Ui::Po
{
    Q_OBJECT

public:
    explicit Po(QWidget *parent = 0, int curId = 0);
    void showCloseButton(bool show);
private:
    PoModel *poModel;
    QString iconFile;
    QModelIndex curIndex;
    QMenu *menu;
    bool readOnly;
    QTimer* timer;
    bool isGroup(const QModelIndex &index);
    void hideIfGroup(bool isGr);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    bool dataIsChanged();
signals:
    void closePoWin();
private slots:
    void onPoMenu(const QPoint &p);
    void populatePoModel();
    void updateEditForm(const QModelIndex &index);
    void setCurentProd(int idProd);
    void setCurentIcon(const QString &icoFile);
    void moveItem(int newParentId);
    void setAccessToActions();
    void on_closeButton_clicked();
    void on_actionAddGroup_triggered();
    void on_actionAddSubGroup_triggered();
    void on_actionAddPo_triggered();
    void on_actionMove_triggered();
    void on_actionDel_triggered();
    void on_saveButton_clicked();
    void on_editAndSelectProducersButton_clicked();
    void on_editIcoButton_clicked();
    void on_namePo_textEdited();
    void on_producer_currentIndexChanged(int);
    void on_note_textEdited();
    void updateLockReferenceBook();
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
};

#endif // PO_H
