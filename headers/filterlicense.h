#ifndef FILTERLICENSE_H
#define FILTERLICENSE_H

#include <QComboBox>
#include "ui_filterlicense.h"

struct ItemComboBox{
    QString name;
    QString data;
};
Q_DECLARE_TYPEINFO(ItemComboBox, Q_PRIMITIVE_TYPE);

class FilterLicense : public QDialog, private Ui::FilterLicense
{
    Q_OBJECT

public:
    explicit FilterLicense(QWidget *parent = 0, QMap<int,QString> organizations = QMap<int,QString>(), int curOrgId = 0);
private:
    int m_curOrgId;
    int sizeConditionNames;
    void insertRowInAdditionalFilter(int index);
    QList< QList<ItemComboBox> > initItemAdditionalFilter();
    QList< QList<ItemComboBox> > items;
    void populateComboBoxAdditionalFilter(QComboBox *cbox, const QList< QList<ItemComboBox> > itemCBox, int groupItemIndex);
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
    void setSelectedData(QList<QVariant> datas);
    void on_noBindingOaH_clicked(bool checked);
    void on_filterOaH_runButtonClicked();
    void on_setButton_clicked();
    void on_organization_currentIndexChanged(int);
};

#endif // FILTERLICENSE_H
