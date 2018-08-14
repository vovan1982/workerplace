#ifndef FILTERDEVICE_H
#define FILTERDEVICE_H

#include <QComboBox>
#include <QLabel>
#include "ui_filterdevice.h"

struct ItemComboBox{
    QString name;
    QString data;
};
Q_DECLARE_TYPEINFO(ItemComboBox, Q_PRIMITIVE_TYPE);

class FilterDevice : public QDialog, private Ui::FilterDevice
{
    Q_OBJECT
    
public:
    explicit FilterDevice(QWidget *parent = 0, bool locationIsSet = false, bool onlyHardwareMode = false, bool onlyOrgTexMode = false);
private:
    int sizeConditionNames;
    bool m_locationIsSet, m_onlyHardwareMode, m_onlyOrgTexMode, selectDepIsFirm;
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
    void setSelectedData(int id, const QString &name, int orgId);
    void on_groupBox_clicked(bool checked);
    void on_filterDepartment_runButtonClicked();
    void on_filterWorkPlace_runButtonClicked();
    void on_filterType_currentIndexChanged(int index);
    void on_setButton_clicked();
    void on_additionalFilteringRules_clicked(bool checked);
};

#endif // FILTERDEVICE_H
