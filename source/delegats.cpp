#include <QDir>
#include <QListView>
#include <QPainter>

#include "headers/delegats.h"
#include "headers/filelistmodel.h"

DateEditDelegate::DateEditDelegate(
        bool calpopup,
        QObject *parent)
               : QItemDelegate(parent),
               m_calpopup(calpopup) {
}

QWidget *DateEditDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex& /* index */) const {
    QDateEdit *editor = new QDateEdit(parent);
    editor->setCalendarPopup(m_calpopup);
    editor->installEventFilter(const_cast<DateEditDelegate*>(this));
    editor->setDate(QDate::currentDate());
    return editor;
}

void DateEditDelegate::setEditorData(
        QWidget *editor,
        const QModelIndex &index) const {
    QDate value = index.model()->data(
            index, Qt::EditRole).toDate();
    QDateEdit *de = static_cast<QDateEdit*>(editor);
    de->setDate(value);
}

void DateEditDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex& index) const {
    QDateEdit *de = static_cast<QDateEdit*>(editor);
    de->interpretText();
    QDate value = de->date();
    model->setData(index, value);
}

void DateEditDelegate::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
}

//--------------------------------------------------------

DoubleSpinBoxDelegat::DoubleSpinBoxDelegat(
        double min,
        double max,
        double step,
        int precision,
        QObject *parent)
                : QItemDelegate(parent),
                m_min(min),
                m_max(max),
                m_step(step),
                m_precision(precision) {
}

QWidget *DoubleSpinBoxDelegat::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex& /* index */) const {
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
    editor->setMinimum(m_min);
    editor->setMaximum(m_max);
    editor->setDecimals(m_precision);
    editor->setSingleStep(m_step);
    editor->setSpecialValueText("Нет");
    editor->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    editor->installEventFilter(const_cast<DoubleSpinBoxDelegat*>(this));
    return editor;
}

void DoubleSpinBoxDelegat::setEditorData(
        QWidget *editor,
        const QModelIndex &index) const {
    double value = index.model()->data(
            index, Qt::EditRole).toDouble();
    QDoubleSpinBox *dsb = static_cast<QDoubleSpinBox*>(editor);
    dsb->setValue(value);
}

void DoubleSpinBoxDelegat::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex& index) const {
    QDoubleSpinBox *dsb = static_cast<QDoubleSpinBox*>(editor);
    dsb->interpretText();
    double value = dsb->value();
    model->setData(index, value);
}

void DoubleSpinBoxDelegat::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
}

void DoubleSpinBoxDelegat::paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const {

    QString text;
    QRect rect;
    QVariant value;
    QStyleOptionViewItem opt = setOptions(index, option);
    value = index.data(Qt::DisplayRole);
    if(value.toDouble() != 0.00)
        text = QLocale().toString(value.toDouble(), 'f', 2);
    else
        text = tr("Нет");
    opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    drawDisplay(painter, opt, opt.rect, text);
    drawFocus(painter, opt, opt.rect);
}

//--------------------------------------------------------

SpinBoxDelegat::SpinBoxDelegat(int min, int max, int step, QObject *parent)
    : QItemDelegate(parent),
    m_min(min),
    m_max(max),
    m_step(step){}

QWidget *SpinBoxDelegat::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex& /* index */) const {
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(m_min);
    editor->setMaximum(m_max);
    editor->setSingleStep(m_step);
    editor->setSpecialValueText("Нет");
    editor->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    editor->installEventFilter(const_cast<SpinBoxDelegat*>(this));
    return editor;
}

void SpinBoxDelegat::setEditorData(
        QWidget *editor,
        const QModelIndex &index) const {
    int value = index.model()->data(
            index, Qt::EditRole).toInt();
    QSpinBox *dsb = static_cast<QSpinBox*>(editor);
    dsb->setValue(value);
}

void SpinBoxDelegat::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex& index) const {
    QSpinBox *dsb = static_cast<QSpinBox*>(editor);
    dsb->interpretText();
    int value = dsb->value();
    model->setData(index, value);
}

void SpinBoxDelegat::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
}

void SpinBoxDelegat::paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const {

    QString text;
    QRect rect;
    QVariant value;
    QStyleOptionViewItem opt = setOptions(index, option);
    value = index.data(Qt::DisplayRole);
    if(value.toInt() != 0)
        text = QLocale().toString(value.toInt());
    else
        text = tr("Нет");
    opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    drawDisplay(painter, opt, opt.rect, text);
    drawFocus(painter, opt, opt.rect);
 }

//--------------------------------------------------------

