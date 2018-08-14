#ifndef SELECTPO_H
#define SELECTPO_H

#include "ui_selectpo.h"

class PoModel;

class SelectPo : public QDialog, private Ui::SelectPo
{
    Q_OBJECT

public:
    explicit SelectPo(QWidget *parent = 0, const QString &filter = "", bool selectOnlyPo = false, bool showEditButton = false);
private:
    PoModel *poModel;
    QString curFilter;
    bool m_selectOnlyPo;
    void populatePoModel(const QString &filter);
signals:
    void itemSelected(int selectedItemId);
    void itemSelected(const QList<QVariant> &poData);
protected:
    void changeEvent(QEvent *e);
private slots:
    void on_selectButton_clicked();
    void on_buttonEditPo_clicked();
};

#endif // SELECTPO_H
