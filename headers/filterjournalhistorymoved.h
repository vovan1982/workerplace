#ifndef FILTERJOURNALHISTORYMOVED_H
#define FILTERJOURNALHISTORYMOVED_H

#include <QComboBox>
#include <QLabel>
#include "ui_filterjournalhistorymoved.h"

struct ItemComboBox{
    QString name;
    QString data;
};
Q_DECLARE_TYPEINFO(ItemComboBox, Q_PRIMITIVE_TYPE);

class FilterJournalHistoryMoved : public QDialog, private Ui::FilterJournalHistoryMoved
{
    Q_OBJECT
    
public:
    explicit FilterJournalHistoryMoved(QWidget *parent = 0, bool filterIsEmpty = true, int organizationId = 0);
private:
    int sizeConditionNames;
    bool m_filterIsEmpty;
    int m_organizationId;
    void insertRowInAdditionalFilter(int index);
    QList< QList<ItemComboBox> > initItemAdditionalFilter();
    QList< QList<ItemComboBox> > items;
    void populateComboBoxAdditionalFilter(QComboBox *cbox, const QList< QList<ItemComboBox> > itemCBox, int groupItemIndex);
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
    int sizeConditionName(QList< QList<ItemComboBox> > itemCondition, int conditionGroupFrom, int conditionGroupTo);
protected:
    void changeEvent(QEvent *e);
signals:
    void setFilter(const QString &filter);
private slots:
    void delFilterRow();
    void addButton_clicked();
    void fieldAdditionalFilterChanget(int index);
    void additionalButton_clicked();
    void conditionAdditionalFilterChanget(const QString &index);
    void setSelectedDataAdditionalFilter(const QList<QVariant> datas);
    void on_setButton_clicked();
};

#endif // FILTERJOURNALHISTORYMOVED_H