TypeDevIconDelegat::TypeDevIconDelegat(QObject *parent/*, int maxLen*/)
                : QItemDelegate(parent)/*,
                m_maxLen(maxLen)*/{
//    model = new FileListModel();
//    model->setDirPath(QDir::currentPath()+"/ico/typedeviceico/");
}

QWidget *TypeDevIconDelegat::createEditor(
        QWidget */*parent*/,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex& /* index */) const {

//    QListView *editor = new QListView(parent);
//    editor->setModel(model);
//    editor->setIconSize(QSize(32,32));
//    editor->setViewMode(QListView::IconMode);
//    editor->installEventFilter(const_cast<TypeDevIconDelegat*>(this));
//    return editor;
    return 0;
}

void TypeDevIconDelegat::setEditorData(
        QWidget */*editor*/,
        const QModelIndex &/*index*/) const {
//    QString value = index.model()->data(index, Qt::EditRole).toString();
//    QListView *dsb = static_cast<QListView*>(editor);
//    if(!value.isNull() || !value.isEmpty())
//        dsb->setCurrentIndex(model->findItem(value));
}

void TypeDevIconDelegat::setModelData(
        QWidget */*editor*/,
        QAbstractItemModel */*model*/,
        const QModelIndex& /*index*/) const {
//    QListView *dsb = static_cast<QListView*>(editor);
//    QString value = dsb->currentIndex().data(Qt::EditRole).toString();
//    model->setData(index, value);
}

void TypeDevIconDelegat::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
}

QSize TypeDevIconDelegat::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QString text;
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);
    QRect decorationRect = rect(option, index, Qt::DecorationRole);
    QRect displayRect = QRect(option.rect.left(),option.rect.top(),32,32);
    QRect checkRect = rect(option, index, Qt::CheckStateRole);
    doLayout(option, &checkRect, &decorationRect, &displayRect, true);
    return (decorationRect|displayRect|checkRect).size();
}

void TypeDevIconDelegat::paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const {
    QString text;
    QVariant value;
    QStyleOptionViewItem opt = setOptions(index, option);
    value = index.data(Qt::EditRole);
    text = value.toString();
    opt.displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
    QPixmap pixmap;
    pixmap.load(QDir::currentPath()+"/ico/typedeviceico/"+text);
    if(text.isEmpty() || text.isNull())
        drawDisplay(painter, opt, opt.rect, text);
    else
        drawDecoration(painter, opt, opt.rect,pixmap.scaled(32,32));
    drawFocus(painter, opt, opt.rect);
 }

//--------------------------------------------------------

TypeDevDelegat::TypeDevDelegat(QObject *parent) : QItemDelegate (parent){}

QWidget *TypeDevDelegat::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex& /* index */) const {
    QComboBox *editor = new QComboBox(parent);
    editor->addItem(tr("Оргтехника"),1);
    editor->addItem(tr("Комплектующее"),0);
    editor->setMaxCount(2);
    editor->setMaxVisibleItems(2);
    editor->show();
    return editor;
}

void TypeDevDelegat::setEditorData(
        QWidget *editor,
        const QModelIndex &index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox *dsb = static_cast<QComboBox*>(editor);
    dsb->setCurrentIndex(dsb->findData(value));
}

void TypeDevDelegat::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex& index) const {
    QComboBox *dsb = static_cast<QComboBox*>(editor);
    int value = dsb->itemData(dsb->currentIndex()).toInt();
    model->setData(index, value);
}

void TypeDevDelegat::updateEditorGeometry(
        QWidget *editor,
        const QStyleOptionViewItem &option,
        const QModelIndex& /* index */) const {
    editor->setGeometry(option.rect);
}

QSize TypeDevDelegat::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QString text;
    QVariant value = index.data(Qt::SizeHintRole);
    if (value.isValid())
        return qvariant_cast<QSize>(value);
    value = index.data(Qt::DisplayRole);
    if(value.toInt() == 1)
        text = tr("Оргтехника");
    else
        text = tr("Комплектующее");
    QRect decorationRect = rect(option, index, Qt::DecorationRole);
    QRect displayRect = QFontMetrics( option.font ).boundingRect(text+" ");
    QRect checkRect = rect(option, index, Qt::CheckStateRole);
    doLayout(option, &checkRect, &decorationRect, &displayRect, true);
    return (decorationRect|displayRect|checkRect).size();
}

void TypeDevDelegat::drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const
{
    QStyleOptionViewItem opt = option;
    opt.displayAlignment |= Qt::AlignCenter | Qt::AlignVCenter;
    QString text1;
    if(text.toInt() == 1)
        text1 = tr("Оргтехника");
    else
        text1 = tr("Комплектующее");
    QItemDelegate::drawDisplay ( painter, opt, rect, text1);
}
